//<LOCALE_TRANSLATE BY hxr /> 2015-1-10
#include "stdafx.h"
#include "LDS.h"
#include "NativePropertyList.h"
#include "Console.h"
#include "env_def.h"
#include "reportvectordlg.h"
#include "ReportPointDlg.h"
#include "CallBackDialog.h"
#include "ScrTools.h"

void InitDatumPointDesPara(CLDSPoint *pPoint)
{
	if(pPoint==NULL)
		return;
	if(pPoint->datum_pos_style==0)
		pPoint->datum_pos=pPoint->Position();
	else if(pPoint->datum_pos_style==1)
	{	//Ĭ�ϽǸ������֫��ƫ��
		if(console.FromPartHandle(pPoint->des_para.RODEND.hRod,CLS_LINEPART)==NULL)
			pPoint->des_para.RODEND.hRod=0;
		if(pPoint->des_para.RODEND.hRod==0)
		{
			pPoint->des_para.RODEND.wing_offset_style=4;
			pPoint->des_para.RODEND.wing_offset_dist=0;
		}
	}
	else if(pPoint->datum_pos_style==2)
	{
		if(console.FromNodeHandle(pPoint->des_para.RODNODE.hNode)==NULL)
			pPoint->des_para.RODNODE.hNode=0;
		if(console.FromPartHandle(pPoint->des_para.RODNODE.hRod,CLS_LINEPART)==NULL)
			pPoint->des_para.RODNODE.hRod=0;
		if(pPoint->des_para.RODNODE.hRod==0)
			pPoint->des_para.RODNODE.bFaceOffset=FALSE;
	}
	else if(pPoint->datum_pos_style==3||pPoint->datum_pos_style==4)
	{	//����pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2ʱ����Ӱ��hDatum1��hDatum2��ֵ���˴���Ҫ�����жϸ�ֵ
		if(console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEPART)==NULL)
			pPoint->des_para.AXIS_INTERS.hDatum1=0;
		if(console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEPART)==NULL)
			pPoint->des_para.AXIS_INTERS.hDatum2=0;
	}
	else if(pPoint->datum_pos_style==5)
	{
		if(console.FromPartHandle(pPoint->des_para.DATUMLINE.hDatumLine,CLS_LINEPART)==NULL)
		{
			CLDSLine *pLine=console.EnumLineFirst();
			pPoint->des_para.DATUMLINE.hDatumLine=(pLine!=NULL)?pLine->handle:0;
		}
	}
	else if(pPoint->datum_pos_style==6)
	{
		pPoint->datum_pos=pPoint->Position();	//ȡ֮ǰλ��Ϊ��׼λ�� wht 16-09-23
		if(console.FromPlaneHandle(pPoint->des_para.DATUMPLANE.hDatumPlane)==NULL)
		{
			CLDSPlane *pPlane=console.EnumPlaneFirst();
			pPoint->des_para.DATUMPLANE.hDatumPlane=(pPlane!=NULL)?pPlane->handle:0;
		}
		if(pPoint->des_para.DATUMPLANE.hDatumPlane==0)	//��׼����ָ����������㣬Ĭ��Ϊָ��X-Z����
			pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=1;
	}
	else if(pPoint->datum_pos_style==7)
	{
		if(console.FromPartHandle(pPoint->des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
			pPoint->des_para.RODEND.hRod=0;
	}
	else if(pPoint->datum_pos_style==8)
	{
		if(console.FromNodeHandle(pPoint->des_para.hNode)==NULL)
			pPoint->des_para.hNode=0;
	}
	else if(pPoint->datum_pos_style==9)
	{
		if(console.FromNodeHandle(pPoint->des_para.hPart)==NULL)
			pPoint->des_para.hPart=0;
	}
	else if(pPoint->datum_pos_style==10)
	{
		if(console.FromNodeHandle(pPoint->des_para.SCALE_NODE.hStartNode)==NULL)
			pPoint->des_para.SCALE_NODE.hStartNode=0;
		if(console.FromNodeHandle(pPoint->des_para.SCALE_NODE.hEndNode)==NULL)
			pPoint->des_para.SCALE_NODE.hEndNode=0;
	}
	else if(pPoint->datum_pos_style==11)
	{
		if(console.FromPartHandle(pPoint->des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE)==NULL)
			pPoint->des_para.COLUMN_INTERS.hLineTube=0;
	}
}

long CParaPointPropertyList::InitPropHashtable(long initId/* =1 */,long initHashSize/* =50 */,long initStatusHashSize/* =0 */)
{
	long id=CNativePropertyList::InitPropHashtable(initId,initHashSize,8);
	//������Ϣ
	SetPropKey("basic",id++);
	SetPropKey("hDatumPt",id++);
	SetPropKey("datum_pos_desc",id++);	//��׼������
	SetPropKey("datum_pos_style",id++);
	//������Ϣ
	SetPropKey("des_para",id++);
	SetPropKey("pos.x",id++);
	SetPropKey("pos.y",id++);
	SetPropKey("pos.z",id++);
	//��ڵ�ͬλ
	SetPropKey("des_para.hNode",id++);
	//ָ�������ϵ��������
	SetPropKey("des_para.hPart",id++);
	SetPropKey("datum_pos",id++);
	SetPropKey("datum_pos.x",id++);
	SetPropKey("datum_pos.y",id++);
	SetPropKey("datum_pos.z",id++);
	//���߽���
	SetPropKey("des_para.AXIS_INTERS.hDatum1",id++);
	SetPropKey("des_para.AXIS_INTERS.hDatum2",id++);
	SetPropKey("des_para.AXIS_INTERS.wing_offset_style1",id++);
	SetPropKey("des_para.AXIS_INTERS.wing_offset_style2",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	SetPropKey("des_para.AXIS_INTERS.wing_offset_dist1",id++);
	SetPropKey("des_para.AXIS_INTERS.wing_offset_dist2",id++);
	SetPropKey("des_para.AXIS_INTERS.near_vector",id++);
	SetPropKey("des_para.AXIS_INTERS.near_vector[0]",id++);
	SetPropKey("des_para.AXIS_INTERS.near_vector[1]",id++);
	SetPropKey("des_para.AXIS_INTERS.near_vector[2]",id++);
	SetPropKey("des_para.AXIS_INTERS.norm_offset",id++);	//����ƫ����;
	//�Ǹ����
	SetPropKey("des_para.RODEND.hRod",id++);				//�Ǹ־��
	SetPropKey("des_para.RODEND.direction",id++);			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	SetPropKey("des_para.RODEND.offset_wing",id++);		//0.X֫;1.Y֫
	SetPropKey("des_para.RODEND.wing_offset_style",id++);	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	SetPropKey("des_para.RODEND.wing_offset_dist",id++);
	SetPropKey("des_para.RODEND.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	SetPropKey("des_para.RODEND.bIncOddEffect",id++);		//����ƫ��ֵ��������ͷӰ��
	SetPropKey("des_para.RODEND.norm_offset",id++);		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	SetPropKey("des_para.LINEPARTNODE.hLinePart",id++);	//�Ǹ־��
	SetPropKey("des_para.LINEPARTNODE.hNode",id++);		//��׼�ڵ���
	SetPropKey("des_para.LINEPARTNODE.bFaceOffset",id++);
	SetPropKey("des_para.LINEPARTNODE.direction",id++);	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	SetPropKey("des_para.LINEPARTNODE.offset_wing",id++);	//0.X֫;1.Y֫
	SetPropKey("des_para.LINEPARTNODE.wing_offset_style",id++);//0.g;1.g1;2.g2;3.g3;4.�Զ���
	SetPropKey("des_para.LINEPARTNODE.wing_offset_dist",id++);
	SetPropKey("des_para.LINEPARTNODE.len_offset_dist",id++);	//֫��ƫ��ֵ;����ƫ��ֵ;
	SetPropKey("des_para.LINEPARTNODE.norm_offset",id++);		//����ƫ����;
	SetPropKey("des_para.LINEPARTNODE.face_norm_offset",id++);
	SetPropKey("des_para.LINEPARTNODE.face_norm_offset[0]",id++);
	SetPropKey("des_para.LINEPARTNODE.face_norm_offset[1]",id++);
	SetPropKey("des_para.LINEPARTNODE.face_norm_offset[2]",id++);
	//��׼�����������겻���
	SetPropKey("des_para.DATUMLINE.hDatumLine",id++);
	SetPropKey("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",id++);
	//��׼�����������겻���
	SetPropKey("des_para.DATUMPLANE.hDatumPlane",id++);
	SetPropKey("des_para.DATUMPLANE.cMirTransType",id++);
	SetPropKey("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",id++);
	//���ڵ������ȷֵ�
	SetPropKey("des_para.SCALE_NODE.hLinePart",id++);			//��׼�˼����
	SetPropKey("des_para.SCALE_NODE.hStartNode",id++);		//ʼ�˽ڵ���
	SetPropKey("des_para.SCALE_NODE.hEndNode",id++);			//�ն˽ڵ���
	SetPropKey("des_para.SCALE_NODE.start_offset_dist",id++);	//ʼ��ƫ����
	SetPropKey("des_para.SCALE_NODE.end_offset_dist",id++);	//�ն�ƫ����
	SetPropKey("des_para.SCALE_NODE.offset_scale",id++);		//ƫ�Ʊ���
	SetPropKey("des_para.SCALE_NODE.offset_dist",id++);		//�ڵȷֻ����ϵ�ƫ��ֵ
	//���涨λ��
	SetPropKey("des_para.COLUMN_INTERS.column",id++);
	SetPropKey("des_para.COLUMN_INTERS.column_r",id++);		//����뾶
	SetPropKey("des_para.COLUMN_INTERS.hLineTube",id++);		//�ֹܾ��
	SetPropKey("des_para.COLUMN_INTERS.axis_vec",id++);		//��������������
	SetPropKey("des_para.COLUMN_INTERS.axis_vec.x",id++);		
	SetPropKey("des_para.COLUMN_INTERS.axis_vec.y",id++);
	SetPropKey("des_para.COLUMN_INTERS.axis_vec.z",id++);
	SetPropKey("des_para.COLUMN_INTERS.sect_plane",id++);
	SetPropKey("des_para.COLUMN_INTERS.sect_center_type",id++);	//����Բ�ĵ㶨λ��ʽ
	SetPropKey("des_para.COLUMN_INTERS.SectCenter.hNode",id++);	//����ƫ�ƻ�׼�ڵ�
	SetPropKey("des_para.COLUMN_INTERS.SectCenter.len_offset_type",id++);	//����ƫ�Ʒ���
	SetPropKey("des_para.COLUMN_INTERS.SectCenter.len_offset",id++);		//����ƫ����
	SetPropKey("des_para.COLUMN_INTERS.sect_center",id++);	//����Բ�ĵ�
	SetPropKey("des_para.COLUMN_INTERS.sect_center.x",id++);	
	SetPropKey("des_para.COLUMN_INTERS.sect_center.y",id++);
	SetPropKey("des_para.COLUMN_INTERS.sect_center.z",id++);
	SetPropKey("des_para.COLUMN_INTERS.sect_norm",id++);		//���淨��NULL��ʾ����Ĵ�ֱ����
	SetPropKey("des_para.COLUMN_INTERS.sect_norm_std_style",id++);//���߷�������
	SetPropKey("des_para.COLUMN_INTERS.sect_norm.x",id++);	
	SetPropKey("des_para.COLUMN_INTERS.sect_norm.y",id++);
	SetPropKey("des_para.COLUMN_INTERS.sect_norm.z",id++);
	SetPropKey("des_para.COLUMN_INTERS.ray",id++);
	SetPropKey("des_para.COLUMN_INTERS.ray_vec_style",id++);	//���߷����巽ʽ
	SetPropKey("des_para.COLUMN_INTERS.ray_vec",id++);		//���߷���	
	SetPropKey("des_para.COLUMN_INTERS.ray_vec_std_style",id++);	//���߷�������
	SetPropKey("des_para.COLUMN_INTERS.ray_vec.x",id++);		
	SetPropKey("des_para.COLUMN_INTERS.ray_vec.y",id++);
	SetPropKey("des_para.COLUMN_INTERS.ray_vec.z",id++);
	SetPropKey("des_para.COLUMN_INTERS.ray_angle",id++);		//ƫ�ƽǶ�
	SetPropKey("des_para.COLUMN_INTERS.ray_para_offset",id++);//����ƽ���ƶ�����
	SetPropKey("des_para.COLUMN_INTERS.bSpecRo",id++);		//�����뾶���巽ʽ
	SetPropKey("des_para.COLUMN_INTERS.ray_offset_r",id++);	//�����뾶
	return id;
}
long CParaPointPropertyList::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("hDatumPt")==id)
	{
		if(m_xPoint.handle>0X20)
			sText.Printf("0X%X",m_xPoint.handle);
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("<New...>");
#else 
			sText.Copy("<�½�...>");
#endif
	}
	else if(GetPropID("datum_pos_desc")==id)
		sText.Printf("%s",m_xPoint.description);
	else if(GetPropID("datum_pos_style")==id)
	{	
		if(m_xPoint.datum_pos_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Manual Input");
		else if(m_xPoint.datum_pos_style==1)
			sText.Copy("1.Angle Ridge Vertex");
		else if(m_xPoint.datum_pos_style==2)
			sText.Copy("2.Rod Node Position");
		else if(m_xPoint.datum_pos_style==3)
			sText.Copy("3.Bolt line Intersection Point");
		else if(m_xPoint.datum_pos_style==4)
			sText.Copy("4.Datum Line Intersection");
		else if(m_xPoint.datum_pos_style==5)
			sText.Copy("5.Coord. Specified On Datum Line");
		else if(m_xPoint.datum_pos_style==6)
			sText.Copy("6.Coord. Specified  On Datum Plane");
		else if(m_xPoint.datum_pos_style==7)
			sText.Copy("7.angle ridge offset");
		else if(m_xPoint.datum_pos_style==8)
			sText.Copy("8.Same Position With Node");
		else if(m_xPoint.datum_pos_style==9)
			sText.Copy("9.Relative Coordinate On Datum Part");
#else 
			sText.Copy("0.�ֶ�����");
		else if(m_xPoint.datum_pos_style==1)
			sText.Copy("1.�Ǹ���㶨λ��");
		else if(m_xPoint.datum_pos_style==2)
			sText.Copy("2.�˼��ڵ㶨λ��");
		else if(m_xPoint.datum_pos_style==3)
			sText.Copy("3.�Ǹ����߽���");
		else if(m_xPoint.datum_pos_style==4)
			sText.Copy("4.��׼�߿ռ佻��");
		else if(m_xPoint.datum_pos_style==5)
			sText.Copy("5.��׼����ָ�����������");
		else if(m_xPoint.datum_pos_style==6)
			sText.Copy("6.��׼����ָ�������");
		else if(m_xPoint.datum_pos_style==7)
			sText.Copy("7.��֫����ƫ�Ƶİ������");
		else if(m_xPoint.datum_pos_style==8)
			sText.Copy("8.��ڵ�ͬλ");
		else if(m_xPoint.datum_pos_style==9)
			sText.Copy("9.��׼�����ϵ��������");
#endif
	}
	else if(GetPropID("pos.x")==id)
	{
		sprintf(sText,"%f",m_xPoint.Position().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pos.y")==id)
	{
		sprintf(sText,"%f",m_xPoint.Position().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pos.z")==id)
	{
		sprintf(sText,"%f",m_xPoint.Position().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.hRod")==id)	//�Ǹ־��
		sprintf(sText,"0X%X",m_xPoint.des_para.RODEND.hRod);
	else if(GetPropID("des_para.RODEND.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		if(m_xPoint.des_para.RODEND.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
			strcpy(sText,"0.ʼ->��");
		else 
			strcpy(sText,"1.��->ʼ");
#endif
	}
	else if(GetPropID("des_para.RODEND.offset_wing")==id)//0.X֫;1.Y֫
	{	//0.X֫;1.Y֫
		if(m_xPoint.des_para.RODEND.offset_wing==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.X Leg");
		else 
			strcpy(sText,"1.Y Leg");
#else 
			strcpy(sText,"0.X֫");
		else 
			strcpy(sText,"1.Y֫");
#endif
	}
	else if(GetPropID("des_para.RODEND.wing_offset_style")==id)	
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(m_xPoint.des_para.RODEND.wing_offset_style==0)
			strcpy(sText,"g");
		else if(m_xPoint.des_para.RODEND.wing_offset_style==1)
			strcpy(sText,"g1");
		else if(m_xPoint.des_para.RODEND.wing_offset_style==2)
			strcpy(sText,"g2");
		else if(m_xPoint.des_para.RODEND.wing_offset_style==3)
			strcpy(sText,"g3");
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else 
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("des_para.RODEND.wing_offset_dist")==id)
	{
		CLDSPart* pAngle=Ta.Parts.FromHandle(m_xPoint.des_para.RODEND.hRod);
		if(pAngle&&pAngle->GetClassTypeId()==CLS_LINEANGLE&&m_xPoint.des_para.RODEND.wing_offset_style<4)
		{
			char cWorkWing='X';
			if(m_xPoint.des_para.RODEND.offset_wing==1)
				cWorkWing='Y';
			JGZJ jgzj=((CLDSLineAngle*)pAngle)->GetZhunJu(cWorkWing);
			if(m_xPoint.des_para.RODEND.wing_offset_style==0)
				m_xPoint.des_para.RODEND.wing_offset_dist=jgzj.g;
			else if(m_xPoint.des_para.RODEND.wing_offset_style==1)
				m_xPoint.des_para.RODEND.wing_offset_dist=jgzj.g1;
			else if(m_xPoint.des_para.RODEND.wing_offset_style==2)
				m_xPoint.des_para.RODEND.wing_offset_dist=jgzj.g2;
			else if(m_xPoint.des_para.RODEND.wing_offset_style==3)
				m_xPoint.des_para.RODEND.wing_offset_dist=jgzj.g3;
		}
		else if(m_xPoint.des_para.RODEND.wing_offset_style!=4)
			m_xPoint.des_para.RODEND.wing_offset_dist=0;
		sprintf(sText,"%f",m_xPoint.des_para.RODEND.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
	{	//����ƫ��ֵ;
		sprintf(sText,"%f",m_xPoint.des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.bIncOddEffect")==id)		
	{	//����ƫ��ֵ��������ͷӰ��
		if(m_xPoint.des_para.RODEND.bIncOddEffect)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else 
			strcpy(sText,"No");
#else 
			strcpy(sText,"��");
		else 
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("des_para.RODEND.norm_offset")==id)//����ƫ����
	{
		CDesignThickPara thick(m_xPoint.des_para.RODEND.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.LINEPARTNODE.hLinePart")==id)	//�Ǹ־��
		sprintf(sText,"0X%X",m_xPoint.des_para.RODNODE.hRod);
	else if(GetPropID("des_para.LINEPARTNODE.hNode")==id)	//��׼�ڵ���
		sprintf(sText,"0X%X",m_xPoint.des_para.RODNODE.hNode);
	else if(GetPropID("des_para.LINEPARTNODE.bFaceOffset")==id)
	{
		if(m_xPoint.des_para.RODNODE.bFaceOffset)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Yes");
		else 
			strcpy(sText,"No");
#else 
			strcpy(sText,"��");
		else 
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		if(m_xPoint.des_para.RODNODE.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
			strcpy(sText,"0.ʼ->��");
		else 
			strcpy(sText,"1.��->ʼ");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.offset_wing")==id)	
	{	//0.X֫;1.Y֫
		if(m_xPoint.des_para.RODNODE.offset_wing==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.X Leg");
		else 
			strcpy(sText,"1.Y Leg");
#else 
			strcpy(sText,"0.X֫");
		else 
			strcpy(sText,"1.Y֫");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.wing_offset_style")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(m_xPoint.des_para.RODNODE.wing_offset_style==0)
			strcpy(sText,"g");
		else if(m_xPoint.des_para.RODNODE.wing_offset_style==1)
			strcpy(sText,"g1");
		else if(m_xPoint.des_para.RODNODE.wing_offset_style==2)
			strcpy(sText,"g2");
		else if(m_xPoint.des_para.RODNODE.wing_offset_style==3)
			strcpy(sText,"g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else 
			strcpy(sText,"�Զ���");
#endif
	}	
	else if(GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==id)
	{
		CLDSPart* pAngle=Ta.Parts.FromHandle(m_xPoint.des_para.RODNODE.hRod);
		if(pAngle&&pAngle->GetClassTypeId()==CLS_LINEANGLE&&m_xPoint.des_para.RODNODE.wing_offset_style<4)
		{
			char cWorkWing='X';
			if(m_xPoint.des_para.RODNODE.offset_wing==1)
				cWorkWing='Y';
			JGZJ jgzj=((CLDSLineAngle*)pAngle)->GetZhunJu(cWorkWing);
			if(m_xPoint.datum_pos_style==2)
			{	//֮ǰδ����ڵ�����׼�����,���׵��½Ǹֶ�ͷ����λ������˨��һ��,��������� wjh-2017.11.23
				CLDSLineAngle* pBasicAngle=(CLDSLineAngle*)pAngle;
				CLDSNode* pDatumNode=pBasicAngle->BelongModel()->FromNodeHandle(m_xPoint.m_hNode);
				jgzj.g=ftoi(cWorkWing=='X'?pBasicAngle->GetNodeWingXG(pDatumNode):pBasicAngle->GetNodeWingYG(pDatumNode));
			}
			if(m_xPoint.des_para.RODNODE.wing_offset_style==0)
				m_xPoint.des_para.RODNODE.wing_offset_dist=jgzj.g;
			else if(m_xPoint.des_para.RODNODE.wing_offset_style==1)
				m_xPoint.des_para.RODNODE.wing_offset_dist=jgzj.g1;
			else if(m_xPoint.des_para.RODNODE.wing_offset_style==2)
				m_xPoint.des_para.RODNODE.wing_offset_dist=jgzj.g2;
			else if(m_xPoint.des_para.RODNODE.wing_offset_style==3)
				m_xPoint.des_para.RODNODE.wing_offset_dist=jgzj.g3;
		}
		else if(m_xPoint.des_para.RODNODE.wing_offset_style!=4)
			m_xPoint.des_para.RODNODE.wing_offset_dist=0;
		sprintf(sText,"%f",m_xPoint.des_para.RODNODE.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.len_offset_dist")==id)	
	{	//����ƫ��ֵ
		sprintf(sText,"%f",m_xPoint.des_para.RODNODE.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.norm_offset")==id)	//����ƫ����
	{
		CDesignThickPara thick(m_xPoint.des_para.RODNODE.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset")==id)
		sprintf(sText,"%s",CStdVector::GetVecTypeString(m_xPoint.des_para.RODNODE.vPlaneNormal));
	else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.RODNODE.vPlaneNormal.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.RODNODE.vPlaneNormal.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.RODNODE.vPlaneNormal.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.hDatum1")==id)
		sprintf(sText,"0X%X",m_xPoint.des_para.AXIS_INTERS.hDatum1);
	else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==id)
		sprintf(sText,"0X%X",m_xPoint.des_para.AXIS_INTERS.hDatum2);
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==0)
			strcpy(sText,"g");
		else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==1)
			strcpy(sText,"g1");
		else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==2)
			strcpy(sText,"g2");
		else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==3)
			strcpy(sText,"g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else 
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==0)
			strcpy(sText,"g");
		else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==1)
			strcpy(sText,"g1");
		else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==2)
			strcpy(sText,"g2");
		else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==3)
			strcpy(sText,"g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else 
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==id)
	{
		if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1<4)
		{
			CLDSLineAngle* pAngle1=(CLDSLineAngle*)Ta.Parts.FromHandle(m_xPoint.des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			CLDSLineAngle* pAngle2=(CLDSLineAngle*)Ta.Parts.FromHandle(m_xPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			char cWorkWing='X';
			if(pAngle1!=NULL && pAngle2!=NULL)
			{
				f3dPoint vec1 = pAngle1->End()-pAngle1->Start();
				f3dPoint vec2 = pAngle2->End()-pAngle2->Start();
				f3dPoint norm = vec1^vec2;
				normalize(norm);
				if(fabs(norm*pAngle1->get_norm_x_wing())<fabs(norm*pAngle1->get_norm_y_wing()))
					cWorkWing='Y';
			}
			if(pAngle1)
			{
				JGZJ jgzj=((CLDSLineAngle*)pAngle1)->GetZhunJu(cWorkWing);
				if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==0)
					m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g;
				else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==1)
					m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g1;
				else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==2)
					m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g2;
				else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1==3)
					m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g3;
			}
			else
				m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1=0;
		}
		else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1!=4)
			m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1=0;
		sprintf(sText,"%d",m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==id)
	{
		if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2<4)
		{
			CLDSLineAngle* pAngle1=(CLDSLineAngle*)Ta.Parts.FromHandle(m_xPoint.des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			CLDSLineAngle* pAngle2=(CLDSLineAngle*)Ta.Parts.FromHandle(m_xPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			char cWorkWing='X';
			if(pAngle1!=NULL && pAngle2!=NULL)
			{
				f3dPoint vec1 = pAngle1->End()-pAngle1->Start();
				f3dPoint vec2 = pAngle2->End()-pAngle2->Start();
				f3dPoint norm = vec1^vec2;
				normalize(norm);
				if(fabs(norm*pAngle2->get_norm_x_wing())<fabs(norm*pAngle2->get_norm_y_wing()))
					cWorkWing='Y';
			}
			if(pAngle2)
			{
				JGZJ jgzj=((CLDSLineAngle*)pAngle2)->GetZhunJu(cWorkWing);
				if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==0)
					m_xPoint.des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g;
				else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==1)
					m_xPoint.des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g1;
				else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==2)
					m_xPoint.des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g2;
				else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2==3)
					m_xPoint.des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g3;
			}
			else
				m_xPoint.des_para.AXIS_INTERS.wing_offset_dist2=0;
		}
		else if(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1!=4)
			m_xPoint.des_para.AXIS_INTERS.wing_offset_dist1=0;
		sprintf(sText,"%d",m_xPoint.des_para.AXIS_INTERS.wing_offset_dist2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.near_vector")==id)
		sprintf(sText,"%s",CStdVector::GetVecTypeString(f3dPoint(m_xPoint.des_para.AXIS_INTERS.near_vector)));
	else if(GetPropID("des_para.AXIS_INTERS.near_vector[0]")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.AXIS_INTERS.near_vector[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.near_vector[1]")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.AXIS_INTERS.near_vector[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.near_vector[2]")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.AXIS_INTERS.near_vector[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.norm_offset")==id)	//����ƫ����;
	{
		CDesignThickPara thick(m_xPoint.des_para.AXIS_INTERS.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.AXIS_INTERS.hDatum1")==id)
		sprintf(sText,"0X%X",m_xPoint.des_para.AXIS_INTERS.hDatum1);
	else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==id)
		sprintf(sText,"0X%X",m_xPoint.des_para.AXIS_INTERS.hDatum2);
	else if(GetPropID("des_para.DATUMLINE.hDatumLine")==id)
		sprintf(sText,"0X%X",m_xPoint.des_para.DATUMLINE.hDatumLine);
	else if(GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==id)
	{
		if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"0.X Axis");
		else if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sprintf(sText,"1.Y Axis");
		else 
			sprintf(sText,"2.Z Axis");
#else 
			sprintf(sText,"0.X��");
		else if(m_xPoint.des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sprintf(sText,"1.Y��");
		else 
			sprintf(sText,"2.Z��");
#endif
	}
	else if(GetPropID("des_para.DATUMPLANE.hDatumPlane")==id)
		sprintf(sText,"0X%X",m_xPoint.des_para.DATUMPLANE.hDatumPlane);
	else if(GetPropID("des_para.DATUMPLANE.cMirTransType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='X')
			strcpy(sText,"X Mirror");
		else if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='Y')
			strcpy(sText,"Y Mirror");
		else if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='Z')
			strcpy(sText,"Z Mirror");
		else
			strcpy(sText,"None");
#else 
		if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='X')
			strcpy(sText,"X��Գ�");
		else if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='Y')
			strcpy(sText,"Y��Գ�");
		else if(m_xPoint.des_para.DATUMPLANE.cMirTransType=='Z')
			strcpy(sText,"Z��Գ�");
		else
			strcpy(sText,"�޶ԳƲ���");
#endif
	}
	else if(GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==id)
	{
		if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Printf("0.X-Z");
		else if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			sText.Printf("1.Y-Z");
		else 
			sText.Printf("2.X-Y");
#else 
			sText.Printf("0.X-Z����");
		else if(m_xPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			sText.Printf("1.Y-Z����");
		else 
			sText.Printf("2.X-Y����");
#endif
	}
	else if(GetPropID("des_para.RODEND.hRod")==id)	//�Ǹ־��
		sprintf(sText,"0X%X",m_xPoint.des_para.RODEND.hRod);
	else if(GetPropID("des_para.RODEND.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
		if(m_xPoint.des_para.RODEND.direction==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
			strcpy(sText,"0.ʼ->��");
		else 
			strcpy(sText,"1.��->ʼ");
#endif
	}
	else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
	{	//����ƫ��ֵ;
		sprintf(sText,"%f",m_xPoint.des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.norm_offset")==id)//����ƫ����
	{
		CDesignThickPara thick(m_xPoint.des_para.RODEND.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.hNode")==id)
		sprintf(sText,"0X%X",m_xPoint.des_para.hNode);
	else if(GetPropID("des_para.hPart")==id)
		sprintf(sText,"0X%X",m_xPoint.des_para.hPart);
	else if(GetPropID("datum_pos.x")==id)
	{
		sprintf(sText,"%f",m_xPoint.datum_pos.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("datum_pos.y")==id)
	{
		sprintf(sText,"%f",m_xPoint.datum_pos.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("datum_pos.z")==id)
	{
		sprintf(sText,"%f",m_xPoint.datum_pos.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.hLinePart")==id)			//��׼�˼����
		sprintf(sText,"0X%0X",m_xPoint.des_para.SCALE_NODE.hLinePart);
	else if(GetPropID("des_para.SCALE_NODE.hStartNode")==id)		//ʼ�˽ڵ���
		sprintf(sText,"0X%0X",m_xPoint.des_para.SCALE_NODE.hStartNode);
	else if(GetPropID("des_para.SCALE_NODE.hEndNode")==id)			//�ն˽ڵ���
		sprintf(sText,"0X%0X",m_xPoint.des_para.SCALE_NODE.hEndNode);
	else if(GetPropID("des_para.SCALE_NODE.start_offset_dist")==id)	//ʼ��ƫ����
	{
		sprintf(sText,"%f",m_xPoint.des_para.SCALE_NODE.start_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.end_offset_dist")==id)	//�ն�ƫ����
	{
		sprintf(sText,"%f",m_xPoint.des_para.SCALE_NODE.end_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.offset_scale")==id)		//ƫ�Ʊ���
	{
		sprintf(sText,"%f",m_xPoint.des_para.SCALE_NODE.offset_scale);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.offset_dist")==id)		//�ڵȷֻ����ϵ�ƫ��ֵ
	{
		sprintf(sText,"%f",m_xPoint.des_para.SCALE_NODE.offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.hLineTube")==id)		//�ֹܾ��
		sprintf(sText,"0X%X",m_xPoint.des_para.COLUMN_INTERS.hLineTube);
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec")==id)
	{
		f3dPoint axis_vec(m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[0],m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[1],m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
		sText.ConvertFromPoint(axis_vec);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.x")==id)		//��������������
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.y")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.z")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center_type")==id)
	{
		if(m_xPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag&0x02)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("1.Parametric");
		else
			sText.Copy("0.User Input");
#else
			sText.Copy("1.��������λ");
		else
			sText.Copy("0.�û�ָ��");
#endif
	}
	else if(GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode")==id)
		sText.Printf("0X%X",abs(m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode));
	else if(GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset_type")==id)
	{
		if(m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode>=0)
#ifdef AFX_TARG_ENU_ENGLISH
			sText.Copy("0.Start->End");
		else
			sText.Copy("1.End->Start");
#else
			sText.Copy("0.ʼ->��");
		else
			sText.Copy("1.��->ʼ");
#endif
	}
	else if(GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset")==id)
		sText.Printf("%g",m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset);
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center")==id)
	{
		f3dPoint center(m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[0],m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[1],m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
		sText.ConvertFromPoint(center);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center.x")==id)	//����Բ�ĵ�
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center.y")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center.z")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm")==id)
	{
		f3dPoint sect_norm(m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[0],m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[1],m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
		sText.ConvertFromPoint(sect_norm);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm_std_style")==id)
	{
		f3dPoint norm(m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[0],m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[1],m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(norm));
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.x")==id)	//���淨��NULL��ʾ����Ĵ�ֱ����
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.y")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.z")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec_style")==id)	//���߷����巽ʽ
	{
		if(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Specify Directly");
		else if(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.Specify Datum Edge");
		else if(m_xPoint.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.Tube's Datum Edge As Datum");
		else if(m_xPoint.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==3)
			strcpy(sText,"3.Tube's Weld Line As Datum");
		else if(m_xPoint.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==4)
			strcpy(sText,"4.Horizontal ray vector");
#else 
			strcpy(sText,"0.ֱ��ָ��");
		else if(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.ָ����׼��");
		else if(m_xPoint.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.�Ի�׼����ת");
		else if(m_xPoint.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==3)
			strcpy(sText,"3.�Ժ�������ת");
		else if(m_xPoint.des_para.COLUMN_INTERS.hLineTube>0x20 && m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==4)
			strcpy(sText,"4.ˮƽ����");
#endif
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style")==id)
	{
		f3dPoint rec_vector(m_xPoint.des_para.COLUMN_INTERS.RayVector.ray_vec[0],m_xPoint.des_para.COLUMN_INTERS.RayVector.ray_vec[1],m_xPoint.des_para.COLUMN_INTERS.RayVector.ray_vec[2]);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(rec_vector));
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.x")==id)		//���߷���
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.RayVector.ray_vec[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.y")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.RayVector.ray_vec[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.z")==id)
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.RayVector.ray_vec[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_angle")==id)		//ƫ�ƽǶ�
		sprintf(sText,"%g",m_xPoint.des_para.COLUMN_INTERS.ray_angle);
	else if(GetPropID("des_para.COLUMN_INTERS.column_r")==id)		//����뾶
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.column_r);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_para_offset")==id)	//����ƽ���ƶ�����
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.ray_para_offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.bSpecRo")==id)			//�����뾶���巽ʽ
	{
		if(!m_xPoint.des_para.COLUMN_INTERS.bSpecRo)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Ray-Cylinder intersection");
		else
			strcpy(sText,"1.Section center");
#else 
			strcpy(sText,"0.�����潻��");
		else
			strcpy(sText,"1.����Բ��");
#endif
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_offset_r")==id)	//�����뾶
	{
		sprintf(sText,"%f",m_xPoint.des_para.COLUMN_INTERS.ray_offset_r);
		SimplifiedNumString(sText);
	}
	else
		return 0;//false;
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
void CParaPointPropertyList::CopyFrom(CLDSPoint* pParaPoint)
{
	CBuffer buffer;
	m_xPoint.handle=pParaPoint->handle;
	pParaPoint->ToBuffer(buffer);
	buffer.SeekToBegin();
	m_xPoint.FromBuffer(buffer);
}
void CParaPointPropertyList::CopyTo(CLDSPoint* pParaPoint)
{
	CBuffer buffer;
	pParaPoint->handle=m_xPoint.handle;
	m_xPoint.ToBuffer(buffer);
	buffer.SeekToBegin();
	pParaPoint->FromBuffer(buffer);
}
bool CParaPointPropertyList::CallBackCommand(UINT message,WPARAM wParam,BOOL *lpRslt)
{
	return true;
}
bool CParaPointPropertyList::ButtonClick(CPropTreeItem* pItem,BOOL *lpRslt)
{
	CReportVectorDlg vecDlg;
	CPropTreeItem *pParentItem=NULL;
	CXhChar100 sText;
	CCallBackDialog* pDialog=(CCallBackDialog*)GetParent();
	if(m_xPoint.datum_pos_style==1)
	{	//�Ǹ���㶨λ
		if(GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)
			pDialog->SelectObject(CLS_LINEPART,pItem->m_idProp);//��׼�˼�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=m_xPoint.des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input normal offset string",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(m_xPoint.des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(GetPropValueStr(pItem->m_idProp,sText))
				SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(m_xPoint.datum_pos_style==2)
	{	//�˼��Ͻڵ㶨λ
		if(GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)
			pDialog->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�
		else if(GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)
			pDialog->SelectObject(CLS_NODE,pItem->m_idProp);		//��׼�ڵ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("des_para.LINEPARTNODE.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=m_xPoint.des_para.RODNODE.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input normal offset string",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(m_xPoint.des_para.RODNODE.norm_offset,49,"%s",sThickStr);
			if(GetPropValueStr(pItem->m_idProp,sText))
				SetItemPropValue(pItem->m_idProp,CString(sText));
		}
		else if(pItem->m_idProp==GetPropID("des_para.LINEPARTNODE.face_norm_offset")||
			pItem->m_idProp==GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")||
			pItem->m_idProp==GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")||
			pItem->m_idProp==GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"))

		{	
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Face Offset Plane Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = m_xPoint.des_para.RODNODE.vPlaneNormal.x;
			vecDlg.m_fVectorY = m_xPoint.des_para.RODNODE.vPlaneNormal.y;
			vecDlg.m_fVectorZ = m_xPoint.des_para.RODNODE.vPlaneNormal.z;
			pItem = FindItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"),&pParentItem);
			vecDlg.m_bReadOnlyX = pItem->IsReadOnly();
			pItem = FindItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"),&pParentItem);
			vecDlg.m_bReadOnlyY = pItem->IsReadOnly();
			pItem = FindItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"),CString(sText));
			}
		}
#endif
	}
	else if(m_xPoint.datum_pos_style==3)
	{	//�Ǹ����߽���
		if(GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			pDialog->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�1
		else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			pDialog->SelectObject(CLS_LINEPART,pItem->m_idProp);	//��׼�˼�2
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("des_para.AXIS_INTERS.norm_offset")==pItem->m_idProp)
		{	//����ƫ����
			CString sThickStr;
			sThickStr=m_xPoint.des_para.AXIS_INTERS.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input normal offset string",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(m_xPoint.des_para.AXIS_INTERS.norm_offset,49,"%s",sThickStr);
			if(GetPropValueStr(pItem->m_idProp,sText))
				SetItemPropValue(pItem->m_idProp,CString(sText));
		}
		else if(GetPropID("des_para.AXIS_INTERS.near_vector")==pItem->m_idProp||
			GetPropID("des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp||
			GetPropID("des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp||
			GetPropID("des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Face Offset Plane Normal";
#else 
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = m_xPoint.des_para.AXIS_INTERS.near_vector[0];
			vecDlg.m_fVectorY = m_xPoint.des_para.AXIS_INTERS.near_vector[1];
			vecDlg.m_fVectorZ = m_xPoint.des_para.AXIS_INTERS.near_vector[2];
			pItem = FindItemByPropId(GetPropID("des_para.AXIS_INTERS.near_vector[0]"),&pParentItem);
			vecDlg.m_bReadOnlyX = pItem->IsReadOnly();
			pItem = FindItemByPropId(GetPropID("des_para.AXIS_INTERS.near_vector[1]"),&pParentItem);
			vecDlg.m_bReadOnlyY = pItem->IsReadOnly();
			pItem = FindItemByPropId(GetPropID("des_para.AXIS_INTERS.near_vector[2]"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.AXIS_INTERS.near_vector[0]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.AXIS_INTERS.near_vector[1]"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.AXIS_INTERS.near_vector[2]"),CString(sText));
			}
		}
#endif
	}
	else if(m_xPoint.datum_pos_style==7)
	{	//�ؽǸ���֫����ƫ�Ƶİ������
		if(GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)
			pDialog->SelectObject(CLS_LINEANGLE,pItem->m_idProp);//��׼�Ǹ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("des_para.RODEND.norm_offset")==pItem->m_idProp)
		{	//����ƫ����

			CString sThickStr;
			sThickStr=m_xPoint.des_para.RODEND.norm_offset;
#ifdef AFX_TARG_ENU_ENGLISH
			Input("Input normal offset string",&sThickStr,'s');
#else 
			Input("���뷨��ƫ�����ַ���",&sThickStr,'s');
#endif
			_snprintf(m_xPoint.des_para.RODEND.norm_offset,49,"%s",sThickStr);
			if(GetPropValueStr(pItem->m_idProp,sText))
				SetItemPropValue(pItem->m_idProp,CString(sText));
		}
#endif
	}
	else if(m_xPoint.datum_pos_style==8)
	{	//��ڵ�ͬλ
		if(GetPropID("des_para.hNode")==pItem->m_idProp)
			pDialog->SelectObject(CLS_NODE,pItem->m_idProp);	//��׼�ڵ�
	}
	else if(m_xPoint.datum_pos_style==9)
	{	//ָ�������ϵ����λ��
		if(GetPropID("des_para.hPart")==pItem->m_idProp)
			pDialog->SelectObject(CLS_PART,pItem->m_idProp);	//��׼����
		else if(GetPropID("datum_pos")==pItem->m_idProp)
		{
			CReportPointDlg pointDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Relative Coord.";
#else 
			pointDlg.m_sCaption = "�������";
#endif
			pointDlg.m_fPointX = m_xPoint.datum_pos.x;
			pointDlg.m_fPointY = m_xPoint.datum_pos.y;
			pointDlg.m_fPointZ = m_xPoint.datum_pos.z;
			if(pointDlg.DoModal()==IDOK)
			{
				CXhChar100 sText("%f",pointDlg.m_fPointX);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("datum_pos.x"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("datum_pos.y"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("datum_pos.z"),CString(sText));
				m_xPoint.datum_pos.Set(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ);
			}
		}
	}
	else if(m_xPoint.datum_pos_style==10)
	{
		if(GetPropID("des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)
			pDialog->SelectObject(CLS_LINEPART,pItem->m_idProp);
		else if(GetPropID("des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)
			pDialog->SelectObject(CLS_NODE,pItem->m_idProp);
		else if(GetPropID("des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)
			pDialog->SelectObject(CLS_NODE,pItem->m_idProp);

	}
	else if(m_xPoint.datum_pos_style==11)
	{
		if(GetPropID("des_para.COLUMN_INTERS.hLineTube")==pItem->m_idProp)
			pDialog->SelectObject(CLS_LINEPART,pItem->m_idProp);//��׼�ֹ�
		else if(GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode")==pItem->m_idProp)
			pDialog->SelectObject(CLS_NODE,pItem->m_idProp);//��׼�ֹ�
		else if(GetPropID("des_para.COLUMN_INTERS.axis_vec")==pItem->m_idProp)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Axis Vector";
#else 
			vecDlg.m_sCaption = "���߷���";
#endif
			vecDlg.m_fVectorX = m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[0];
			vecDlg.m_fVectorY = m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[1];
			vecDlg.m_fVectorZ = m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[2];
			CPropTreeItem *pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.axis_vec.x"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.axis_vec.y"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.axis_vec.z"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.axis_vec.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.axis_vec.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.axis_vec.z"),CString(sText));
				m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[0] = vecDlg.m_fVectorX;
				m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[1] = vecDlg.m_fVectorY;
				m_xPoint.des_para.COLUMN_INTERS.AxisVector.axis_vec[2] = vecDlg.m_fVectorZ;
			}
		}
		else if(GetPropID("des_para.COLUMN_INTERS.sect_center")==pItem->m_idProp)
		{
			CReportVectorDlg vecDlg;
			CLDSLineTube *pLineTube = (CLDSLineTube *)console.FromPartHandle(m_xPoint.des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
			if(pLineTube)
#ifdef AFX_TARG_ENU_ENGLISH
				vecDlg.m_sCaption = "Section Pick Point";
			else 
				vecDlg.m_sCaption = "Section Center";
#else 
				vecDlg.m_sCaption = "����ʰȡ��";
			else 
				vecDlg.m_sCaption = "����Բ��";
#endif
			vecDlg.m_fVectorX = m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[0];
			vecDlg.m_fVectorY = m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[1];
			vecDlg.m_fVectorZ = m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[2];
			CPropTreeItem *pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.x"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.y"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.z"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.sect_center.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.sect_center.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.sect_center.z"),CString(sText));
				m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[0] = vecDlg.m_fVectorX;
				m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[1] = vecDlg.m_fVectorY;
				m_xPoint.des_para.COLUMN_INTERS.SectCenter.sect_center[2] = vecDlg.m_fVectorZ;
			}
		}
		else if(GetPropID("des_para.COLUMN_INTERS.sect_norm")==pItem->m_idProp)
		{
			CReportVectorDlg vecDlg;
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Section Normal";
#else 
			vecDlg.m_sCaption = "���淨��";
#endif
			vecDlg.m_fVectorX = m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[0];
			vecDlg.m_fVectorY = m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[1];
			vecDlg.m_fVectorZ = m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[2];
			CPropTreeItem *pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_norm.x"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_norm.y"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_norm.z"),NULL);
			if(pFindItem)
				vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				char sText[MAX_TEMP_CHAR_100+1];
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.sect_norm.x"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.sect_norm.y"),CString(sText));
				_snprintf(sText,MAX_TEMP_CHAR_100,"%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				SetItemPropValue(GetPropID("des_para.COLUMN_INTERS.sect_norm.z"),CString(sText));
				m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[0] = vecDlg.m_fVectorX;
				m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[1] = vecDlg.m_fVectorY;
				m_xPoint.des_para.COLUMN_INTERS.SectNorm.sect_norm[2] = vecDlg.m_fVectorZ;
			}
		}
	}
	if (ReflectButtonClickFunc)
		ReflectButtonClickFunc(this,pItem);
	if(lpRslt)
		*lpRslt=TRUE;
	return true;
}
bool CParaPointPropertyList::UpdateColumnIntersSectCenterParamProperty(CPropTreeItem* pParentItem)
{
	CXhChar100 sText;
	CItemInfo *lpInfo;
	CPropTreeItem* pFindItem=NULL;
	if((m_xPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag&0x02)==0 || m_xPoint.des_para.COLUMN_INTERS.hLineTube<0x20)
	{
		DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode"));
		DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset_type"));
		DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset"));
		//����Բ�ĵ�
		CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(m_xPoint.des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		CPropTreeItem *pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center"),NULL);
		if(pFindItem==NULL)
		{	//ָ������Բ�ĵ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Section Center";
			lpInfo->m_strPropHelp = "Section center";
			if(pLineTube)
			{
				lpInfo->m_strPropName = "Section Pick Point";
				lpInfo->m_strPropHelp = "Section pick point used to calculate section center";
			}
#else 
			lpInfo->m_strPropName = "����Բ��";
			lpInfo->m_strPropHelp = "����Բ��";
			if(pLineTube)
			{
				lpInfo->m_strPropName = "����ʰȡ��";
				lpInfo->m_strPropHelp = "����ʰȡ�����������Բ�ĵ�";
			}
#endif
			pFindItem= InsertItem(pParentItem ,lpInfo,-1,TRUE);
			pFindItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center");
			pFindItem->m_bHideChildren = TRUE;
		}
		if(GetPropValueStr(pFindItem->m_idProp,sText))
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		CPropTreeItem* pSonPropItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.x"),NULL);
		if(pSonPropItem==NULL)
		{	//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="X coordinate of ";
#else 
			lpInfo->m_strPropHelp="X����";
#endif
			pSonPropItem = InsertItem(pFindItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center.x");
		}
		if(GetPropValueStr(pSonPropItem->m_idProp,sText))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.y"),NULL);
		if(pSonPropItem==NULL)
		{	//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Y coordinate";
#else 
			lpInfo->m_strPropHelp="Y����";
#endif
			pSonPropItem = InsertItem(pFindItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center.y");
		}
		if(GetPropValueStr(pSonPropItem->m_idProp,sText))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.z"),NULL);
		if(pSonPropItem==NULL)
		{	//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Z coordinate";
#else 
			lpInfo->m_strPropHelp="Z����";
#endif
			pSonPropItem = InsertItem(pFindItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center.z");
		}
		if(GetPropValueStr(pSonPropItem->m_idProp,sText))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
	}
	else
	{
		DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center"));
		DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.x"));
		DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.y"));
		DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center.z"));

		pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode"),NULL);
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Node";
			lpInfo->m_strPropHelp = "Section center's location offset datum node";
#else 
			lpInfo->m_strPropName = "ƫ�ƻ�׼�ڵ�";
			lpInfo->m_strPropHelp = "����Բ�ĵ��ƫ�ƶ�λ��ʼ�ڵ�";
#endif
			pFindItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
			pFindItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode");
		}
		if(GetPropValueStr(pFindItem->m_idProp,sText))
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset_type"),NULL);
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset Direction";
			lpInfo->m_strPropHelp = "Offset Direction from datum node along rod";
			lpInfo->m_cmbItems="0.Start->End|1.End->Start";
#else 
			lpInfo->m_strPropName = "ƫ�Ʒ���";
			lpInfo->m_strPropHelp = "��ƫ����ʼ�ڵ��ػ�׼�ֹܵ�ƫ�Ʒ���";
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ";
#endif
			pFindItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
			pFindItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset_type");
		}
		if(GetPropValueStr(pFindItem->m_idProp,sText))
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
		pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset"),NULL);
		if(pFindItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset Distance";
			lpInfo->m_strPropHelp = "Offset distance from datum node";
#else 
			lpInfo->m_strPropName = "ƫ����";
			lpInfo->m_strPropHelp = "����Բ�ĵ���ƫ����ʼ�ڵ��ƫ����";
#endif
			pFindItem = InsertItem(pParentItem,lpInfo, -1,TRUE);
			pFindItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset");
		}
		if(GetPropValueStr(pFindItem->m_idProp,sText))
			pFindItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	return true;
}
bool CParaPointPropertyList::ModifyValueFunc(CPropTreeItem* pItem,CString& valueStr,BOOL *lpRslt)
{
	BOOL bRefreshPtPara=FALSE;
	CXhChar50 value_str;
	CLDSPoint *pPoint=&m_xPoint;
	if(GetPropID("datum_pos_desc")==pItem->m_idProp )
		strncpy(pPoint->description,valueStr,50);
	else if(GetPropID("hDatumPt")==pItem->m_idProp)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("<New...>")==0)
#else 
		if(valueStr.Compare("<�½�...>")==0)
#endif
		{
			CLDSPoint* pPt=new CLDSPoint();
			*pPoint=*pPt;
		}
		else
		{
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->handle));
			CLDSPoint* pPt=Ta.Point.FromHandle(pPoint->handle);
			*pPoint=*pPt;
		}
		bRefreshPtPara=TRUE;
		CXhChar50 sText;
		SetItemPropValue(GetPropID("datum_pos_desc"),pPoint->description);
		GetPropValueStr(GetPropID("datum_pos_style"),sText);
		SetItemPropValue(GetPropID("datum_pos_style"),sText);
	}
	else if(GetPropID("datum_pos_style")==pItem->m_idProp)
	{
		int nPosStyle=valueStr[0] - '0';
		if(pPoint->datum_pos_style!=nPosStyle)
		{
			pPoint->datum_pos_style=nPosStyle;
			InitDatumPointDesPara(pPoint);
			bRefreshPtPara=TRUE;
		}
	}
	else if(pPoint->datum_pos_style==0)	//ֱ��ָ��
	{
		if(GetPropID("pos.x")==pItem->m_idProp)
		{
			double coord_x=atof(valueStr);
			pPoint->SetPositionX(coord_x);
			pPoint->datum_pos.x=coord_x;
		}
		else if(GetPropID("pos.y")==pItem->m_idProp)
		{
			double coord_y=atof(valueStr);
			pPoint->SetPositionY(atof(valueStr));
			pPoint->datum_pos.y=coord_y;
		}
		else if(GetPropID("pos.z")==pItem->m_idProp)
		{
			double coord_z=atof(valueStr);
			pPoint->SetPositionZ(atof(valueStr));
			pPoint->datum_pos.z=coord_z;
		}
	}
	else if(pPoint->datum_pos_style==1)	//�Ǹ���㶨λ
	{
		if(GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(GetPropID("des_para.RODEND.offset_wing")==pItem->m_idProp)//0.X֫;1.Y֫
			pPoint->des_para.RODEND.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("des_para.RODEND.wing_offset_style")==pItem->m_idProp)	
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.RODEND.wing_offset_style=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.RODEND.wing_offset_style=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.RODEND.wing_offset_style=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.RODEND.wing_offset_style=3;
			else 
				pPoint->des_para.RODEND.wing_offset_style=4;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.RODEND.hRod,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X֫
				else if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X֫
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.RODEND.wing_offset_style==0)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g;
				else if(pPoint->des_para.RODEND.wing_offset_style==1)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g1;
				else if(pPoint->des_para.RODEND.wing_offset_style==2)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g2;
				else if(pPoint->des_para.RODEND.wing_offset_style==3)
					pPoint->des_para.RODEND.wing_offset_dist=jgzj.g3;
			}
			long idProp=GetPropID("des_para.RODEND.wing_offset_dist");
			if(GetPropValueStr(idProp,value_str))
				SetItemPropValue(idProp,CString(value_str));
			SetItemReadOnly(idProp,pPoint->des_para.RODEND.wing_offset_style!=4);
		}
#endif
		else if(GetPropID("des_para.RODEND.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.wing_offset_dist=atof(valueStr);
		else if(GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
		else if(GetPropID("des_para.RODEND.bIncOddEffect")==pItem->m_idProp)		
		{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
				pPoint->des_para.RODEND.bIncOddEffect=TRUE;
			else 
				pPoint->des_para.RODEND.bIncOddEffect=FALSE;
		}
	}
	else if(pPoint->datum_pos_style==2)	//�˼��ڵ㶨λ
	{
		if(GetPropID("des_para.LINEPARTNODE.hLinePart")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODNODE.hRod));
		else if(GetPropID("des_para.LINEPARTNODE.hNode")==pItem->m_idProp)	//��׼�ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODNODE.hNode));
		else if(GetPropID("des_para.LINEPARTNODE.direction")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.direction=valueStr[0]-'0';
		else if(GetPropID("des_para.LINEPARTNODE.offset_wing")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.offset_wing=valueStr[0]-'0';
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("des_para.LINEPARTNODE.wing_offset_style")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.RODNODE.wing_offset_style=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.RODNODE.wing_offset_style=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.RODNODE.wing_offset_style=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.RODNODE.wing_offset_style=3;
			else 
				pPoint->des_para.RODNODE.wing_offset_style=4;
			//
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.RODNODE.hRod,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==0)
					jgzj=pLineAngle->xWingXZhunJu;	//X֫
				else if(pLineAngle->m_bEnableTeG&&pPoint->des_para.RODEND.offset_wing==1)
					jgzj=pLineAngle->xWingYZhunJu;	//X֫
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				//֮ǰδ����ڵ�����׼�����,���׵��½Ǹֶ�ͷ����λ������˨��һ��,��������� wjh-2017.11.23
				CLDSNode* pDatumNode=pLineAngle->BelongModel()->FromNodeHandle(pPoint->m_hNode);
				jgzj.g=ftoi(pPoint->des_para.RODEND.offset_wing==0?pLineAngle->GetNodeWingXG(pDatumNode):pLineAngle->GetNodeWingYG(pDatumNode));

				if(pPoint->des_para.RODNODE.wing_offset_style==0)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g;
				else if(pPoint->des_para.RODNODE.wing_offset_style==1)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g1;
				else if(pPoint->des_para.RODNODE.wing_offset_style==2)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g2;
				else if(pPoint->des_para.RODNODE.wing_offset_style==3)
					pPoint->des_para.RODNODE.wing_offset_dist=jgzj.g3;
			}
			long idProp=GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			if(GetPropValueStr(idProp,value_str))
				SetItemPropValue(idProp,CString(value_str));
			SetItemReadOnly(idProp,pPoint->des_para.RODNODE.wing_offset_style!=4);
		}	
#endif
		else if(GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODNODE.wing_offset_dist=atof(valueStr);
		else if(GetPropID("des_para.LINEPARTNODE.len_offset_dist")==pItem->m_idProp)	
			pPoint->des_para.RODNODE.len_offset_dist=atof(valueStr);
		else if(GetPropID("des_para.LINEPARTNODE.bFaceOffset")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("Yes")==0)
#else 
			if(valueStr.CompareNoCase("��")==0)
#endif
				pPoint->des_para.RODNODE.bFaceOffset=TRUE;
			else 
				pPoint->des_para.RODNODE.bFaceOffset=FALSE;
			DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset"));
			DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]"));
			DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]"));
			DeleteItemByPropId(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]"));
			if(pPoint->des_para.RODNODE.bFaceOffset)
			{	//ƽ���淨��
				CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL,*pRootItem=GetRootItem();
				//
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();

#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Face Offset Plane Normal";
				lpInfo->m_strPropHelp="Face offset plane normal";
#else 
				lpInfo->m_strPropName="ƽ���淨��";
				lpInfo->m_strPropHelp="ƽ���淨��";
#endif
				pPropItem = InsertItem(pRootItem,lpInfo,-1,TRUE);
				pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset");
				if(GetPropValueStr(pPropItem->m_idProp,value_str))
					pPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				int vectorType=CStdVector::GetVectorType(pPoint->des_para.RODNODE.vPlaneNormal);
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="X coordinate of the face offset plane normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,0,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
				if(GetPropValueStr(pSonPropItem->m_idProp,value_str))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Y coordinate of the face offset plane normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
				if(GetPropValueStr(pSonPropItem->m_idProp,value_str))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMONEDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Z coordinate of the face offset plane normal";
#else 
				lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,2,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
				if(GetPropValueStr(pSonPropItem->m_idProp,value_str))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=value_str;
				pSonPropItem->SetReadOnly(vectorType!=0);
			}
		}
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset")==pItem->m_idProp)
		{
			f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.RODNODE.vPlaneNormal=norm;
			long idProp=GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.x);
			SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.y);
			SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			SetItemPropValue(idProp,"%.f",pPoint->des_para.RODNODE.vPlaneNormal.z);
			SetItemReadOnly(idProp,valueStr[0]!='0');

		}
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.x=atof(valueStr);
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.y=atof(valueStr);
		else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==pItem->m_idProp)
			pPoint->des_para.RODNODE.vPlaneNormal.z=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==3)	//�˼����߽���
	{
		if(GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=3;
			else 
				pPoint->des_para.AXIS_INTERS.wing_offset_style1=4;
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG)
				{
					f3dPoint work_norm;
					if(pLineAngle&&pCrossAngle)
					{
						f3dPoint line_vec1=pLineAngle->End()-pLineAngle->Start();
						f3dPoint line_vec2=pCrossAngle->End()-pCrossAngle->Start();
						work_norm=line_vec1^line_vec2;
						normalize(work_norm);
					}
					if(fabs(work_norm*pLineAngle->get_norm_x_wing())>fabs(work_norm*pLineAngle->get_norm_y_wing()))
						jgzj=pLineAngle->xWingXZhunJu;	//X֫
					else 
						jgzj=pLineAngle->xWingYZhunJu;	//Y֫
				}
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==0)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==1)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g1;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==2)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g2;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style1==3)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist1=jgzj.g3;
			}
			long idProp=GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			if(GetPropValueStr(idProp,value_str))
				SetItemPropValue(idProp,CString(value_str));
			SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style1!=4);
		}
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==pItem->m_idProp)
		{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			if(valueStr.CompareNoCase("g")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=0;
			else if(valueStr.CompareNoCase("g1")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=1;
			else if(valueStr.CompareNoCase("g2")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=2;
			else if(valueStr.CompareNoCase("g3")==0)
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=3;
			else 
				pPoint->des_para.AXIS_INTERS.wing_offset_style2=4;
			//
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE);
			CLDSLineAngle *pCrossAngle=(CLDSLineAngle*)console.FromPartHandle(pPoint->des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE);
			if(pLineAngle)
			{
				JGZJ jgzj;
				if(pLineAngle->m_bEnableTeG)
				{
					f3dPoint work_norm;
					if(pLineAngle&&pCrossAngle)
					{
						f3dPoint line_vec1=pLineAngle->End()-pLineAngle->Start();
						f3dPoint line_vec2=pCrossAngle->End()-pCrossAngle->Start();
						work_norm=line_vec1^line_vec2;
						normalize(work_norm);
					}
					if(fabs(work_norm*pLineAngle->get_norm_x_wing())>fabs(work_norm*pLineAngle->get_norm_y_wing()))
						jgzj=pLineAngle->xWingXZhunJu;	//X֫
					else 
						jgzj=pLineAngle->xWingYZhunJu;	//Y֫
				}
				else
					getjgzj(jgzj,pLineAngle->GetWidth());
				if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==0)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==1)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g1;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==2)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g2;
				else if(pPoint->des_para.AXIS_INTERS.wing_offset_style2==3)
					pPoint->des_para.AXIS_INTERS.wing_offset_dist2=jgzj.g3;
			}
			long idProp=GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			if(GetPropValueStr(idProp,value_str))
				SetItemPropValue(idProp,CString(value_str));
			SetItemReadOnly(idProp,pPoint->des_para.AXIS_INTERS.wing_offset_style2!=4);
		}
#endif
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist1=atol(valueStr);
		else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.wing_offset_dist2=atol(valueStr);
		else if(GetPropID("des_para.AXIS_INTERS.near_vector")==pItem->m_idProp)
		{
			f3dPoint vec=CStdVector::GetVector(valueStr[0]-'0');
			pPoint->des_para.AXIS_INTERS.near_vector[0]=vec.x;
			pPoint->des_para.AXIS_INTERS.near_vector[1]=vec.y;
			pPoint->des_para.AXIS_INTERS.near_vector[2]=vec.z;
			long idProp=GetPropID("des_para.AXIS_INTERS.near_vector[0]");
			SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[0]);
			SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=GetPropID("des_para.AXIS_INTERS.near_vector[1]");
			SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[1]);
			SetItemReadOnly(idProp,valueStr[0]!='0');
			idProp=GetPropID("des_para.AXIS_INTERS.near_vector[2]");
			SetItemPropValue(idProp,"%.f",pPoint->des_para.AXIS_INTERS.near_vector[2]);
			SetItemReadOnly(idProp,valueStr[0]!='0');
		}
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[0]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[0]=atof(valueStr);
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[1]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[1]=atof(valueStr);
		else if(GetPropID("des_para.AXIS_INTERS.near_vector[2]")==pItem->m_idProp)
			pPoint->des_para.AXIS_INTERS.near_vector[2]=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==4)	//��׼�߿ռ佻��
	{
		if(GetPropID("des_para.AXIS_INTERS.hDatum1")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum1));
		else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.AXIS_INTERS.hDatum2));
	}
	else if(pPoint->datum_pos_style==5)	//��׼����ָ�����������
	{
		if(GetPropID("des_para.DATUMLINE.hDatumLine")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMLINE.hDatumLine));
		else if(GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==pItem->m_idProp)
			pPoint->des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=valueStr[0]-'0';
	}
	else if(pPoint->datum_pos_style==6)	//��׼����ָ�������
	{
		if(GetPropID("des_para.DATUMPLANE.hDatumPlane")==pItem->m_idProp)
		{
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.DATUMPLANE.hDatumPlane));
			CLDSPlane *pPlane=console.FromPlaneHandle(pPoint->des_para.DATUMPLANE.hDatumPlane);
			if(pPlane)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				CString sValue="Datum Plane��";
#else 
				CString sValue="��׼�棺";
#endif
				CString ss;
				ss.Format("%s_0X%X",pPlane->description,pPlane->handle); 
				sValue+=ss;
				pItem->m_lpNodeInfo->m_strPropHelp=sValue;
				SetPromptString(sValue);
			}
		}
		else if(GetPropID("des_para.DATUMPLANE.cMirTransType")==pItem->m_idProp)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("X Mirror")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='X';
			else if(valueStr.CompareNoCase("Y Mirror")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='Y';
			else if(valueStr.CompareNoCase("Z Mirror")==0)
#else 
			if(valueStr.CompareNoCase("X��Գ�")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='X';
			else if(valueStr.CompareNoCase("Y��Գ�")==0)
				pPoint->des_para.DATUMPLANE.cMirTransType='Y';
			else if(valueStr.CompareNoCase("Z��Գ�")==0)
#endif
				pPoint->des_para.DATUMPLANE.cMirTransType='Z';
			else
				pPoint->des_para.DATUMPLANE.cMirTransType=0;
		}
		else if(GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==pItem->m_idProp)
		{	
			if(valueStr[0]=='0')	 //0.X-Z����(Y��������)
				pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=1;
			else if(valueStr[0]=='1')//1.Y-Z����(X��������)
				pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=0;
			else					 //2.X-Y����(Z��������)
				pPoint->des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=2;
		}
	}
	else if(pPoint->datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
	{
		if(GetPropID("des_para.RODEND.hRod")==pItem->m_idProp)	//�Ǹ־��
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.RODEND.hRod));
		else if(GetPropID("des_para.RODEND.direction")==pItem->m_idProp)	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
			pPoint->des_para.RODEND.direction=valueStr[0]-'0';
		else if(GetPropID("des_para.RODEND.len_offset_dist")==pItem->m_idProp)
			pPoint->des_para.RODEND.len_offset_dist=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==8)	//��ڵ�ͬλ
	{
		if(GetPropID("des_para.hNode")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hNode));
	}
	else if(pPoint->datum_pos_style==9)	//ָ�������ϵ��������
	{
		if(GetPropID("des_para.hPart")==pItem->m_idProp)
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.hPart));
		else if(GetPropID("datum_pos.x")==pItem->m_idProp)
			pPoint->datum_pos.x=atof(valueStr);
		else if(GetPropID("datum_pos.y")==pItem->m_idProp)
			pPoint->datum_pos.y=atof(valueStr);
		else if(GetPropID("datum_pos.z")==pItem->m_idProp)
			pPoint->datum_pos.z=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==10)
	{
		if(GetPropID("des_para.SCALE_NODE.hLinePart")==pItem->m_idProp)			//��׼�˼����
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hLinePart));
		else if(GetPropID("des_para.SCALE_NODE.hStartNode")==pItem->m_idProp)	//ʼ�˽ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hStartNode));
		else if(GetPropID("des_para.SCALE_NODE.hEndNode")==pItem->m_idProp)		//�ն˽ڵ���
			sscanf(valueStr.GetBuffer(1),"%X",&(pPoint->des_para.SCALE_NODE.hEndNode));
		else if(GetPropID("des_para.SCALE_NODE.start_offset_dist")==pItem->m_idProp)//ʼ��ƫ����
			pPoint->des_para.SCALE_NODE.start_offset_dist=atof(valueStr);
		else if(GetPropID("des_para.SCALE_NODE.end_offset_dist")==pItem->m_idProp)	//�ն�ƫ����
			pPoint->des_para.SCALE_NODE.end_offset_dist=atof(valueStr);
		else if(GetPropID("des_para.SCALE_NODE.offset_scale")==pItem->m_idProp)		//ƫ�Ʊ���
			pPoint->des_para.SCALE_NODE.offset_scale=atof(valueStr);
		else if(GetPropID("des_para.SCALE_NODE.offset_dist")==pItem->m_idProp)		//�ڵȷֻ����ϵ�ƫ��ֵ
			pPoint->des_para.SCALE_NODE.offset_dist=atof(valueStr);
	}
	else if(pPoint->datum_pos_style==11)
	{
		if(GetPropID("des_para.COLUMN_INTERS.hLineTube")==pItem->m_idProp)
		{
			pPoint->des_para.COLUMN_INTERS.hLineTube=HexStrToLong(valueStr);
			DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.column_r"));
			DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.axis_vec"));
			DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.axis_vec.x"));
			DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.axis_vec.y"));
			DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.axis_vec.z"));
			//
			CItemInfo *lpInfo=NULL;
			CXhChar100 sText;
			CPropTreeItem *pParentItem = pItem->m_pParent;
			CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(pPoint->des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
			if(pLineTube==NULL)
			{	//����뾶	
				DeleteItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center_type"));
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Cylinder Radius";
				lpInfo->m_strPropHelp = "Cylinder radius";
#else 
				lpInfo->m_strPropName = "����뾶";
				lpInfo->m_strPropHelp = "����뾶";
#endif
				CPropTreeItem *pPropItem = InsertItem(pParentItem,lpInfo,1,TRUE);
				pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.column_r");
				if(GetPropValueStr(pPropItem->m_idProp,sText))
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				//��������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_BUTTON;
				lpInfo->m_buttonType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Cylinder Axis Vector";
				lpInfo->m_strPropHelp = "Cylinder axis vector";
#else 
				lpInfo->m_strPropName = "��������������";
				lpInfo->m_strPropHelp = "��������������";
#endif
				pPropItem = InsertItem(pParentItem,lpInfo,2,TRUE);
				pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.axis_vec");
				if(GetPropValueStr(pPropItem->m_idProp,sText))
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_bHideChildren = TRUE;
				//x
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="X coordinate of the cylinder axis vector";
#else 
				lpInfo->m_strPropHelp="��������X����";
#endif
				CPropTreeItem *pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.axis_vec.x");
				if(GetPropValueStr(pSonPropItem->m_idProp,sText))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				//y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Y coordinate of the cylinder axis vector";
#else 
				lpInfo->m_strPropHelp="��������Y����";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.axis_vec.y");
				if(GetPropValueStr(pSonPropItem->m_idProp,sText))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				//z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Z coordinate of the cylinder axis vector";
#else 
				lpInfo->m_strPropHelp="��������Z����";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.axis_vec.z");
				if(GetPropValueStr(pSonPropItem->m_idProp,sText))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				//չ��
				Expand(pItem,pItem->m_iIndex);
			}
			CPropTreeItem *pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.sect_center_type"),NULL);
			if(pFindItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_COMBO;
				lpInfo->m_cmbType=CDT_LIST;
		#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Location style";
				lpInfo->m_strPropHelp = "Section center location style";
				lpInfo->m_cmbItems="0.User Inpu|1.Parametric";
		#else 
				lpInfo->m_strPropName = "Բ�ĵ㶨λ��ʽ";
				lpInfo->m_strPropHelp = "����Բ�ĵ�Ķ�λ��ʽ";
				lpInfo->m_cmbItems="0.�û�ָ��|1.��������λ";
		#endif
				if(pLineTube==NULL)
					pFindItem = InsertItem(pParentItem,lpInfo,3,TRUE);
				else
					pFindItem = InsertItem(pParentItem,lpInfo,1,TRUE);
				pFindItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center_type");
			}
			if(pLineTube==NULL)
			{	//�������ֹ�ʱ��Ӧȷ�����Ͳ�Ϊ��������λ���Ҳ���ѡ
				m_xPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag&=0xfd;
				pFindItem->SetReadOnly();
			}
			else
				pFindItem->SetReadOnly(FALSE);
			if(GetPropValueStr(pFindItem->m_idProp,sText))
				pFindItem->m_lpNodeInfo->m_strPropValue = sText;
			UpdateColumnIntersSectCenterParamProperty(pFindItem);
		}
		else if(GetPropID("des_para.COLUMN_INTERS.sect_center_type")==pItem->m_idProp)
		{
			if(valueStr.GetAt(0)=='0')
				m_xPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag&=0xFD;
			else
				m_xPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag|=0x02;
			UpdateColumnIntersSectCenterParamProperty(pItem);
		}
		else if(GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode")==pItem->m_idProp)
		{
			if(m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode>0)
				m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=HexStrToLong(valueStr);
			else
				m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-HexStrToLong(valueStr);
		}
		else if(GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset_type")==pItem->m_idProp)
		{
			if(valueStr[0]=='0')
				m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=abs(m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode);
			else
				m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-abs(m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode);
		}
		else if(GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset")==pItem->m_idProp)
			m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.len_offset=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.column_r")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.column_r=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.x")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[0]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.y")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[1]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.z")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.AxisVector.axis_vec[2]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.sect_center.x")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[0]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.sect_center.y")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[1]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.sect_center.z")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectCenter.sect_center[2]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.sect_norm_std_style")==pItem->m_idProp)
		{
			long idPropX = GetPropID("des_para.COLUMN_INTERS.sect_norm.x");
			long idPropY = GetPropID("des_para.COLUMN_INTERS.sect_norm.y");
			long idPropZ = GetPropID("des_para.COLUMN_INTERS.sect_norm.z");
			if(valueStr[0]=='0')	//�Զ��巽��
			{
				SetItemReadOnly(idPropX,FALSE);
				SetItemReadOnly(idPropY,FALSE);
				SetItemReadOnly(idPropZ,FALSE);
			}
			else
			{
				SetItemReadOnly(idPropX,TRUE);
				SetItemReadOnly(idPropY,TRUE);
				SetItemReadOnly(idPropZ,TRUE);
				f3dPoint norm=CStdVector::GetVector(valueStr[0]-'0');
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[0]=norm.x;
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[1]=norm.y;
				pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[2]=norm.z;
				CXhChar100 sText;
				if(GetPropValueStr(idPropX,sText))
					SetItemPropValue(idPropX,sText);
				if(GetPropValueStr(idPropY,sText))
					SetItemPropValue(idPropY,sText);
				if(GetPropValueStr(idPropZ,sText))
					SetItemPropValue(idPropZ,sText);
			}
		}
		else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.x")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[0]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.y")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[1]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.z")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.SectNorm.sect_norm[2]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.ray_vec_style")==pItem->m_idProp)
		{	//���߷����巽ʽ
			pPoint->des_para.COLUMN_INTERS.ray_vec_style=valueStr[0]-'0';
			DeleteAllSonItems(pItem);
			CXhChar100 sText;
			if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0||pPoint->des_para.COLUMN_INTERS.ray_vec_style==1)
			{	//��������
				CItemInfo* lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Specify";
				lpInfo->m_strPropHelp = "Specify vector";
#else 
				lpInfo->m_strPropName = "ָ������";
				lpInfo->m_strPropHelp = "ָ������";
#endif
				CPropTreeItem* pSonPropItem = InsertItem(pItem,lpInfo, -1,TRUE);
				pSonPropItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style");
				if(GetPropValueStr(pSonPropItem->m_idProp,sText))
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="X coordinate of ray vertor";
				else 
					lpInfo->m_strPropHelp="X coordinate of datum edge vertor";
#else 
					lpInfo->m_strPropHelp="���߷���X����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���X����";
#endif
				pSonPropItem = InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_vec.x");
				if(GetPropValueStr(pSonPropItem->m_idProp,sText))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="X coordinate of ray vertor";
				else 
					lpInfo->m_strPropHelp="X coordinate of datum edge vertor";
#else 
					lpInfo->m_strPropHelp="���߷���Y����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���Y����";
#endif
				pSonPropItem = InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_vec.y");
				if(GetPropValueStr(pSonPropItem->m_idProp,sText))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
				if(pPoint->des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="X coordinate of ray vertor";
				else 
					lpInfo->m_strPropHelp="X coordinate of datum edge vertor";
#else 
					lpInfo->m_strPropHelp="���߷���Z����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���Z����";
#endif
				pSonPropItem = InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_vec.z");
				if(GetPropValueStr(pSonPropItem->m_idProp,sText))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
				pSonPropItem->SetReadOnly(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
			}
			if(pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0&&pPoint->des_para.COLUMN_INTERS.ray_vec_style!=4)
			{	//��ת�Ƕ�
				CItemInfo* lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Rotate Angle";
				lpInfo->m_strPropHelp="Rotate angle relates to specify datum edge,unit is ��";
#else 
				lpInfo->m_strPropName="��ת�Ƕ�";
				lpInfo->m_strPropHelp="�����ָ����׼�ߵ���ת�Ƕ�,��λΪ��";
#endif
				CPropTreeItem* pSonPropItem = InsertItem(pItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_angle");
				if(GetPropValueStr(pSonPropItem->m_idProp,sText))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
			}
		}
		else if(GetPropID("des_para.COLUMN_INTERS.ray_angle")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.ray_angle=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style")==pItem->m_idProp)
		{
			long idPropX = GetPropID("des_para.COLUMN_INTERS.ray_vec.x");
			long idPropY = GetPropID("des_para.COLUMN_INTERS.ray_vec.y");
			long idPropZ = GetPropID("des_para.COLUMN_INTERS.ray_vec.z");
			if(valueStr[0]=='0')	//�Զ��巽��
			{
				SetItemReadOnly(idPropX,pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				SetItemReadOnly(idPropY,pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
				SetItemReadOnly(idPropZ,pPoint->des_para.COLUMN_INTERS.ray_vec_style!=0);
			}
			else
			{
				SetItemReadOnly(idPropX,TRUE);
				SetItemReadOnly(idPropY,TRUE);
				SetItemReadOnly(idPropZ,TRUE);
				f3dPoint vector=CStdVector::GetVector(valueStr[0]-'0');
				pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[0]=vector.x;
				pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[1]=vector.y;
				pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[2]=vector.z;
				CXhChar100 sText;
				if(GetPropValueStr(idPropX,sText))
					SetItemPropValue(idPropX,sText);
				if(GetPropValueStr(idPropY,sText))
					SetItemPropValue(idPropY,sText);
				if(GetPropValueStr(idPropZ,sText))
					SetItemPropValue(idPropZ,sText);
			}
		}
		else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.x")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[0]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.y")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[1]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.z")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.RayVector.ray_vec[2]=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.ray_para_offset")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.ray_para_offset=atof(valueStr);
		else if(GetPropID("des_para.COLUMN_INTERS.bSpecRo")==pItem->m_idProp)
		{
			pPoint->des_para.COLUMN_INTERS.bSpecRo=(valueStr[0]-'0')!=0;
			CPropTreeItem *pFindItem = FindItemByPropId(GetPropID("des_para.COLUMN_INTERS.ray_offset_r"),NULL);
			if(pFindItem)
			{
				if(pPoint->des_para.COLUMN_INTERS.bSpecRo)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					SetItemPropName(pFindItem->m_idProp,CString("Ray Radius"));
					pFindItem->m_lpNodeInfo->m_strPropHelp="The radius of ray tip, based on cylinder center";
#else 
					SetItemPropName(pFindItem->m_idProp,CString("���߰뾶"));
					pFindItem->m_lpNodeInfo->m_strPropHelp="������Ϊ��������߰뾶�������߷����ƫ����";
#endif
				}
				else 
				{
#ifdef AFX_TARG_ENU_ENGLISH
					SetItemPropName(pFindItem->m_idProp,CString("Offset to ray tip"));
					pFindItem->m_lpNodeInfo->m_strPropHelp="The offset along ray vector from cylinder center";
#else 
					SetItemPropName(pFindItem->m_idProp,CString("���߶�ƫ����"));
					pFindItem->m_lpNodeInfo->m_strPropHelp="������Ϊ��׼�������߷����ƫ����";
#endif
				}
			}
		}
		else if(GetPropID("des_para.COLUMN_INTERS.ray_offset_r")==pItem->m_idProp)
			pPoint->des_para.COLUMN_INTERS.ray_offset_r=atof(valueStr);
	}
	if(bRefreshPtPara)
		UpdateDatumPointParaProperty();
	if(ReflectModifyValueFunc)
		ReflectModifyValueFunc(this,pItem,valueStr);
	if(lpRslt)
		*lpRslt=TRUE;
	return TRUE;
}
CString GetPointHandleStr(bool bIncNewItem=FALSE)
{
	CString  sPointHandle="";
	CString sValue;
	int nPointNum=Ta.Point.GetNodeNum();
	for(int i=0;i<nPointNum;i++)
	{
		CLDSPoint Point=Ta.Point[i];
		sValue.Format("0X%X|",Point.handle);
		sPointHandle+=sValue;
	}
	if(bIncNewItem)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sPointHandle+="<New...>";
#else 
		sPointHandle+="<�½�...>";
#endif
	}
	return sPointHandle;
}
CString GetPlaneHandleStr(bool bIncNewItem=FALSE)
{
	CString sPlaneHandle="";
	CString sValue;
	int nPlane=Ta.Plane.GetNodeNum();
	for(int i=0;i<nPlane;i++)
	{
		CLDSPlane pPlane=Ta.Plane[i];
		if(i==nPlane-1)
			sValue.Format("0X%X",pPlane.handle);
		else 
			sValue.Format("0X%X|",pPlane.handle);
		sPlaneHandle+=sValue;
	}
	if(bIncNewItem)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sPlaneHandle+="<New...>";
#else 
		sPlaneHandle+="<�½�...>";
#endif
	}
	return sPlaneHandle;
}
static CString GetSelectPlaneDesc(CLDSPoint* pPoint)
{
#ifdef AFX_TARG_ENU_ENGLISH
	CString sDescription="Datum Plane��";
#else 
	CString sDescription="��׼�棺";
#endif
	CString ss;
	CLDSPlane *pPlane=console.FromPlaneHandle(pPoint->des_para.DATUMPLANE.hDatumPlane);
	if(pPlane)
	{
		ss.Format("%s_0X%X",pPlane->description,pPlane->handle);
		sDescription+=ss;
	}
	return sDescription;
}
void CParaPointPropertyList::DisplayPropertyList(int overwrite0_append1/*=0*/,DWORD dwPropGroup/*=1*/)
{
	CXhChar100 sText;
	CPropTreeItem *pPropItem=NULL,*pGroupItem=NULL,*pLeafItem=NULL,*pParentItem=GetRootItem();
	if(overwrite0_append1==0)
	{
		CleanTree();
		m_nObjClassTypeId = 0;
	}
	CItemInfo* lpInfo=NULL;
	if(m_bDisBaseInfo||m_bDisPosStyle)
	{
		lpInfo=new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Basic Info";
#else 
		lpInfo->m_strPropName = "������Ϣ";
#endif
		pGroupItem = InsertItem(pParentItem,lpInfo,-1);
		pGroupItem->m_idProp = GetPropID("basic");
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_dwPropGroup=dwPropGroup;
	}
	if(m_bDisBaseInfo)
	{
		//���
		lpInfo=new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbItems=GetPointHandleStr(TRUE);
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Handle";
		lpInfo->m_strPropHelp="Handle of datum point";
#else 
		lpInfo->m_strPropName="���";
		lpInfo->m_strPropHelp="��׼��ľ��";
#endif
		pPropItem = InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp=GetPropID("hDatumPt");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pPropItem->SetReadOnly(!m_bDisPosStyle);
		pPropItem->m_dwPropGroup=dwPropGroup;
		//����
		lpInfo=new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Description";
		lpInfo->m_strPropHelp="Description of datum point";
#else 
		lpInfo->m_strPropName="����";
		lpInfo->m_strPropHelp="��׼�������";
#endif
		pPropItem = InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp=GetPropID("datum_pos_desc");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
			pPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pPropItem->m_dwPropGroup=dwPropGroup;
	}
	if(m_bDisPosStyle)
	{
		//��׼�㶨λ��ʽ
		lpInfo=new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Manual Input|1.Angle Ridge Vertex|2.Rod Node Position|3.Bolt line Intersection Point|4.Datum Line Intersection|5.Coord. Specified On Datum Line|6.Coord. Specified  On Datum Plane|7.Angle Ridge Offset|8.Same Position With Node|9.Relative Coordinate On Datum Part";
		lpInfo->m_strPropName="Position Style";
		lpInfo->m_strPropHelp="Position style of datum point";
#else 
		lpInfo->m_cmbItems="0.�ֶ�����|1.�Ǹ���㶨λ��|2.�˼��ڵ㶨λ��|3.�Ǹ����߽���|4.��׼�߿ռ佻��|5.��׼����ָ�����������|6.��׼����ָ�������|7.��֫����ƫ�Ƶİ������|8.��ڵ�ͬλ|9.��׼�����ϵ��������";
		lpInfo->m_strPropName="��λ��ʽ";
		lpInfo->m_strPropHelp="��׼��Ķ�λ��ʽ";
#endif
		pPropItem = InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp=GetPropID("datum_pos_style");
		if(GetPropValueStr(pPropItem->m_idProp,sText))
		pPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pPropItem->m_dwPropGroup=dwPropGroup;
	}
	lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Parameter Info";
#else 
	lpInfo->m_strPropName = "������Ϣ";
#endif
	pGroupItem = InsertItem(pParentItem,lpInfo,-1);
	pGroupItem->m_idProp = GetPropID("des_para");
	pGroupItem->m_bHideChildren=FALSE;
	pGroupItem->m_dwPropGroup=dwPropGroup;
	UpdateDatumPointParaProperty();
	
	Redraw();
}
CString GetLineHandleStr(bool bIncNewItem=FALSE)
{
	CString sLineHandleArr,sValue;
	for(CLDSLine* pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
	{
		sValue.Format("0X%X|",pLine->handle);
		sLineHandleArr+=sValue;
	}
	if(bIncNewItem)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sLineHandleArr+="<New...>";
#else
		sLineHandleArr+="<�½�...>";
#endif
	}
	return sLineHandleArr;
}
void CParaPointPropertyList::UpdateDatumPointParaProperty()
{
	CItemInfo* lpInfo=NULL;
	CXhChar100 valueStr;
	CPropTreeItem *pPropItem=NULL,*pSonPropItem=NULL;
	CPropTreeItem* pParentItem=FindItemByPropId(GetPropID("des_para"),NULL);
	DeleteAllSonItems(pParentItem);
	if(m_xPoint.datum_pos_style==0)		//ֱ��ָ������
	{	//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="X coordinate of datum point";
#else 
		lpInfo->m_strPropHelp="��׼��X����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("pos.x");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Y coordinate of datum point";
#else 
		lpInfo->m_strPropHelp="��׼��Y����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("pos.y");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Z coordinate of datum point";
#else 
		lpInfo->m_strPropHelp="��׼��Z����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("pos.z");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==1)	//�Ǹ����
	{	//��׼�Ǹ־��
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_xPoint.des_para.hPart,CLS_LINEPART);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		if(pLinePart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Unknown Part Handle";
		else 
			lpInfo->m_strPropName.Format("Datum %s Handle",pLinePart->GetPartTypeName());
#else 
			lpInfo->m_strPropName="δ֪�������";
		else 
			lpInfo->m_strPropName.Format("��׼%s���",pLinePart->GetPartTypeName());
#endif
		lpInfo->m_strPropHelp=lpInfo->m_strPropName;
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.hRod");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		if(pLinePart==NULL||pLinePart->IsAngle())
		{	//ƫ��֫
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset leg.";
#else 
			lpInfo->m_cmbItems="0.X֫|1.Y֫";
			lpInfo->m_strPropName="ƫ��֫";
			lpInfo->m_strPropHelp="ƫ��֫��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.RODEND.offset_wing");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined|";
			lpInfo->m_strPropName="Leg Offset Style";
			lpInfo->m_strPropHelp="Offset style is preconcert stadia or user-defining offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.RODEND.wing_offset_style");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset";
			lpInfo->m_strPropHelp="Leg offset.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.RODEND.wing_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(m_xPoint.des_para.RODEND.wing_offset_style!=4);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.norm_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum rod from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�˼�ʼ->�շ���ƫ�ƻ��ػ�׼�˼���->ʼƫ�ơ�";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.direction");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length offset";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.len_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ��ֵ��������ͷӰ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Oddment Effect";
		lpInfo->m_strPropHelp="Consider ength offset in oddment effect or not.";
#else 
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName="����ͷӰ��";
		lpInfo->m_strPropHelp="����ƫ���Ƿ��������ͷӰ�졣";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.bIncOddEffect");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==2)	//�˼��Ͻڵ㶨λ
	{
		//��׼�˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod Handle";
		lpInfo->m_strPropHelp="Datum rod HANDLE.";
#else 
		lpInfo->m_strPropName="��׼�˼����";
		lpInfo->m_strPropHelp="��׼�˼������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.hLinePart");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//��׼�ڵ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Handle";
		lpInfo->m_strPropHelp="Datum node Handle.";
#else 
		lpInfo->m_strPropName="��׼�ڵ���";
		lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.hNode");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(m_xPoint.des_para.RODNODE.hRod,CLS_LINEPART);
		if(pLinePart&&pLinePart->IsAngle())
		{	//ƫ��֫
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.X Leg|1.Y Leg";
			lpInfo->m_strPropName="Offset Leg";
			lpInfo->m_strPropHelp="Offset leg.";
#else 
			lpInfo->m_cmbItems="0.X֫|1.Y֫";
			lpInfo->m_strPropName="ƫ��֫";
			lpInfo->m_strPropHelp="ƫ��֫��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.offset_wing");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined|";
			lpInfo->m_strPropName="Leg Offset Style";
			lpInfo->m_strPropHelp="Offset style is preconcert stadia or user-defining offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������";
			lpInfo->m_strPropHelp="ƫ������Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.wing_offset_style");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset";
			lpInfo->m_strPropHelp="Leg offset.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ";
			lpInfo->m_strPropHelp="֫��ƫ��ֵ��";
#endif
			pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.wing_offset_dist");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(m_xPoint.des_para.RODNODE.wing_offset_style!=4);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum rod from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�˼�ʼ->�շ���ƫ�ƻ��ػ�׼�˼���->ʼƫ�ơ�";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.direction");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length offset";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp =GetPropID("des_para.LINEPARTNODE.len_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.norm_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ƽ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Yes|No";
		lpInfo->m_strPropName="Plane Offset";
		lpInfo->m_strPropHelp="Plane offset";
#else 
		lpInfo->m_cmbItems="��|��";
		lpInfo->m_strPropName="ƽ��";
		lpInfo->m_strPropHelp="ƽ��";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp =GetPropID("des_para.LINEPARTNODE.bFaceOffset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		if(m_xPoint.des_para.RODNODE.bFaceOffset)
		{	//ƽ���淨��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
			lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Face Offset Plane Normal";
			lpInfo->m_strPropHelp="Face offset plane noarma";
#else 
			lpInfo->m_strPropName="ƽ���淨��";
			lpInfo->m_strPropHelp="ƽ���淨��";
#endif
			pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp =GetPropID("des_para.LINEPARTNODE.face_norm_offset");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			int vectorType=CStdVector::GetVectorType(m_xPoint.des_para.RODNODE.vPlaneNormal);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="X coordinate of face offset plane normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��X�������";
#endif
			pSonPropItem =InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp =GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Y coordinate of face offset plane normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��Y�������";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Z coordinate of face offset plane normal";
#else 
			lpInfo->m_strPropHelp="ƽ���淨��Z�������";
#endif
			pSonPropItem =InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pSonPropItem->SetReadOnly(vectorType!=0);
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
	else if(m_xPoint.datum_pos_style==3)	//�˼����߽���
	{
		//��׼�˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod 1 Handle";
		lpInfo->m_strPropHelp="Datum rod 1 handle.";
#else 
		lpInfo->m_strPropName="��׼�˼�1���";
		lpInfo->m_strPropHelp="��׼�˼�1�����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.hDatum1");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		CLDSLinePart *pLinePart1=(CLDSLinePart*)console.FromPartHandle(m_xPoint.des_para.AXIS_INTERS.hDatum1,CLS_LINEPART);
		if(pLinePart1&&pLinePart1->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined|";
			lpInfo->m_strPropName="Leg Offset Style 1";
			lpInfo->m_strPropHelp="Datum angle offset style: preconcert stadia or user-defining offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.wing_offset_style1");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset 1";
			lpInfo->m_strPropHelp="Leg offset of datum angle 1.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ1";
			lpInfo->m_strPropHelp="��׼�Ǹ�1֫��ƫ��ֵ��";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.wing_offset_dist1");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(m_xPoint.des_para.AXIS_INTERS.wing_offset_style1!=4);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		//��׼�˼�2���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod 2 Handle";
		lpInfo->m_strPropHelp="Datum rod 2 handle.";
#else 
		lpInfo->m_strPropName="��׼�˼�2���";
		lpInfo->m_strPropHelp="��׼�˼�2���";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.hDatum2");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		CLDSLinePart *pLinePart2=(CLDSLinePart*)console.FromPartHandle(m_xPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEPART);
		if(pLinePart2&&pLinePart2->IsAngle())
		{	//ƫ������  0.g;1.g1;2.g2;3.g3;4.�Զ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined|";
			lpInfo->m_strPropName="Leg Offset Style 2";
			lpInfo->m_strPropHelp="Datum angle 2 offset style: preconcert stadia or user-defining offset.";
#else 
			lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���|";
			lpInfo->m_strPropName="֫��ƫ������2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2ƫ������:Ԥ��׼����Զ���ƫ�ƾ��롣";
#endif
			pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.wing_offset_style2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ƫ�ƾ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Leg Offset2";
			lpInfo->m_strPropHelp="Leg offset of datum angle 2.";
#else 
			lpInfo->m_strPropName="֫��ƫ��ֵ2";
			lpInfo->m_strPropHelp="��׼�Ǹ�2֫��ƫ��ֵ��";
#endif
			pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.wing_offset_dist2");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			pPropItem->SetReadOnly(m_xPoint.des_para.AXIS_INTERS.wing_offset_style2!=4);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.norm_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//���Ʒ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbItems=CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Near Vector";
		lpInfo->m_strPropHelp="Near vector";
#else 
		lpInfo->m_strPropName="���Ʒ���";
		lpInfo->m_strPropHelp="���Ʒ���";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.AXIS_INTERS.near_vector");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		int vectorType=CStdVector::GetVectorType(f3dPoint(m_xPoint.des_para.AXIS_INTERS.near_vector));
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="X coordinate of near vertor";
#else 
		lpInfo->m_strPropHelp="���Ʒ���X�������";
#endif
		pSonPropItem =InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.near_vector[0]");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Y coordinate of near vertor";
#else 
		lpInfo->m_strPropHelp="���Ʒ���Y�������";
#endif
		pSonPropItem =InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.near_vector[1]");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Z coordinate of near vertor";
#else 
		lpInfo->m_strPropHelp="���Ʒ���Z�������";
#endif
		pSonPropItem =InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.near_vector[2]");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->SetReadOnly(vectorType!=0);
		pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==4)	//��׼�߽���
	{
		//��׼��1���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line 1 Handle";
		lpInfo->m_strPropHelp="Datum line 1 handle";
#else 
		lpInfo->m_strPropName="��׼��1���";
		lpInfo->m_strPropHelp="��׼��1���";
#endif
		lpInfo->m_cmbItems=GetLineHandleStr();
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.hDatum1");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//��׼��2���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line 2 Handle";
		lpInfo->m_strPropHelp="Datum line 2 handle";
#else 
		lpInfo->m_strPropName="��׼��2���";
		lpInfo->m_strPropHelp="��׼��2���";
#endif
		lpInfo->m_cmbItems=GetLineHandleStr();
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp =GetPropID("des_para.AXIS_INTERS.hDatum2");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==5)	//��׼�����������겻���
	{
		//��׼�߾��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
		lpInfo->m_cmbItems=GetLineHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Line Handle";
		lpInfo->m_strPropHelp="Datum line handle";
#else 
		lpInfo->m_strPropName="��׼�߾��";
		lpInfo->m_strPropHelp="��׼�߾��";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.DATUMLINE.hDatumLine");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ָ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X Axis|1.Y Axis|2.Z Axis";
		lpInfo->m_strPropName="Specify";
		lpInfo->m_strPropHelp="Specify axis";
#else 
		lpInfo->m_cmbItems="0.X��|1.Y��|2.Z��";
		lpInfo->m_strPropName="ָ������";
		lpInfo->m_strPropHelp="ָ������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==6)	//��׼�����������겻���
	{
		//��׼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
		lpInfo->m_cmbItems=GetPlaneHandleStr();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Plane Handle";
#else 
		lpInfo->m_strPropName="��׼����";
#endif
		lpInfo->m_strPropHelp=GetSelectPlaneDesc(&m_xPoint);
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.DATUMPLANE.hDatumPlane");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ԳƱ任����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="None|X Mirror|Y Mirror|Z Mirror";
		lpInfo->m_strPropName="Reference Point Mirror Style";
		lpInfo->m_strPropHelp="First according to current point create mirroring reference point,second intersects with datum plane,then mirrors to origin position";
#else 
		lpInfo->m_cmbItems="�޶ԳƲ���|X��Գ�|Y��Գ�|Z��Գ�";
		lpInfo->m_strPropName="���յ�ԳƷ�ʽ";
		lpInfo->m_strPropHelp="�ȸ��ݵ�ǰ�����ɶԳƵĲ��յ㣬�����׼�����������Ȼ���ٶԳƻ�ԭλ�á�";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.DATUMPLANE.cMirTransType");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ָ������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.X-Z |1.Y-Z |2.X-Y ";
		lpInfo->m_strPropName="Specify";
		lpInfo->m_strPropHelp="Specify coordinate axis";
#else 
		lpInfo->m_cmbItems="0.X-Z����|1.Y-Z����|2.X-Y����";
		lpInfo->m_strPropName="ָ���������";
		lpInfo->m_strPropHelp="ָ���������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==7)	//�ؽǸ���֫����ƫ�Ƶİ������
	{
		//��׼�Ǹ־��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Angle Handle";
		lpInfo->m_strPropHelp="Datum angle handle.";
#else 
		lpInfo->m_strPropName="��׼�Ǹ־��";
		lpInfo->m_strPropHelp="��׼�Ǹ־����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.hRod");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ�Ʒ��� 0.ʼ->��;1.��->ʼ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Start->End|1.End->Start|";
		lpInfo->m_strPropName="Length Offset Vertor";
		lpInfo->m_strPropHelp="Length offset vertor,along datum angle from start to end or from end to start offset.";
#else 
		lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ|";
		lpInfo->m_strPropName="����ƫ�Ʒ���";
		lpInfo->m_strPropHelp="����ƫ�Ʒ���,�ػ�׼�Ǹ�ʼ->�շ���ƫ�ƻ��ػ�׼�Ǹ���->ʼƫ�ơ�";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp =GetPropID("des_para.RODEND.direction");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ��ֵ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Length Offset";
		lpInfo->m_strPropHelp="Length offset";
#else 
		lpInfo->m_strPropName="����ƫ��ֵ";
		lpInfo->m_strPropHelp="����ƫ��ֵ";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.len_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Normal Offset";
		lpInfo->m_strPropHelp="Normal offset";
#else 
		lpInfo->m_strPropName="����ƫ����";
		lpInfo->m_strPropHelp="����ƫ����";
#endif
		pPropItem =InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.RODEND.norm_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==8)	//��ڵ�ͬΪ
	{	//��׼�ڵ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Node Handle";
		lpInfo->m_strPropHelp="Datum node handle.";
#else 
		lpInfo->m_strPropName="��׼�ڵ���";
		lpInfo->m_strPropHelp="��׼�ڵ�����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.hNode");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==9)	//ָ�������ϵ��������
	{
		//��׼�˼����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Part Handle";
		lpInfo->m_strPropHelp="Datum part handle.";
#else 
		lpInfo->m_strPropName="��׼�������";
		lpInfo->m_strPropHelp="��׼���������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.hPart");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//�������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Relative Coord.";
		lpInfo->m_strPropHelp="Relative coordinate";
#else 
		lpInfo->m_strPropName="�������";
		lpInfo->m_strPropHelp="�������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("datum_pos");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//X
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="X coordinate of relative coordinate";
#else 
		lpInfo->m_strPropHelp="�������X�������";
#endif
		pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = GetPropID("datum_pos.x");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//Y
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Y coordinate of relative coordinate";
#else 
		lpInfo->m_strPropHelp="�������Y�������";
#endif
		pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = GetPropID("datum_pos.y");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//Z
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropHelp="Z coordinate of relative coordinate";
#else 
		lpInfo->m_strPropHelp="�������Z�������";
#endif
		pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
		pSonPropItem->m_idProp = GetPropID("datum_pos.z");
		if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
			pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	else if(m_xPoint.datum_pos_style==10)
	{	//λ��
		//��ϽǸ־��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Datum Rod Handle";
		lpInfo->m_strPropHelp="Datum rod handle,when datum node is rod's datum rod end point,it will consider oddment information.";
#else 
		lpInfo->m_strPropName="��׼�˼����";
		lpInfo->m_strPropHelp="��׼�˼��������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.hLinePart");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ʼ�˻�׼�ڵ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Start Datum Node Handle";
		lpInfo->m_strPropHelp="Datum node handle of start��when datum node is rod's datum rod end point,it will consider oddment information.��";
#else 
		lpInfo->m_strPropName="ʼ�˻�׼�ڵ���";
		lpInfo->m_strPropHelp="ʼ�˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.hStartNode");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ʼ��ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Start Offset";
		lpInfo->m_strPropHelp="Offset,start node along the direction from start node to end node.";
#else 
		lpInfo->m_strPropName="ʼ��ƫ����";
		lpInfo->m_strPropHelp="ʼ�˽ڵ�����ʼ�˽ڵ�->�ն˽ڵ㷽���ƫ������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.start_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//�ն˻�׼�ڵ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="End Datum Node Handle";
		lpInfo->m_strPropHelp="Datum node handle of end��when datum node is rod's datum rod end point,it will consider oddment information.";
#else 
		lpInfo->m_strPropName="�ն˻�׼�ڵ���";
		lpInfo->m_strPropHelp="�ն˻�׼�ڵ�������׼�ڵ�Ϊ�˼���׼�˼��˵�ʱ�ῼ�Ǹ˼�������ͷ��Ϣ��";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.hEndNode");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//�ն�ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="End Offset";
		lpInfo->m_strPropHelp="Offset,end node along the direction from end node to start node.";
#else 
		lpInfo->m_strPropName="�ն�ƫ����";
		lpInfo->m_strPropHelp="�ն˽ڵ������ն˽ڵ�->ʼ�˽ڵ㷽���ƫ������";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.end_offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ƫ�Ʊ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Offset Scale";
		lpInfo->m_strPropHelp="From start node position begins calculating after considering start offset";
#else 
		lpInfo->m_strPropName="ƫ�Ʊ���";
		lpInfo->m_strPropHelp="��ʼ�˽ڵ㿼��ʼ��ƫ�����Ժ��λ�ÿ�ʼ���㡣";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.offset_scale");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ƫ�ƾ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Offset";
		lpInfo->m_strPropHelp="Offset distance after calculating according to offset scale.";
#else 
		lpInfo->m_strPropName="ƫ�ƾ���";
		lpInfo->m_strPropHelp="��ƫ�Ʊ��������Ժ��ٽ���ƫ�Ƶľ��롣";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.SCALE_NODE.offset_dist");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
	//���涨λ��
	else if(m_xPoint.datum_pos_style==11)
	{	//�ֹܾ��
		lpInfo= new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Datum Tube Handle";
		lpInfo->m_strPropHelp = "Tube handle that used for confirming cylinder";
#else 
		lpInfo->m_strPropName = "��׼�ֹܾ��";
		lpInfo->m_strPropHelp = "����ȷ������ĸֹܾ��";
#endif
		CPropTreeItem *pTubePropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pTubePropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.hLineTube");
		if(GetPropValueStr(pTubePropItem->m_idProp,valueStr))
			pTubePropItem->m_lpNodeInfo->m_strPropValue = valueStr;
		pTubePropItem->m_bHideChildren = FALSE;
		pTubePropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

		CLDSLineTube *pLineTube = (CLDSLineTube*)console.FromPartHandle(m_xPoint.des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		if(pLineTube==NULL)
		{	//����뾶	
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Cylinder Radius";
			lpInfo->m_strPropHelp = "Cylinder radius";
#else 
			lpInfo->m_strPropName = "����뾶";
			lpInfo->m_strPropHelp = "����뾶";
#endif
			pPropItem = InsertItem(pTubePropItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.column_r");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//��������������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Cylinder Axis Vector";
			lpInfo->m_strPropHelp = "Cylinder axis vector";
#else 
			lpInfo->m_strPropName = "�������߷���";
			lpInfo->m_strPropHelp = "��������������";
#endif
			pPropItem = InsertItem(pTubePropItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.axis_vec");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//if(m_bDisplayPointSonItem)
			{
				pPropItem->m_bHideChildren = TRUE;
				//x
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="X coordinate of cylinder axis";
#else 
				lpInfo->m_strPropHelp="��������X����";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.axis_vec.x");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Y coordinate of cylinder axis";
#else 
				lpInfo->m_strPropHelp="��������Y����";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.axis_vec.y");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
				//z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Z coordinate of cylinder axis";
#else 
				lpInfo->m_strPropHelp="��������Z����";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.axis_vec.z");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			}
		}
		//����Բ�ĵ㶨λ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Location style";
		lpInfo->m_strPropHelp = "Section center location style";
		lpInfo->m_cmbItems="0.User Inpu|1.Parametric";
#else 
		lpInfo->m_strPropName = "Բ�ĵ㶨λ��ʽ";
		lpInfo->m_strPropHelp = "����Բ�ĵ�Ķ�λ��ʽ";
		lpInfo->m_cmbItems="0.�û�ָ��|1.��������λ";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center_type");
		if(pLineTube==NULL)
		{	//�������ֹ�ʱ��Ӧȷ�����Ͳ�Ϊ��������λ���Ҳ���ѡ
			m_xPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag&=0xfd;
			pPropItem->SetReadOnly();
		}
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
		if(pLineTube!=NULL&&m_xPoint.des_para.COLUMN_INTERS.cPointAndVectorFlag&0x02)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset from node";
			lpInfo->m_strPropHelp = "Section center's location offset datum node";
#else 
			lpInfo->m_strPropName = "ƫ����ʼ�ڵ�";
			lpInfo->m_strPropHelp = "����Բ�ĵ��ƫ�ƶ�λ��ʼ�ڵ�";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset Direction";
			lpInfo->m_strPropHelp = "Offset Direction from datum node along rod";
			lpInfo->m_cmbItems="0.Start->End|1.End->Start";
#else 
			lpInfo->m_strPropName = "ƫ�Ʒ���";
			lpInfo->m_strPropHelp = "��ƫ����ʼ�ڵ��ػ�׼�ֹܵ�ƫ�Ʒ���";
			lpInfo->m_cmbItems="0.ʼ->��|1.��->ʼ";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset_type");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Offset Distance";
			lpInfo->m_strPropHelp = "Offset distance from datum node";
#else 
			lpInfo->m_strPropName = "ƫ����";
			lpInfo->m_strPropHelp = "����Բ�ĵ���ƫ����ʼ�ڵ��ƫ����";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.SectCenter.len_offset");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
		}
		else
		{	//ָ������Բ�ĵ�
#ifdef AFX_TARG_ENU_ENGLISH
			CString sPropName = "Section Center";
			if(pLineTube)
				sPropName = "Section Pick Point";
#else 
			CString sPropName = "����Բ��";
			if(pLineTube)
				sPropName = "����ʰȡ��";
#endif
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_BUTTON;
			lpInfo->m_buttonType=BDT_COMMON;
			lpInfo->m_strPropName = sPropName;
			if(pLineTube)
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Section pick point used to calculate section center";
			else 
				lpInfo->m_strPropHelp = "Section center";
#else 
				lpInfo->m_strPropHelp = "����ʰȡ�����������Բ�ĵ�";
			else 
				lpInfo->m_strPropHelp = "����Բ��";
#endif
			pSonPropItem= InsertItem(pPropItem ,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			//if(m_bDisplayPointSonItem)
			{
				pSonPropItem->m_bHideChildren = TRUE;
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="X coordinate of "+sPropName;
#else 
				lpInfo->m_strPropHelp=sPropName+"X����";
#endif
				CPropTreeItem* pLeafItem = InsertItem(pSonPropItem,lpInfo,-1,TRUE);
				pLeafItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center.x");
				if(GetPropValueStr(pLeafItem->m_idProp,valueStr))
					pLeafItem->m_lpNodeInfo->m_strPropValue=valueStr;
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Y coordinate of "+sPropName;
#else 
				lpInfo->m_strPropHelp=sPropName+"Y����";
#endif
				pLeafItem = InsertItem(pSonPropItem,lpInfo,-1,TRUE);
				pLeafItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center.y");
				if(GetPropValueStr(pLeafItem->m_idProp,valueStr))
					pLeafItem->m_lpNodeInfo->m_strPropValue=valueStr;
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp="Z coordinate of "+sPropName;
#else 
				lpInfo->m_strPropHelp=sPropName+"Z����";
#endif
				pLeafItem = InsertItem(pSonPropItem,lpInfo,-1,TRUE);
				pLeafItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_center.z");
				if(GetPropValueStr(pLeafItem->m_idProp,valueStr))
					pLeafItem->m_lpNodeInfo->m_strPropValue=valueStr;
			}
		}
		//���淨��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_BUTTON;
		lpInfo->m_buttonType=BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Section Normal";
		lpInfo->m_strPropHelp="Section normal:it means vertical section of cylinder when it's null";
#else 
		lpInfo->m_strPropName="���淨��";
		lpInfo->m_strPropHelp="���淨�ߣ�Ϊ��ʱ��ʾΪ����Ĵ�ֱ����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_norm");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
		pPropItem->m_bHideChildren = FALSE;
		//if(m_bDisplayPointSonItem)
		{
			pPropItem->m_bHideChildren = TRUE;
			//
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Specify";
			lpInfo->m_strPropHelp = "Specify vector";
#else 
			lpInfo->m_strPropName = "ָ������";
			lpInfo->m_strPropHelp = "ָ������";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.sect_norm_std_style");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="X coordinate of section normal";
#else 
			lpInfo->m_strPropHelp="���淨��X����";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_norm.x");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Y coordinate of section normal";
#else 
			lpInfo->m_strPropHelp="���淨��Y����";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_norm.y");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
			lpInfo->m_strPropName="Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp="Z coordinate of section normal";
#else 
			lpInfo->m_strPropHelp="���淨��Z����";
#endif
			pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.sect_norm.z");
			if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
				pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}
		//�����巽ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Specify Directly|1.Specify Datum Edge|";
		if(pLineTube)
			lpInfo->m_cmbItems+="2.Tube's Datum Edge As Datum|3.Tube's Weld Line As Datum|4.Horizontal ray vector";
		lpInfo->m_strPropName="Ray Vector";
		lpInfo->m_strPropHelp="Ray vector define type:0.Specify directly,1 to 3 input rotate angle (1.Specify Datum Edge,2.Tube's Datum Edge As Datum,3.Tube's Weld Line As Datum)";
#else 
		lpInfo->m_cmbItems="0.ֱ��ָ��|1.ָ����׼��|";
		if(pLineTube)
			lpInfo->m_cmbItems+="2.�Ի�׼����ת|3.�Ժ�������ת|4.ˮƽ����";
		lpInfo->m_strPropName="���߷���";
		lpInfo->m_strPropHelp="���߷���������:0.ֱ��ָ��,1~3������ת�Ƕ�(1.��ָ������ת,2.�Ի�׼��Ϊ��׼��ת,3.�Ժ�����Ϊ��׼��ת,4.�����ϵ�ˮƽ���߷���)";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_vec_style");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		if(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==0||m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==1)
		{	
			//if(m_bDisplayPointSonItem)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbItems = CStdVector::MakeVectorSetStr();
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Specify";
				lpInfo->m_strPropHelp = "Specify vector";
#else 
				lpInfo->m_strPropName = "ָ������";
				lpInfo->m_strPropHelp = "ָ������";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo, -1,TRUE);
				pSonPropItem->m_idProp =GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
					pSonPropItem->m_lpNodeInfo->m_strPropValue = valueStr;
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="X";
				if(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="X coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="X coordinate of datum edge vector";
#else 
					lpInfo->m_strPropHelp="���߷���X����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���X����";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_vec.x");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Y";
				if(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="Y coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="Y coordinate of datum edge vector";
#else 
					lpInfo->m_strPropHelp="���߷���Y����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���Y����";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_vec.y");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType=PIT_EDIT;
				lpInfo->m_strPropName="Z";
				if(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropHelp="Z coordinate of ray vector";
				else 
					lpInfo->m_strPropHelp="Z coordinate of datum edge vector";
#else 
					lpInfo->m_strPropHelp="���߷���Z����";
				else 
					lpInfo->m_strPropHelp="��׼�߷���Z����";
#endif
				pSonPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
				pSonPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_vec.z");
				if(GetPropValueStr(pSonPropItem->m_idProp,valueStr))
					pSonPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
				pSonPropItem->SetReadOnly(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style!=0);
			}
		}
		if(m_xPoint.des_para.COLUMN_INTERS.ray_vec_style!=0&&m_xPoint.des_para.COLUMN_INTERS.ray_vec_style!=4)
		{	//��ת�Ƕ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Rotate Angle";
			lpInfo->m_strPropHelp="Rotate angle relates to specify datum edge,unit is ��";
#else 
			lpInfo->m_strPropName="��ת�Ƕ�";
			lpInfo->m_strPropHelp="�����ָ����ʼ�ߵ���ת�Ƕ�,��λΪ��";
#endif
			pPropItem = InsertItem(pPropItem,lpInfo,-1,TRUE);
			pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_angle");
			if(GetPropValueStr(pPropItem->m_idProp,valueStr))
				pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		}

		//���߰뾶����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Ray-Cylinder intersection|1.Section center";
		lpInfo->m_strPropName="Ray radius criterion";
		lpInfo->m_strPropHelp="0.Based on intersection point of ray and cylinder��offset along ray vertor,1.Based on cylinder center��offset along ray vector";
#else 
		lpInfo->m_cmbItems="0.�����潻��|1.����Բ��";
		lpInfo->m_strPropName="���߰뾶����";
		lpInfo->m_strPropHelp="0.������������Ľ���Ϊ��׼�������߷���ƫ��,1.������Ϊ��׼�������߷���ƫ��";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.bSpecRo");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//ƫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
		if(m_xPoint.des_para.COLUMN_INTERS.bSpecRo)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName="Offset to ray tip";
			lpInfo->m_strPropHelp="The offset along ray vector from cylinder center";
#else 
			lpInfo->m_strPropName="ƫ����";
			lpInfo->m_strPropHelp="�����������潻��Ϊ��׼�������߷����ƫ����";
#endif
		}
		else 
		{
			if(m_xPoint.des_para.COLUMN_INTERS.bSpecRo)
			{	//ָ���������߰뾶
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Ray Radius";
				lpInfo->m_strPropHelp="The radius of ray tip, based on cylinder center";
#else 
				lpInfo->m_strPropName="���߰뾶";
				lpInfo->m_strPropHelp="������Ϊ��������߰뾶";
#endif
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName="Ray tip offset";
				lpInfo->m_strPropHelp="Based on cylinder center��the offset along ray vector";
#else 
				lpInfo->m_strPropName="���߷���ƫ����";
				lpInfo->m_strPropHelp="������Ϊ��׼�������߷����ƫ����";
#endif
			}
		}
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_offset_r");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		//����ƽ���ƶ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType=PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName="Ray Parallel Offset";
		lpInfo->m_strPropHelp="Ray parallel offset��Parallel lines moving perpendicular to the ray direction,along the positive direction side moves with right hand screwing is positive,and the another side is negative��";
#else 
		lpInfo->m_strPropName="����ƽ�ƾ���";
		lpInfo->m_strPropHelp="����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����";
#endif
		pPropItem = InsertItem(pParentItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = GetPropID("des_para.COLUMN_INTERS.ray_para_offset");
		if(GetPropValueStr(pPropItem->m_idProp,valueStr))
			pPropItem->m_lpNodeInfo->m_strPropValue=valueStr;
		pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
	}
}
BOOL CParaPointPropertyList::FinishSelectObjOper(long hPickObj,long idEventProp)
{
	if(idEventProp==GetPropID("des_para.LINEPARTNODE.hLinePart"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			m_xPoint.des_para.RODNODE.hRod=pLinePart->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(idEventProp==GetPropID("des_para.RODEND.hRod"))
	{
		CLDSLineAngle *pLineAngle=(CLDSLineAngle*)console.FromPartHandle(hPickObj,CLS_LINEANGLE);
		if(pLineAngle)
		{
			m_xPoint.des_para.RODEND.hRod=pLineAngle->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	} 
#endif
	else if(idEventProp==GetPropID("des_para.LINEPARTNODE.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(hPickObj);
		if(pNode)
		{
			m_xPoint.des_para.RODNODE.hNode=pNode->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==GetPropID("des_para.AXIS_INTERS.hDatum1"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			m_xPoint.des_para.AXIS_INTERS.hDatum1=pLinePart->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==GetPropID("des_para.AXIS_INTERS.hDatum2"))
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)console.FromPartHandle(hPickObj,CLS_LINEPART);
		if(pLinePart)
		{
			m_xPoint.des_para.AXIS_INTERS.hDatum2=pLinePart->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==GetPropID("des_para.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(hPickObj);
		if(pNode)
		{
			m_xPoint.des_para.hNode=pNode->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==GetPropID("des_para.hPart"))
	{
		CLDSPart *pPart=console.FromPartHandle(hPickObj);
		if(pPart)
		{
			m_xPoint.des_para.hPart=pPart->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==GetPropID("des_para.COLUMN_INTERS.hLineTube"))
	{
		CLDSLineTube *pLineTube=(CLDSLineTube*)console.FromPartHandle(hPickObj,CLS_LINETUBE);
		if(pLineTube)
		{
			m_xPoint.des_para.COLUMN_INTERS.hLineTube=pLineTube->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==GetPropID("des_para.COLUMN_INTERS.SectCenter.hNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(hPickObj);
		if(pNode)
		{
			if(m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode>=0)
				m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=pNode->handle;
			else
				m_xPoint.des_para.COLUMN_INTERS.SectCenterParam.hNode=-pNode->handle;
			m_xPoint.UpdatePos(console.GetActiveModel());
		}
	}
	else if(idEventProp==GetPropID("des_para.SCALE_NODE.hLinePart"))
	{
		CLDSLinePart*pPart =(CLDSLinePart*)console.FromPartHandle(hPickObj,CLS_LINEPART);
		if(pPart)
			m_xPoint.des_para.SCALE_NODE.hLinePart=pPart->handle;
		m_xPoint.UpdatePos(console.GetActiveModel());
	}
	else if(idEventProp==GetPropID("des_para.SCALE_NODE.hStartNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(hPickObj);
		if(pNode)
			m_xPoint.des_para.SCALE_NODE.hStartNode=pNode->handle;
		m_xPoint.UpdatePos(console.GetActiveModel());
	}
	else if	(idEventProp==GetPropID("des_para.SCALE_NODE.hEndNode"))
	{
		CLDSNode *pNode=console.FromNodeHandle(hPickObj);
		if(pNode)
			m_xPoint.des_para.SCALE_NODE.hEndNode=pNode->handle;
		m_xPoint.UpdatePos(console.GetActiveModel());
	}
	else
		return FALSE;
	return TRUE;
}
