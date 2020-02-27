//<LOCALE_TRANSLATE BY hxr /> 2014-12-10
#include "stdafx.h"
#include "Tower.h"
#include "stdio.h"
#include "PartLib.h"
#include "CreateFace.h"
#include "BiteCut.h"
#include "LogFile.h"
#include "Expression.h"
#include "SortFunc.h"
#include "LifeObj.h"
#include "btc.h"
#include "LabelProp.h"
#include "NewAngleLocaModeMacroDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
//�Ǹ������λ����Ʋ���
CJgBerPosPara::CJgBerPosPara()
{
	//memset(this,0,sizeof(CJgBerPosPara)); �˴�����ʹ��memset,����datum_point��������ʹ�� wht 10-11-30
	m_iPosParaType=0;	//�Ǹ�
	jgber_cal_style=datum_pos_style=datum_to_ber_style=prj_type=0;
	iStartJgBerStyle=iEndJgBerStyle=offset_X_dist_style=offset_Y_dist_style=0;
	work_wing_auto0_x1_y2_both3=0;
	huoqu_pos_auto_0_x_wing1_y_wing2=0;
	fEccentricDist=0;
	len_offset_dist=datum_offset_dist=offset_X_dist=offset_Y_dist=huoqu_dist=offset_zhun=
		fStartJgLengOffsetX=fStartJgLengOffsetY=fEndJgLengOffsetX=fEndJgLengOffsetY=0;
	datum_jg_h=start_jg_h=end_jg_h=huoqu_jg_h=0;
	datum_point;			//��׼����
	face_offset_norm;		//ƽ���淨��
	bByOtherEndOffset=FALSE;//������һ�˽���ƽ�Ƽ���
	cFaceOffsetDatumLine=0;
}
BOOL CJgBerPosPara::IsInDatumJgBer()		//�����Ƿ��ڻ�׼�Ǹ�������
{
	if(datum_pos_style==1||datum_pos_style==4||datum_pos_style==7||datum_pos_style==10)
		return TRUE;
	else
		return FALSE;
}
BOOL CJgBerPosPara::IsInDatumJgWingX()
{
	if(datum_pos_style==2||datum_pos_style==5||datum_pos_style==8||
		datum_pos_style==11||datum_pos_style==13)
		return TRUE;
	else
		return FALSE;
}
BOOL CJgBerPosPara::IsInDatumJgWingY()
{
	if(datum_pos_style==3||datum_pos_style==6||datum_pos_style==9||
		datum_pos_style==12||datum_pos_style==14)
		return TRUE;
	else
		return FALSE;
}
BOOL CJgBerPosPara::IsInHuoQuLine()		//�����Ƿ��ڻ�������
{
	if(datum_pos_style>=7&&datum_pos_style<=12)
		return TRUE;
	else
		return FALSE;
}
BOOL CJgBerPosPara::IsInOffsetZhunLine()	//�����Ƿ���ƫ��׼����
{
	if(datum_pos_style==13||datum_pos_style==14)
		return TRUE;
	else
		return FALSE;
}
int CJgBerPosPara::GetDatumType()	//������ת��Ϊ��һ����
{
	if(datum_pos_style==0)
		return 0;	//�ڵ㼴����
	else if(datum_pos_style<=3)
		return 1;	//ͶӰ
	else if(datum_pos_style<=6)
		return 2;	//ƽ��
	else if(datum_pos_style<=9)
		return 3;	//ͶӰ����
	else if(datum_pos_style<=12)
		return 4;	//ƽ�ƻ���
	else if(datum_pos_style<=14)
		return 5;	//ƫ��׼��
	else //if(datum_pos_style==15)
		return 6;	//ָ������
}
BOOL CJgBerPosPara::IsFaceOffset()	//�Ƿ�Ϊƽ��	wht 10-07-30
{
	if(jgber_cal_style==2||
		(jgber_cal_style!=1&&(
		(datum_pos_style>3&&datum_pos_style<=6)||
		(datum_pos_style>9&&datum_pos_style<=12)||
		(datum_pos_style==0&&datum_to_ber_style==1)
		)))
		return TRUE;
	else
		return FALSE;
}
BOOL CJgBerPosPara::ChangeToDatumType(int prj1_faceoffset2_huoqu3and4_offsetzhun5)	//������ת��Ϊ��һ����
{
	if(datum_pos_style==0||datum_pos_style==15)
		return FALSE;
	switch(prj1_faceoffset2_huoqu3and4_offsetzhun5)
	{
	case 0:
		datum_pos_style=0;
		return TRUE;
	case 1:	//ת��Ϊ��ͨͶӰ
		if(datum_pos_style==0)
			datum_to_ber_style=0;
		else if(datum_pos_style>=1&&datum_pos_style<=3)
			return TRUE;
		else if(datum_pos_style>=4&&datum_pos_style<=6)
		{
			datum_pos_style-=3;
			return TRUE;
		}
		else if(datum_pos_style>=7&&datum_pos_style<=9)
		{
			datum_pos_style-=6;
			return TRUE;
		}
		else if(datum_pos_style>=10&&datum_pos_style<=12)
		{
			datum_pos_style-=9;
			return TRUE;
		}
		else if(datum_pos_style>=13&&datum_pos_style<=14)
		{
			datum_pos_style-=11;
			return TRUE;
		}
		else
			return FALSE;
	case 2:	//ת��Ϊ��ͨƽ��
		if(datum_pos_style==0)
			datum_to_ber_style=1;
		else if(datum_pos_style>=1&&datum_pos_style<=3)
		{
			datum_pos_style+=3;
			return TRUE;
		}
		else if(datum_pos_style>=4&&datum_pos_style<=6)
			return TRUE;
		else if(datum_pos_style>=7&&datum_pos_style<=9)
		{
			datum_pos_style-=3;
			return TRUE;
		}
		else if(datum_pos_style>=10&&datum_pos_style<=12)
		{
			datum_pos_style-=6;
			return TRUE;
		}
		else if(datum_pos_style>=13&&datum_pos_style<=14)
		{
			datum_pos_style-=8;
			return TRUE;
		}
		else
			return FALSE;
	case 3:	//ת��ΪͶӰ������
		if(datum_pos_style>=1&&datum_pos_style<=3)
		{
			datum_pos_style+=6;
			return TRUE;
		}
		else if(datum_pos_style>=4&&datum_pos_style<=6)
		{
			datum_pos_style+=3;
			return TRUE;
		}
		else if(datum_pos_style>=7&&datum_pos_style<=9)
			return TRUE;
		else if(datum_pos_style>=10&&datum_pos_style<=12)
		{
			datum_pos_style-=3;
			return TRUE;
		}
		else if(datum_pos_style>=13&&datum_pos_style<=14)
		{
			datum_pos_style-=5;
			return TRUE;
		}
		else
			return FALSE;
	case 4:	//ת��Ϊƽ�ƻ�����
		if(datum_pos_style>=1&&datum_pos_style<=3)
		{
			datum_pos_style+=9;
			return TRUE;
		}
		else if(datum_pos_style>=4&&datum_pos_style<=6)
		{
			datum_pos_style+=6;
			return TRUE;
		}
		else if(datum_pos_style>=7&&datum_pos_style<=9)
		{
			datum_pos_style+=3;
			return TRUE;
		}
		else if(datum_pos_style>=10&&datum_pos_style<=12)
			return TRUE;
		else if(datum_pos_style>=13&&datum_pos_style<=14)
		{
			datum_pos_style-=2;
			return TRUE;
		}
		else
			return FALSE;
	case 5:	//ת��Ϊƫ��׼��
		if(datum_pos_style>=2&&datum_pos_style<=3)
		{
			datum_pos_style+=11;
			return TRUE;
		}
		else if(datum_pos_style>=5&&datum_pos_style<=6)
		{
			datum_pos_style+=8;
			return TRUE;
		}
		else if(datum_pos_style>=8&&datum_pos_style<=9)
		{
			datum_pos_style+=5;
			return TRUE;
		}
		else if(datum_pos_style>=11&&datum_pos_style<=12)
		{
			datum_pos_style+=2;
			return TRUE;
		}
		else if(datum_pos_style>=13&&datum_pos_style<=14)
			return TRUE;
		else
			return FALSE;
	case 6:	//ת��Ϊָ������
		datum_pos_style=15;
		return TRUE;
	default:
		return FALSE;
	}
}

bool CJgBerPosPara::ReplaceReferenceHandle(CHandleReplacer& replacer)
{
	bool replaced=false;
	replaced|=replacer.VerifyAndReplace(&datum_jg_h);
	replaced|=replacer.VerifyAndReplace(&start_jg_h);
	replaced|=replacer.VerifyAndReplace(&end_jg_h);
	replaced|=replacer.VerifyAndReplace(&huoqu_jg_h);
	return replaced;
}
void CJgBerPosPara::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadInteger(&jgber_cal_style);		//�Ǹ������㷽ʽ
	buffer.ReadInteger(&datum_pos_style);		//������㷽ʽ
	buffer.ReadInteger(&datum_to_ber_style);	//���ݻ���������ķ�ʽ
	datum_point.FromBuffer(buffer,version,doc_type);
	buffer.ReadWord(&work_wing_auto0_x1_y2_both3);
	buffer.ReadInteger(&prj_type);
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000022)||	//TMA
		(doc_type==2&&version>=1000077)||	//LMA
		(doc_type==4&&version>=1000002))	//LDS
		buffer.ReadDouble(&len_offset_dist);
	buffer.ReadDouble(&datum_offset_dist);
	buffer.ReadDouble(&offset_X_dist);
	buffer.ReadInteger(&offset_X_dist_style);
	buffer.ReadDouble(&offset_Y_dist);
	buffer.ReadInteger(&offset_Y_dist_style);
	buffer.ReadInteger(&datum_jg_h);
	buffer.ReadInteger(&start_jg_h);
	buffer.ReadInteger(&end_jg_h);
	buffer.ReadInteger(&iStartJgBerStyle);
	buffer.ReadInteger(&iEndJgBerStyle);
	buffer.ReadInteger(&huoqu_jg_h);			// ������׼�Ǹ־��
	buffer.ReadByte(&cFaceOffsetDatumLine);
	buffer.ReadDouble(&face_offset_norm.x);	// ƽ���淨��
	buffer.ReadDouble(&face_offset_norm.y);	// ƽ���淨��
	buffer.ReadDouble(&face_offset_norm.z);	// ƽ���淨��
	buffer.ReadDouble(&huoqu_dist);			//�����߼�϶
	buffer.ReadDouble(&offset_zhun);			//ƫ��׼�ߵ�ƫ����
	buffer.ReadInteger(&bByOtherEndOffset);	//������һ�˽���ƽ�Ƽ���
	buffer.ReadDouble(&fStartJgLengOffsetX);
	buffer.ReadDouble(&fStartJgLengOffsetY);
	buffer.ReadDouble(&fEndJgLengOffsetX);
	buffer.ReadDouble(&fEndJgLengOffsetY);
	if( version==0||doc_type==4||doc_type==5||
		(doc_type==1&&version>=4000020)||	//TMA
		(doc_type==2&&version>=1000075))	//LMA
		buffer.ReadDouble(&fEccentricDist);
}
void CJgBerPosPara::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(jgber_cal_style);		//�Ǹ������㷽ʽ
	buffer.WriteInteger(datum_pos_style);		//������㷽ʽ
	buffer.WriteInteger(datum_to_ber_style);	//���ݻ���������ķ�ʽ
	datum_point.ToBuffer(buffer,version,doc_type);
	buffer.WriteWord(work_wing_auto0_x1_y2_both3);
	buffer.WriteInteger(prj_type);
	buffer.WriteDouble(len_offset_dist);
	buffer.WriteDouble(datum_offset_dist);
	buffer.WriteDouble(offset_X_dist);
	buffer.WriteInteger(offset_X_dist_style);
	buffer.WriteDouble(offset_Y_dist);
	buffer.WriteInteger(offset_Y_dist_style);
	buffer.WriteInteger(datum_jg_h);
	buffer.WriteInteger(start_jg_h);
	buffer.WriteInteger(end_jg_h);
	buffer.WriteInteger(iStartJgBerStyle);
	buffer.WriteInteger(iEndJgBerStyle);
	buffer.WriteInteger(huoqu_jg_h);		// ������׼�Ǹ־��
	buffer.WriteByte(cFaceOffsetDatumLine);
	buffer.WriteDouble(face_offset_norm.x);	// ƽ���淨��
	buffer.WriteDouble(face_offset_norm.y);	// ƽ���淨��
	buffer.WriteDouble(face_offset_norm.z);	// ƽ���淨��
	buffer.WriteDouble(huoqu_dist);			//�����߼�϶
	buffer.WriteDouble(offset_zhun);		//ƫ��׼�ߵ�ƫ����
	buffer.WriteInteger(bByOtherEndOffset);	//������һ�˽���ƽ�Ƽ���
	buffer.WriteDouble(fStartJgLengOffsetX);
	buffer.WriteDouble(fStartJgLengOffsetY);
	buffer.WriteDouble(fEndJgLengOffsetX);
	buffer.WriteDouble(fEndJgLengOffsetY);
	buffer.WriteDouble(fEccentricDist);
}

void CJgBerPosPara::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<�Ǹ����λ�ò���>\n");
	if (jgber_cal_style==0)
		fprintf(fp,"<�Ǹ������㷽ʽ jgber_cal_style=\"0.��ʾ����ƫ��\"/>\n");
	else if (jgber_cal_style==1)
		fprintf(fp,"<�Ǹ������㷽ʽ jgber_cal_style=\"1.��ʾ������ͶӰ\"/>\n");
	else if (jgber_cal_style==2)
		fprintf(fp,"<�Ǹ������㷽ʽ jgber_cal_style=\"2.��ʾ������ƽ��\"/>\n");
	else if (jgber_cal_style==3)
		fprintf(fp,"<�Ǹ������㷽ʽ jgber_cal_style=\"3.��ʾֱ��ָ������\"/>\n");
	if (datum_pos_style==0)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"0.�ڵ㼴����\"/>\n");
	else if (datum_pos_style==1)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"1.������ͶӰ\"/>\n");
	else if (datum_pos_style==2)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"2.X֫����ͶӰ\"/>\n");
	else if (datum_pos_style==3)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"3.Y֫����ͶӰ\"/>\n");
	else if (datum_pos_style==4)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"4.������ƽ��\"/>\n");
	else if (datum_pos_style==5)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"5.X֫����ƽ��\"/>\n");
	else if (datum_pos_style==6)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"6.Y֫����ƽ��\"/>\n");
	else if (datum_pos_style==7)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"7.ͶӰ���߻�����\"/>\n");
	else if (datum_pos_style==8)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"8.ͶӰX֫���߻�����\"/>\n");
	else if (datum_pos_style==9)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"9.ͶӰY֫���߻�����\"/>\n");
	else if (datum_pos_style==10)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"10.ƽ�����߻�����\"/>\n");
	else if (datum_pos_style==11)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"11.ƽ��X֫���߻�����\"/>\n");
	else if (datum_pos_style==12)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"12.ƽ��Y֫���߻�����\"/>\n");
	else if (datum_pos_style==13)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"13.X֫ƫ��׼��\"/>\n");
	else if (datum_pos_style==14)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"14.Y֫ƫ��׼��\"/>\n");
	else if (datum_pos_style==15)
		fprintf(fp,"<����ļ��㷽ʽ datum_pos_style=\"15.ֱ��ָ����������\"/>\n");
	if (datum_to_ber_style==0)
		fprintf(fp,"<���ݻ���������ķ�ʽ datum_to_ber_style=\"0.��ʾͶӰ\"/>\n");
	else if (datum_to_ber_style==1)
		fprintf(fp,"<���ݻ���������ķ�ʽ datum_to_ber_style=\"1.��ʾƽ��\"/>\n");
	datum_point.ToXmlFile(fp,schema);
	fprintf(fp,"<ֱ��ָ������ʱ���Ի���ƫ��ʱ�ĵ�ǰ����֫ work_wing_auto0_x1_y2_both3=\"%d\"/>\n",work_wing_auto0_x1_y2_both3);
	if (prj_type==0)
		fprintf(fp,"<ͶӰ��ʽ prj_type=\"0.��ֱ��֫����ͶӰ\"/>\n");
	else if (prj_type==1)
		fprintf(fp,"<ͶӰ��ʽ prj_type=\"1.ƽ����֫����ͶӰ\"/>\n");
	sprintf(sText,"%f",len_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ�� len_offset_dist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",datum_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<��ӵ��Ի�׼֫�ط��ߵ�ƫ�� datum_offset_dist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",offset_X_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<�Զ���ƫ����ʱX֫�Ի�׼���ƫ���� offset_X_dist=\"%s\"/>\n",sText);
	if (offset_X_dist_style==0)
		fprintf(fp,"<�Ǹ�X֫ƫ�Ʒ�ʽ offset_X_dist_style=\"0.��׼׼��\"/>\n");
	else if (offset_X_dist_style==1)
		fprintf(fp,"<�Ǹ�X֫ƫ�Ʒ�ʽ offset_X_dist_style=\"1.g1\"/>\n");
	else if (offset_X_dist_style==2)
		fprintf(fp,"<�Ǹ�X֫ƫ�Ʒ�ʽ offset_X_dist_style=\"2.g2\"/>\n");
	else if (offset_X_dist_style==3)
		fprintf(fp,"<�Ǹ�X֫ƫ�Ʒ�ʽ offset_X_dist_style=\"3.g3\"/>\n");
	else if (offset_X_dist_style==4)
		fprintf(fp,"<�Ǹ�X֫ƫ�Ʒ�ʽ offset_X_dist_style=\"4.�Զ���\"/>\n");
	sprintf(sText,"%f",offset_Y_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<�Զ���ƫ����ʱY֫�Ի�׼���ƫ���� offset_Y_dist=\"%s\"/>\n",sText);
	if (offset_Y_dist_style==0)
		fprintf(fp,"<�Ǹ�Y֫ƫ�Ʒ�ʽ offset_Y_dist_style=\"0.��׼׼��\"/>\n");
	else if (offset_Y_dist_style==1)
		fprintf(fp,"<�Ǹ�Y֫ƫ�Ʒ�ʽ offset_Y_dist_style=\"1.g1\"/>\n");
	else if (offset_Y_dist_style==2)
		fprintf(fp,"<�Ǹ�Y֫ƫ�Ʒ�ʽ offset_Y_dist_style=\"2.g2\"/>\n");
	else if (offset_Y_dist_style==3)
		fprintf(fp,"<�Ǹ�Y֫ƫ�Ʒ�ʽ offset_Y_dist_style=\"3.g3\"/>\n");
	else if (offset_Y_dist_style==4)
		fprintf(fp,"<�Ǹ�Y֫ƫ�Ʒ�ʽ offset_Y_dist_style=\"4.�Զ���\"/>\n");
	fprintf(fp,"<���λ�û�׼�Ǹ� datum_jg_h=\"0X%X\"/>\n",datum_jg_h);
	fprintf(fp,"<��ʼ�Ǹ־�� start_jg_h=\"0X%X\"/>\n",start_jg_h);
	fprintf(fp,"<��ֹ�Ǹ־�� end_jg_h=\"0X%X\"/>\n",end_jg_h);
	if (iStartJgBerStyle==0)
		fprintf(fp,"<ʼ��ָ����׼�ǸֵĻ�׼�˵� iStartJgBerStyle=\"0.ʼ���\"/>\n");
	else if (iStartJgBerStyle==1)
		fprintf(fp,"<ʼ��ָ����׼�ǸֵĻ�׼�˵� iStartJgBerStyle=\"1.�����\"/>\n");
	if (iEndJgBerStyle==0)
		fprintf(fp,"<�ն�ָ����׼�ǸֵĻ�׼�˵� iEndJgBerStyle=\"0.ʼ���\"/>\n");
	else if (iEndJgBerStyle==1)
		fprintf(fp,"<�ն�ָ����׼�ǸֵĻ�׼�˵� iEndJgBerStyle=\"1.�����\"/>\n");
	fprintf(fp,"<������׼�Ǹ־�� huoqu_jg_h=\"0X%X\"/>\n",huoqu_jg_h);
	fprintf(fp,"<ƽ�����׼�� cFaceOffsetDatumLine=\"%d\"/>\n",cFaceOffsetDatumLine);
	sprintf(sText,"%f",face_offset_norm.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",face_offset_norm.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",face_offset_norm.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<ƽ���淨�� face_offset_norm=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",huoqu_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<�����߼�϶ huoqu_dist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",offset_zhun);
	SimplifiedNumString(sText);
	fprintf(fp,"<ƫ��׼�ߵ�ƫ���� offset_zhun=\"%s\"/>\n",sText);
	if (bByOtherEndOffset)
		fprintf(fp,"<������һ�˽���ƽ�Ƽ��� bByOtherEndOffset=\"true\"/>\n");
	else
		fprintf(fp,"<������һ�˽���ƽ�Ƽ��� bByOtherEndOffset=\"false\"/>\n");
	sprintf(sText,"%f",fStartJgLengOffsetX);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ͶӰʱ��ʼ�Ǹ�X֫��׼��㷨��ƫ���� fStartJgLengOffsetX=\"%s\"/>\n",sText);
	sprintf(sText,"%f",fStartJgLengOffsetY);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ͶӰʱ��ʼ�Ǹ�Y֫��׼��㷨��ƫ���� fStartJgLengOffsetY=\"%s\"/>\n",sText);
	sprintf(sText,"%f",fEndJgLengOffsetX);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ͶӰʱ��ֹ�Ǹ�X֫��׼��㷨��ƫ���� fEndJgLengOffsetX=\"%s\"/>\n",sText);
	sprintf(sText,"%f",fEndJgLengOffsetY);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ͶӰʱ��ֹ�Ǹ�Y֫��׼��㷨��ƫ���� fEndJgLengOffsetY=\"%s\"/>\n",sText);
	sprintf(sText,"%f",fEccentricDist);
	SimplifiedNumString(sText);
	fprintf(fp,"<ƫ�ľ� fEccentricDist=\"%s\"/>\n",sText);
	fprintf(fp,"</�Ǹ����λ�ò���>\n");
}

void WING_OFFSET_STYLE::SetOffset(double offset_dist/*=0*/,BYTE style/*=4*/,BYTE oper_style/*=0*/)
{
	offsetDist=offset_dist;
	gStyle=style;
	operStyle=oper_style;
}
void WING_OFFSET_STYLE::FromBuffer(CBuffer &buffer)
{
	buffer.ReadByte(&gStyle);		
	buffer.ReadByte(&operStyle);		
	buffer.ReadDouble(&offsetDist);	
}
void WING_OFFSET_STYLE::ToBuffer(CBuffer &buffer)
{
	buffer.WriteByte(gStyle);		//�Ǹ�֫ƫ����:0.��׼׼��;1:g1;2:g2;3:g3;4:�Զ���
	buffer.WriteByte(operStyle);	//��������0.����ƫ��;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	buffer.WriteDouble(offsetDist);	//����gStyle=4ʱ��Ч
}

void WING_OFFSET_STYLE::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<֫ƫ�Ʒ�ʽ>\n");
	if (gStyle==0)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ gStyle=\"0.��׼׼��\"/>\n");
	else if (gStyle==1)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ gStyle=\"1.g1\"/>\n");
	else if (gStyle==2)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ gStyle=\"2.g2\"/>\n");
	else if (gStyle==3)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ gStyle=\"3.g3\"/>\n");
	else if (gStyle==4)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ gStyle=\"4.�Զ���\"/>\n");
	if (operStyle==0)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ operStyle=\"0.����ƫ��\"/>\n");
	else if (operStyle==1)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ operStyle=\"1.X-Yƽ��ƽ��\"/>\n");
	else if (operStyle==2)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ operStyle=\"2.Y-Zƽ��ƽ��\"/>\n");
	else if (operStyle==3)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ operStyle=\"3.X-Zƽ��ƽ��\"/>\n");
	else if (operStyle==4)
		fprintf(fp,"<�Ǹ�֫ƫ�Ʒ�ʽ operStyle=\"4.�Զ���ƽ����\"/>\n");
	sprintf(sText,"%f",offsetDist);
	SimplifiedNumString(sText);
	fprintf(fp,"<ƫ���� offsetDist=\"%s\"/>\n",sText);
	fprintf(fp,"</֫ƫ�Ʒ�ʽ>\n");
}

IMPLEMENT_GET(CMultiOffsetPos,ciDestLineMode){
	BYTE ciType = (cbSimpleUdfFlag&0x0c)>>2;
	if(ciType==1&&!IsInDatumJgBer())	//ͬʱ���������߻�֫����
		ciType+=1;
	else if(ciType>1)
		ciType+=1;
	return ciType;
}
IMPLEMENT_SET(CMultiOffsetPos,ciDestLineMode){
	cbSimpleUdfFlag&=0xF3;		//1111 0011 ��ʼ����3~4λΪ0
	if(TO_BERLINE==value)	//��������
	{
		cbSimpleUdfFlag|=0x04;	//0000 0100
		if(spatialOperationStyle==2||spatialOperationStyle==3)
			spatialOperationStyle=1;
		else if(spatialOperationStyle==5||spatialOperationStyle==6)
			spatialOperationStyle=4;
		else if(spatialOperationStyle==8||spatialOperationStyle==9)
			spatialOperationStyle=7;
		else if(spatialOperationStyle==11||spatialOperationStyle==12)
			spatialOperationStyle=10;
		else
			CLDSDbObject::Log2File()->Log("��ǰ�Ǹ���㶨λ��ʽ��Ϊ�������ߺ��޷�ȷ����λ��ʽ");
	}
	else if(TO_AXISLINE==value)
	{
		cbSimpleUdfFlag|=0x04;	//0000 0100
		BYTE cLappedWing=cDatumAngleWing;
		//if(bUdfLappedDatumWing&&(cDatumAngleWing=='X'||cDatumAngleWing=='Y'))
		//	cLappedWing=cDatumAngleWing;

		if(spatialOperationStyle==1&&cLappedWing=='X')
			spatialOperationStyle=2;
		else if(spatialOperationStyle==1&&cLappedWing=='Y')
			spatialOperationStyle=3;
		else if(spatialOperationStyle==4&&cLappedWing=='X')
			spatialOperationStyle=5;
		else if(spatialOperationStyle==4&&cLappedWing=='Y')
			spatialOperationStyle=6;
		else if(spatialOperationStyle==7&&cLappedWing=='X')
			spatialOperationStyle=8;
		else if(spatialOperationStyle==7&&cLappedWing=='Y')
			spatialOperationStyle=9;
		else if(spatialOperationStyle==10&&cLappedWing=='X')
			spatialOperationStyle=11;
		else if(spatialOperationStyle==10&&cLappedWing=='Y')
			spatialOperationStyle=12;
		//TODO:else δ����
	}
	else if(TO_BENDLINE==value)
		cbSimpleUdfFlag|=0x08;	//0000 1000
	else if(TO_OFFSETZHUN==value)
		cbSimpleUdfFlag|=0x0C;	//0000 1100
}
IMPLEMENT_GET(CMultiOffsetPos,bUdfLappedDatumWing){return (cbSimpleUdfFlag&0x02)>>1;}
IMPLEMENT_SET(CMultiOffsetPos,bUdfLappedDatumWing){
	if(value)
	{	//�趨ָ���������֫ʱ��һ��Ҫ�뵱ǰspatialOperationStyleָ���Ĵ������֫һ��
		if(IsInDatumJgBer())
			cDatumAngleWing=0;
		else if(IsInDatumJgWingX())
			cDatumAngleWing='X';
		else if(IsInDatumJgWingY())
			cDatumAngleWing='Y';
		cbSimpleUdfFlag|=0x02;	//0000 0010
	}
	else
		cbSimpleUdfFlag&=0xFD;	//1111 1101
}
CMultiOffsetPos::CMultiOffsetPos()
{
	ClearContents();
}
void CMultiOffsetPos::ClearContents()	//�������Բ���Ĭ��ֵ
{	//���ڴ˶����ں���datumLineStart,datumLineEnd�Ȼ�׼�����(�ں��麯��),����ֱ����memset��������
	m_iPosParaType=3;	//�Ǹ��¶�λ����ģʽ
	jgber_cal_style=0;	//0.��������λ��1.ֱ��ָ���������
	//A. ��ʼλ�ö�λ��ʽ
	datum_jg_h=0;			//���λ�û�׼�Ǹ�
	origin_pos_style=0;	//0.�ڵ�λ��;1.��׼��;2.��׼�Ǹֹؼ���;3.��һ�˽ڵ�����;4.��һ���������
	cDatumAngleWing=0;	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���(����origin_pos_style=2ʱ��Ч)
	datumWingOffset.offsetDist=datumWingOffset.gStyle=datumWingOffset.operStyle=0;;
	//B. �ռ临���������(��ʼ��->����Ҳ�ɳƻ��㶨λ)����: 0.ָ������
	// 1:������ͶӰ      2:X֫����ͶӰ       3:Y֫����ͶӰ
	// 4:������ƽ��      5:X֫����ƽ��       6:Y֫����ƽ��
	// 7:ͶӰ���߻�����  8:ͶӰX֫���߻����� 9:ͶӰY֫���߻�����
	//10:ƽ�����߻����� 11:ƽ��X֫���߻�����12:ƽ��Y֫���߻�����
	//13:X֫ƫ��׼��    14:Y֫ƫ��׼��      15:��֫�������
	//16:ָ����ͶӰ		17:ָ����ƽ��		18:������Ƶ�ͶӰ 19:������Ƶ�ƽ��
	spatialOperationStyle=18;
	huoqu_pos_auto_0_x_wing1_y_wing2=0;	//������λ�õ��ж�
	len_offset_dist=0;		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	len_offset_type=0;		//�ش�ӻ�׼�˼��������췽��
	datum_offset_dist=0;	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	cFaceOffsetDatumLine=0;	//0:����;'X':X֫����;'Y':Y֫���ߣ���spatialOperationStyle==17ʱ��Ч
	face_offset_norm.Set(0,0,1);	//Ĭ��ΪX-Yƽ��
	huoqu_jg_h=0;		//������׼�Ǹ־��
	huoqu_dist=0;		//�����߼�϶
	offset_zhun=0;		//ƫ��׼�ߵ�ƫ����
	fEccentricDist=0;	//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
	cbSimpleUdfFlag=0;	//Ĭ�ϲ�ָ����֫��������,���������ж�
	coordCtrlType=0;	//0:X-Z����;1:Y-Z����;2.X-Y����(����spatialOperationStyle=15ʱ��Ч)
	cMainCoordCtrlWing='X';//�Ǹ�����֫��'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	ctrlWing.offsetDist=ctrlWing.gStyle=ctrlWing.operStyle=0;	//�Ǹ���Ҫ����֫(�����������������֫)
	otherWing.offsetDist=otherWing.gStyle=otherWing.operStyle=0;//�Ǹָ�������֫(����һ�����������֫)
	//C. ƫ�Ʋ���(����->����ʸ��ƫ��)���������ڴ�Ӹ˼���(��)���λ�ü���
	cWorkWingStyle=0;//�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫0:�Զ��ж�;1:X֫;2:Y֫;3:˫֫;�����Զ��ж�
	wing_x_offset.offsetDist=wing_x_offset.operStyle=wing_x_offset.gStyle=0;
	wing_y_offset.offsetDist=wing_y_offset.operStyle=wing_y_offset.gStyle=0;	//��ʱ����WING_OFFSET_STYLE::operStyle����
	//
	hDatumLine=0;	//��׼�߾�� ����0ʱ��ʾ����ʼ�ն˻�׼��
}
WING_OFFSET_STYLE& CMultiOffsetPos::get_xMasterWing()
{
#ifdef __NEW_ANGLE_LOCA_MODE_
	if (spatialOperationStyle == 19&&this->cFaceOffsetDatumLine == 'X')
		return wing_x_offset;
	else if (spatialOperationStyle == 19&&this->cFaceOffsetDatumLine == 'Y')
		return wing_y_offset;
	else if(spatialOperationStyle == 18||spatialOperationStyle == 19)
		return wing_x_offset;
#endif
	return ctrlWing;
}
WING_OFFSET_STYLE& CMultiOffsetPos::set_xMasterWing(WING_OFFSET_STYLE xOffsetStyle)
{
#ifdef __NEW_ANGLE_LOCA_MODE_
	if (spatialOperationStyle == 19&&this->cFaceOffsetDatumLine == 'X')
		return wing_x_offset = xOffsetStyle;
	else if (spatialOperationStyle == 19&&this->cFaceOffsetDatumLine == 'Y')
		return wing_y_offset = xOffsetStyle;
	else if(spatialOperationStyle == 18||spatialOperationStyle == 19)
		return wing_x_offset = xOffsetStyle;
#endif
	return ctrlWing = xOffsetStyle;
}
WING_OFFSET_STYLE& CMultiOffsetPos::get_xSlaveWing()
{
#ifdef __NEW_ANGLE_LOCA_MODE_
	if (spatialOperationStyle == 19&&this->cFaceOffsetDatumLine == 'X')
		return wing_y_offset;
	else if (spatialOperationStyle == 19&&this->cFaceOffsetDatumLine == 'Y')
		return wing_x_offset;
	else if(spatialOperationStyle == 18||spatialOperationStyle == 19)
		return wing_y_offset;
#endif
	return otherWing;
}
WING_OFFSET_STYLE& CMultiOffsetPos::set_xSlaveWing(WING_OFFSET_STYLE xOffsetStyle)
{
#ifdef __NEW_ANGLE_LOCA_MODE_
	if (spatialOperationStyle == 19&&this->cFaceOffsetDatumLine == 'X')
		return wing_y_offset = xOffsetStyle;
	else if (spatialOperationStyle == 19&&this->cFaceOffsetDatumLine == 'Y')
		return wing_x_offset = xOffsetStyle;
	else if(spatialOperationStyle == 18||spatialOperationStyle == 19)
		return wing_y_offset = xOffsetStyle;
#endif
	return otherWing = xOffsetStyle;
}
bool CMultiOffsetPos::get_blLegayDualFairWingCtrlPoint()
{	//������Ƶ�ͶӰ��������Ϊ��׼��������Ƶ�ƽ�� wjh-2020.1.1
	return spatialOperationStyle == 18||
		(spatialOperationStyle == 19&&cFaceOffsetDatumLine!='X'&&cFaceOffsetDatumLine!='Y');
}
int CMultiOffsetPos::GetSimpleDatumPoint()
{
	if(spatialOperationStyle==0)
		return 0;//0.ָ������
	else if(spatialOperationStyle==15||spatialOperationStyle==18||spatialOperationStyle==19)
		return 1;//1.���Ƶ�"
	else if(spatialOperationStyle==16||spatialOperationStyle==17)
		return 3;//3.���߶�λ
	else //if(spatialOperationStyle>0&&spatialOperationStyle<=14)
		return 2;//2.��Ӷ�λ
}
void CMultiOffsetPos::SetSimpleDatumPoint(int iSimpleDatumPointType)
{
	if(iSimpleDatumPointType==0)
		spatialOperationStyle=0;
	else if(iSimpleDatumPointType==1&&GetSimpleDatumPoint()!=1)
		spatialOperationStyle=15;		//��ʱ,֮�����ִ�е�λIntelliUpdateEndSimplePosParam�Ⱥ���ʱ����
	else if(iSimpleDatumPointType==3&&GetSimpleDatumPoint()!=3)
		spatialOperationStyle==IsFaceOffset()?17:16;//��ʱ,֮�����ִ�е�λIntelliUpdateEndSimplePosParam�Ⱥ���ʱ����
	else if(iSimpleDatumPointType==2&&GetSimpleDatumPoint()!=2)
		spatialOperationStyle=2;		//��ʱ,֮�����ִ�е�λIntelliUpdateEndSimplePosParam�Ⱥ���ʱ����
}
void CMultiOffsetPos::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadByte(&jgber_cal_style);
	buffer.ReadByte(&origin_pos_style);	//0.�ڵ�λ��;1.��׼��;2.��׼�Ǹֹؼ���;3.��һ�˽ڵ�����;4.��һ���������
	buffer.ReadInteger(&datum_jg_h);	//���λ�û�׼�Ǹ�
	//����origin_pos_style=2ʱ��Ч
	buffer.ReadByte(&cDatumAngleWing);	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	datumWingOffset.FromBuffer(buffer);
	if(origin_pos_style==1&&(
		(doc_type==1&&version==4010001)||	//TMA(V4.1.0.02)
		(doc_type==2&&version==1010001)||	//LMA(V1.1.0.02)
		(doc_type==4&&version==1010001)||	//LDS(V1.1.0.02)
		(doc_type==5&&version==1010001)))	//TDA(V1.1.0.02)
		datumPoint.FromBuffer(buffer,version,doc_type);
	//��һ��ƫ�Ʋ���(��ʼ��->����Ҳ�ɳƻ��㶨λ)����: 0.ָ����׼��
	// 1:������ͶӰ      2:X֫����ͶӰ       3:Y֫����ͶӰ
	// 4:������ƽ��      5:X֫����ƽ��       6:Y֫����ƽ��
	// 7:ͶӰ���߻�����  8:ͶӰX֫���߻����� 9:ͶӰY֫���߻�����
	//10:ƽ�����߻����� 11:ƽ��X֫���߻�����12:ƽ��Y֫���߻�����
	//13:X֫ƫ��׼��    14:Y֫ƫ��׼��      15:��֫�������
	//16:ָ����ͶӰ		17:ָ����ƽ��		18:������Ƶ�ͶӰ 19:������Ƶ�ƽ��
	buffer.ReadByte(&spatialOperationStyle);
	if( spatialOperationStyle==0&&(version==0||
		(doc_type==1&&version>=4010002)||	//TMA(V4.1.0.02)
		(doc_type==2&&version>=1010002)||	//LMA(V1.1.0.02)
		(doc_type==4&&version>=1010002)||	//LDS(V1.1.0.02)
		(doc_type==5&&version>=1010002)))	//TDA(V1.1.0.02)
		datumPoint.FromBuffer(buffer,version,doc_type);
	if( version==0||
		(doc_type==1&&version>=4010300)||	//TMA(V4,1,3,0)
		(doc_type==2&&version>=1010300)||	//LMA(V1,1,3,0)
		(doc_type==4&&version>=1030002)||	//LDS(V1,3,0,2)
		(doc_type==5&&version>=1030002))	//TDA(V1,3,0,2)
		buffer.ReadByte(&len_offset_type);		//len_offset_dist��ƫ������0.��׼�˼���������,1.���������ص�ǰ�˼���ֱ����ƫ��,'X','Y','Z'
	buffer.ReadDouble(&len_offset_dist);		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	buffer.ReadDouble(&datum_offset_dist);	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	buffer.ReadByte(&cFaceOffsetDatumLine);
	buffer.ReadDouble(&face_offset_norm.x);	//ƽ���淨��X����
	buffer.ReadDouble(&face_offset_norm.y);	//ƽ���淨��Y����
	buffer.ReadDouble(&face_offset_norm.z);	//ƽ���淨��Z����
	buffer.ReadInteger(&huoqu_jg_h);		//������׼�Ǹ־��
	buffer.ReadByte(&huoqu_pos_auto_0_x_wing1_y_wing2);
	buffer.ReadDouble(&huoqu_dist);		//�����߼�϶
	buffer.ReadDouble(&offset_zhun);		//ƫ��׼�ߵ�ƫ����
	//fEccentricDist��fLenOffsetOfCoordWing������ͬ�ڴ� wjh-2016.4.27
	buffer.ReadDouble(&fEccentricDist);	//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
	//����spatialOperationStyle=15ʱ��Ч
	if(spatialOperationStyle==15)
	{
		buffer.ReadByte(&coordCtrlType);	//0:X,Z����;1:Y,Z����;X,Y����
		buffer.ReadByte(&cMainCoordCtrlWing);//�Ǹ�����֫ 'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
		ctrlWing.FromBuffer(buffer);	//�Ǹ���Ҫ����֫(�����������������֫)
		otherWing.FromBuffer(buffer);	//�Ǹָ�������֫(����һ�����������֫)
		if( version>0&&
			(doc_type==1&&version<5010102)||	//TMA(V5,1,1,2)
			(doc_type==2&&version<2000502)||	//LMA(V2,0,5,2)
			(doc_type==4&&version<1030502)||	//LDS(V1,3,5,2)
			(doc_type==5&&version<1030502))		//TDA(V1,3,5,2)
		{	//����TMAV5.1.1.2�濪ʼ��һ֫ƽ��һ֫ͶӰʱ�ļ����㷨�����仯���ʶԾɰ汾����֫ƽ�ƽ��ƴ���(��С) wjh-2016.4.27
			if(ctrlWing.operStyle==0&&otherWing.operStyle!=0)
				ctrlWing.operStyle=otherWing.operStyle;
			else if(ctrlWing.operStyle!=0&&otherWing.operStyle==0)
				otherWing.operStyle=ctrlWing.operStyle;
		}
		else if( version>0&&
			(doc_type==1&&version<5010300)||	//TMA(V5,1,3,0)
			(doc_type==2&&version<2000600)||	//LMA(V2,0,6,0)
			(doc_type==4&&version<1030600)||	//LDS(V1,3,6,0)
			(doc_type==5&&version<1030600))		//TDA(V1,3,6,0)
		{	//TMAV5.1.3.0���TMAV5.1.2.0��������֫������Ƶ�΢�� wjh-2016.6.4
			if(ctrlWing.operStyle==0&&otherWing.operStyle==0)
				fLenOffsetOfCoordWing=0;
			//TMA V5.1.2.0��Ϊ���ص���Ը��ص����ƫ�ƣ����°汾ͳһ��Ϊ���ص�������ص㡡wjh-2016.6.4
			else if(ctrlWing.operStyle==0&&otherWing.operStyle!=0)
				fLenOffsetOfCoordWing*=-1.0;
		}
	}
	//����spatialOperationStyle=16,17ʱ��Ч
	if(spatialOperationStyle==16||spatialOperationStyle==17)
	{
		datumLineStart.FromBuffer(buffer,version,doc_type);
		datumLineEnd.FromBuffer(buffer,version,doc_type);
		if( version==0||
			(doc_type==1&&version>=4020009)||	//TMA(V4,2,0,9)
			(doc_type==2&&version>=1020009)||	//LMA(V1,2,0,9)
			(doc_type==4&&version>=1020009)||	//LDS(V1,2,0,9)
			(doc_type==5&&version>=1020009))	//TDA(V1,2,0,9)
			buffer.ReadInteger(&hDatumLine);
		if( version==0||
			(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
			(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
			(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
			(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
			buffer.ReadByte(&cMirTransType);
	}
	if(  version==0||(
		(doc_type==1&&version>=5010100)||	//TMA V5.1.1.0
		(doc_type==2&&version>=2000501)||	//LMA V2.0.5.1
		(doc_type==4&&version>=1030501)||	//LDS V1.3.5.1
		(doc_type==5&&version>=1030501)))	//TDA V1.3.5.1
		buffer.ReadByte(&cbSimpleUdfFlag);
	//�ڶ���ƫ�Ʋ���(����->����ʸ��ƫ��)���������ڴ�Ӹ˼���(��)���λ�ü���
	//BYTE wingXGStyle,wingYGStyle;		//0.����ƫ��;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	//double offset_x_dist,offset_y_dist;	//����gStyle=4ʱ��Ч
	buffer.ReadByte(&cWorkWingStyle);//�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫0:�Զ��ж�;1:X֫;2:Y֫;3:˫֫;�����Զ��ж�
	wing_x_offset.FromBuffer(buffer);
	wing_y_offset.FromBuffer(buffer);	//��ʱ����WING_OFFSET_STYLE::operStyle����
}
void CMultiOffsetPos::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(	(version!=0&&doc_type==1&&version<5000400)||	//TMA(V5,0,4,0)
		(version!=0&&doc_type==2&&version<2000400)||	//LMA(V2,0,4,0)
		(version!=0&&doc_type==4&&version<1030400)||	//LDS(V1,3,4,0)
		(version!=0&&doc_type==5&&version<1030400))	//TDA(V1,3,4,0)
		jgber_cal_style==2 ? buffer.WriteByte(0) : buffer.WriteByte(jgber_cal_style);
	else
		buffer.WriteByte(jgber_cal_style);
	buffer.WriteByte(origin_pos_style);	//0.�ڵ�λ��;1.��׼��;2.��׼�Ǹֹؼ���;3.��һ�˽ڵ�����;4.��һ���������
	buffer.WriteInteger(datum_jg_h);	//���λ�û�׼�Ǹ�
		//����origin_pos_style=2ʱ��Ч
	buffer.WriteByte(cDatumAngleWing);	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	datumWingOffset.ToBuffer(buffer);
	//��һ��ƫ�Ʋ���(��ʼ��->����Ҳ�ɳƻ��㶨λ)����: 0.ָ����׼��
	// 1:������ͶӰ      2:X֫����ͶӰ       3:Y֫����ͶӰ
	// 4:������ƽ��      5:X֫����ƽ��       6:Y֫����ƽ��
	// 7:ͶӰ���߻�����  8:ͶӰX֫���߻����� 9:ͶӰY֫���߻�����
	//10:ƽ�����߻����� 11:ƽ��X֫���߻�����12:ƽ��Y֫���߻�����
	//13:X֫ƫ��׼��    14:Y֫ƫ��׼��      15:��֫���������֫�������
	//16:ָ����ͶӰ		17:ָ����ƽ��		18:������Ƶ�ͶӰ 19:������Ƶ�ƽ��
	buffer.WriteByte(spatialOperationStyle);
	if(spatialOperationStyle==0)
		datumPoint.ToBuffer(buffer,version,doc_type);
	if( version==0||
		(doc_type==1&&version>=4010300)||	//TMA(V4,1,3,0)
		(doc_type==2&&version>=1010300)||	//LMA(V1,1,3,0)
		(doc_type==4&&version>=1030002)||	//LDS(V1,3,0,2)
		(doc_type==5&&version>=1030002))	//TDA(V1,3,0,2)
		buffer.WriteByte(len_offset_type);		//len_offset_dist��ƫ������0.��׼�˼���������,1.���������ص�ǰ�˼���ֱ����ƫ��,'X','Y','Z'
	buffer.WriteDouble(len_offset_dist);		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	buffer.WriteDouble(datum_offset_dist);	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	buffer.WriteByte(cFaceOffsetDatumLine);	//��spatialOperationStyle==17ʱ��Ч
	buffer.WriteDouble(face_offset_norm.x);	//ƽ���淨��X����
	buffer.WriteDouble(face_offset_norm.y);	//ƽ���淨��Y����
	buffer.WriteDouble(face_offset_norm.z);	//ƽ���淨��Z����
	buffer.WriteInteger(huoqu_jg_h);		//������׼�Ǹ־��
	buffer.WriteByte(huoqu_pos_auto_0_x_wing1_y_wing2);
	buffer.WriteDouble(huoqu_dist);		//�����߼�϶
	buffer.WriteDouble(offset_zhun);		//ƫ��׼�ߵ�ƫ����
	buffer.WriteDouble(fEccentricDist);	//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
		//����spatialOperationStyle=15ʱ��Ч
	if(spatialOperationStyle==15)
	{
		buffer.WriteByte(coordCtrlType);	//0:X,Z����;1:Y,Z����;X,Y����
		buffer.WriteByte(cMainCoordCtrlWing);//�Ǹ�����֫ 'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
		ctrlWing.ToBuffer(buffer);	//�Ǹ���Ҫ����֫(�����������������֫)
		otherWing.ToBuffer(buffer);	//�Ǹָ�������֫(����һ�����������֫)
	}
		//����spatialOperationStyle=16,17ʱ��Ч
	if(spatialOperationStyle==16||spatialOperationStyle==17)
	{
		datumLineStart.ToBuffer(buffer,version,doc_type);
		datumLineEnd.ToBuffer(buffer,version,doc_type);
		if( version==0||
			(doc_type==1&&version>=4020009)||	//TMA(V4,2,0,9)
			(doc_type==2&&version>=1020009)||	//LMA(V1,2,0,9)
			(doc_type==4&&version>=1020009)||	//LDS(V1,2,0,9)
			(doc_type==5&&version>=1020009))	//TDA(V1,2,0,9)
			buffer.WriteInteger(hDatumLine);
		if( version==0||
			(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
			(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
			(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
			(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
			buffer.WriteByte(cMirTransType);
	}
	if(  version==0||(
		(doc_type==1&&version>=5010100)||	//TMA V5.1.1.0
		(doc_type==2&&version>=2000501)||	//LMA V2.0.5.1
		(doc_type==4&&version>=1030501)||	//LDS V1.3.5.1
		(doc_type==5&&version>=1030501)))	//TDA V1.3.5.1
		buffer.WriteByte(cbSimpleUdfFlag);
	//�ڶ���ƫ�Ʋ���(����->����ʸ��ƫ��)���������ڴ�Ӹ˼���(��)���λ�ü���
	//BYTE wingXGStyle,wingYGStyle;		//0.����ƫ��;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	//double offset_x_dist,offset_y_dist;	//����gStyle=4ʱ��Ч
	buffer.WriteByte(cWorkWingStyle);//�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫0:�Զ��ж�;1:X֫;2:Y֫;3:˫֫;�����Զ��ж�
	wing_x_offset.ToBuffer(buffer);
	wing_y_offset.ToBuffer(buffer);	//��ʱ����WING_OFFSET_STYLE::operStyle����
}

void CMultiOffsetPos::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<�˼���㶨λ��Ϣ>\n");
	if (jgber_cal_style!=1)
		fprintf(fp,"<�Ǹ������㷽ʽ jgber_cal_style=\"0.��������λ\"/>\n");
	else if (jgber_cal_style==1)
		fprintf(fp,"<�Ǹ������㷽ʽ jgber_cal_style=\"1.ֱ��ָ���������\"/>\n");
	if (origin_pos_style==0)
		fprintf(fp,"<ԭ��λ������ origin_pos_style=\"0.�ڵ�λ��\"/>\n");
	else if (origin_pos_style==1)
		fprintf(fp,"<ԭ��λ������ origin_pos_style=\"1.��׼��\"/>\n");
	else if (origin_pos_style==2)
		fprintf(fp,"<ԭ��λ������ origin_pos_style=\"2.��׼�Ǹֹؼ���\"/>\n");
	else if (origin_pos_style==3)
		fprintf(fp,"<ԭ��λ������ origin_pos_style=\"3.��һ�˽ڵ�����\"/>\n");
	else if (origin_pos_style==4)
		fprintf(fp,"<ԭ��λ������ origin_pos_style=\"4.��һ���������\"/>\n");
	fprintf(fp,"<���λ�û�׼�Ǹ� datum_jg_h=\"0X%X\"/>\n",datum_jg_h);
	//����origin_pos_style=2ʱ��Ч
	fprintf(fp,"<��׼�Ǹ�֫ cDatumAngleWing=\"%d\"/>\n",cDatumAngleWing);
	datumWingOffset.ToXmlFile(fp,schema);
	if (spatialOperationStyle==0)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"0.ָ����׼��\"/>\n");
	else if (spatialOperationStyle==1)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"1.������ͶӰ\"/>\n");
	else if (spatialOperationStyle==2)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"2.X֫����ͶӰ\"/>\n");
	else if (spatialOperationStyle==3)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"3.Y֫����ͶӰ\"/>\n");
	else if (spatialOperationStyle==4)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"4.������ƽ��\"/>\n");
	else if (spatialOperationStyle==5)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"5.X֫����ƽ��\"/>\n");
	else if (spatialOperationStyle==6)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"6.Y֫����ƽ��\"/>\n");
	else if (spatialOperationStyle==7)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"7.ͶӰ���߻�����\"/>\n");
	else if (spatialOperationStyle==8)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"8.ͶӰX֫���߻�����\"/>\n");
	else if (spatialOperationStyle==9)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"9.ͶӰY֫���߻�����\"/>\n");
	else if (spatialOperationStyle==10)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"10.ƽ�����߻�����\"/>\n");
	else if (spatialOperationStyle==11)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"11.ƽ��X֫���߻�����\"/>\n");
	else if (spatialOperationStyle==12)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"12.ƽ��Y֫���߻�����\"/>\n");
	else if (spatialOperationStyle==13)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"13.X֫ƫ��׼��\"/>\n");
	else if (spatialOperationStyle==14)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"14.Y֫ƫ��׼��\"/>\n");
	else if (spatialOperationStyle==15)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"15.��֫�������\"/>\n");
	else if (spatialOperationStyle==16)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"16.ָ����ͶӰ\"/>\n");
	else if (spatialOperationStyle==17)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"17.ָ����ƽ��\"/>\n");
	else if (spatialOperationStyle==18)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"18.������Ƶ�ͶӰ\"/>\n");
	else if (spatialOperationStyle==19)
		fprintf(fp,"<��һ��ƫ�Ʋ������� spatialOperationStyle=\"19.������Ƶ�ƽ��\"/>\n");
	if(spatialOperationStyle==0)
		datumPoint.ToXmlFile(fp,schema);
	sprintf(sText,"%f",len_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ�� len_offset_dist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",datum_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<��ӵ��Ի�׼֫�ط��ߵ�ƫ�� datum_offset_dist=\"%s\"/>\n",sText);
	fprintf(fp,"<ƽ�����׼�� cFaceOffsetDatumLine=\"%d\"/>\n",cFaceOffsetDatumLine);
	sprintf(sText,"%f",face_offset_norm.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",face_offset_norm.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",face_offset_norm.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<ƽ���淨�� face_offset_norm=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"<������׼�Ǹ־�� huoqu_jg_h=\"0X%X\"/>\n",huoqu_jg_h);
	fprintf(fp,"<������λ�õ��ж� huoqu_pos_auto_0_x_wing1_y_wing2=\"%d\"/>\n",huoqu_pos_auto_0_x_wing1_y_wing2);
	sprintf(sText,"%f",huoqu_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<�����߼�϶ huoqu_dist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",offset_zhun);
	SimplifiedNumString(sText);
	fprintf(fp,"<ƫ��׼�ߵ�ƫ���� offset_zhun=\"%s\"/>\n",sText);
	sprintf(sText,"%f",fEccentricDist);
	SimplifiedNumString(sText);
	fprintf(fp,"<ƫ�ľ� fEccentricDist=\"%s\"/>\n",sText);
	//����spatialOperationStyle=15ʱ��Ч
	if(spatialOperationStyle==15)
	{
		fprintf(fp,"<����������� coordCtrlType=\"%d\"/>\n",coordCtrlType);
		fprintf(fp,"<�Ǹ�����֫ cMainCoordCtrlWing=\"%d\"/>\n",cMainCoordCtrlWing);
		ctrlWing.ToXmlFile(fp,schema);
		otherWing.ToXmlFile(fp,schema);
	}
	//����spatialOperationStyle=16,17ʱ��Ч
	if(spatialOperationStyle==16||spatialOperationStyle==17)
	{
		datumLineStart.ToXmlFile(fp,schema);
		datumLineEnd.ToXmlFile(fp,schema);
	}
	if (cWorkWingStyle==0)
		fprintf(fp,"<�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫ cWorkWingStyle=\"0.�Զ��ж�\"/>\n");
	else if (cWorkWingStyle==1)
		fprintf(fp,"<�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫ cWorkWingStyle=\"1.X֫\"/>\n");
	else if (cWorkWingStyle==2)
		fprintf(fp,"<�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫ cWorkWingStyle=\"2.Y֫\"/>\n");
	else if (cWorkWingStyle==3)
		fprintf(fp,"<�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫ cWorkWingStyle=\"3.˫֫\"/>\n");
	else
		fprintf(fp,"<�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫ cWorkWingStyle=\"4.�Զ��ж�\"/>\n");
	wing_x_offset.ToXmlFile(fp,schema);
	wing_y_offset.ToXmlFile(fp,schema);
	fprintf(fp,"</�˼���㶨λ��Ϣ>\n");
}
void CMultiOffsetPos::FromJgBerPosPara(CJgBerPosPara *pPosPara)
{
	ClearContents();
	// 1:������ͶӰ      2:X֫����ͶӰ       3:Y֫����ͶӰ
	// 4:������ƽ��      5:X֫����ƽ��       6:Y֫����ƽ��
	// 7:ͶӰ���߻�����  8:ͶӰX֫���߻����� 9:ͶӰY֫���߻�����
	//10:ƽ�����߻����� 11:ƽ��X֫���߻�����12:ƽ��Y֫���߻�����
	//13:X֫ƫ��׼��    14:Y֫ƫ��׼��      15:��֫�������
	//16:ָ����ͶӰ		17:ָ����ƽ��
	if(pPosPara->jgber_cal_style==0)
	{	//����ƫ��
		//��һ�� ԭʼ�㶨λ���ڵ�λ��
		if(pPosPara->datum_pos_style==15)
		{
			origin_pos_style=0;	//ָ����׼��	���ܸ�ֵ����������ٽ�����㶨λ��ʽ��Ϊ��������ʱ����GetStartInitPos�����wjh-2016.11.18
			datumPoint=pPosPara->datum_point;
		}
		else if(pPosPara->IsFaceOffset()&&pPosPara->bByOtherEndOffset)
		{
			origin_pos_style=3;	//��һ�˽ڵ�����
			//origin_pos_style=4;	//��һ���������
		}
		else
			origin_pos_style=0;//�ڵ�λ��
		//�ڶ��� ԭʼ��->����(���㶨λ)��
		if(pPosPara->datum_pos_style==0)
		{
			if(pPosPara->datum_to_ber_style==0)		//������Ƶ�ͶӰ
				spatialOperationStyle=18;
			else //if(pPosPara->datum_to_ber_style==1)//������Ƶ�ƽ��
				spatialOperationStyle=19;
		}
		else if(pPosPara->datum_pos_style<=14)
			spatialOperationStyle=(BYTE)pPosPara->datum_pos_style;
		else //if(pPosPara->datum_pos_style==15)
			spatialOperationStyle=0;	//ԭʼ�㼴����,�޲���
		this->datum_jg_h=pPosPara->datum_jg_h;
		this->huoqu_jg_h=pPosPara->huoqu_jg_h;
		this->huoqu_dist=pPosPara->huoqu_dist;
		this->offset_zhun=pPosPara->offset_zhun;
		this->len_offset_dist=pPosPara->len_offset_dist;
		this->fEccentricDist=pPosPara->fEccentricDist;
		this->datum_offset_dist=pPosPara->datum_offset_dist;
		this->cFaceOffsetDatumLine=pPosPara->cFaceOffsetDatumLine;
		this->face_offset_norm=pPosPara->face_offset_norm;
		//��ʱ������δ���� wjh-2012.4.27
		huoqu_pos_auto_0_x_wing1_y_wing2=(BYTE)pPosPara->huoqu_pos_auto_0_x_wing1_y_wing2;
		//pPosPara->datum_to_ber_style;	//����datum_pos_style==0&&datum_to_ber_style==1��Ч�Ѱ�����IsFaceOffset��
		//pPosPara->m_iPosParaType;	//������δ����
		//������ ����->Ŀ���(���)����֫ƫ�ƾ�Ϊ0
		cWorkWingStyle=(BYTE)pPosPara->work_wing_auto0_x1_y2_both3;
		wing_x_offset.gStyle=(BYTE)pPosPara->offset_X_dist_style;
		wing_x_offset.offsetDist=pPosPara->offset_X_dist;
		wing_y_offset.gStyle=(BYTE)pPosPara->offset_Y_dist_style;
		wing_y_offset.offsetDist=pPosPara->offset_Y_dist;
	}
	else if(pPosPara->jgber_cal_style==1||pPosPara->jgber_cal_style==2)
	{	//������ͶӰ�������ƽ��
		//��һ�� ԭʼ�㶨λ���ڵ�λ��
		origin_pos_style=0;
		//�ڶ��� ԭʼ��->����(���㶨λ)�� ͬ��ʼ��
			//1.ת����׼��Ϊָ����
		datumLineStart.SetRapidAngleBer(pPosPara->start_jg_h,(BYTE)pPosPara->iStartJgBerStyle,2,pPosPara->fStartJgLengOffsetX);
		datumLineEnd.SetRapidAngleBer(pPosPara->end_jg_h,(BYTE)pPosPara->iEndJgBerStyle,2,pPosPara->fEndJgLengOffsetX);
		//2.ת�����㶨λ����
		if(pPosPara->jgber_cal_style==1)
			spatialOperationStyle=16;	//ָ����ͶӰ
		else if(pPosPara->jgber_cal_style==2)
		{
			spatialOperationStyle=17;	//ָ����ƽ��
			cFaceOffsetDatumLine=pPosPara->cFaceOffsetDatumLine;	//��׼��ƫ������0:����;'X':X֫����;'Y':Y֫����
			face_offset_norm=pPosPara->face_offset_norm;
		}
		//������ ����->Ŀ���(���)����֫ƫ�ƾ�Ϊ0
		cWorkWingStyle=3;
		wing_x_offset.gStyle=4;
		wing_x_offset.offsetDist=0;
		wing_y_offset.gStyle=4;
		wing_y_offset.offsetDist=0;
	}
	else if(pPosPara->jgber_cal_style==3)
	{	//ֱ��ָ������
		jgber_cal_style=1;
		/*//��һ�� ԭʼ�㶨λ��ָ������
		origin_pos_style=0;
		//�ڶ��� ԭʼ��->����(���㶨λ)�� ͬ��ʼ��
		spatialOperationStyle=0;
		//������ ����->Ŀ���(���)����֫ƫ�ƾ�Ϊ0
		cWorkWingStyle=3;
		wing_x_offset.gStyle=4;
		wing_x_offset.offsetDist=0;
		wing_y_offset.gStyle=4;
		wing_y_offset.offsetDist=0;*/
	}
}
BOOL CMultiOffsetPos::IsFaceOffset()		//�Ƿ�Ϊƽ��	wht 10-07-30		
{
	if(  spatialOperationStyle==17||(spatialOperationStyle>3&&spatialOperationStyle<=6)||
		(spatialOperationStyle>9&&spatialOperationStyle<=12)||spatialOperationStyle==19)
		return TRUE;
	else
		return FALSE;
}
BOOL CMultiOffsetPos::IsInDatumJgBer()		//�����Ƿ��ڻ�׼�Ǹ�������
{
	if(spatialOperationStyle==1||spatialOperationStyle==4||spatialOperationStyle==7||spatialOperationStyle==10)
		return TRUE;
	else
		return FALSE;
}
BOOL CMultiOffsetPos::IsInDatumJgWingX()	//�����Ƿ��ڻ�׼�Ǹ�X֫��
{
	if(spatialOperationStyle==2||spatialOperationStyle==5||spatialOperationStyle==8||
		spatialOperationStyle==11||spatialOperationStyle==13)
		return TRUE;
	else
		return FALSE;
}
BOOL CMultiOffsetPos::IsInDatumJgWingY()	//�����Ƿ��ڻ�׼�Ǹ�Y֫��
{
	if(spatialOperationStyle==3||spatialOperationStyle==6||spatialOperationStyle==9||
		spatialOperationStyle==12||spatialOperationStyle==14)
		return TRUE;
	else
		return FALSE;
}
BOOL CMultiOffsetPos::IsInHuoQuLine(bool blIncOffsetGLine/*=false*/)		//�����Ƿ��ڻ�������
{
	if(spatialOperationStyle>=7&&spatialOperationStyle<=12)
		return TRUE;
	else if(blIncOffsetGLine&&(spatialOperationStyle==13||spatialOperationStyle==14))
		return TRUE;
	else
		return FALSE;
}
BOOL CMultiOffsetPos::IsInOffsetZhunLine()	//�����Ƿ���ƫ��׼����
{
	if(spatialOperationStyle==13||spatialOperationStyle==14)
		return TRUE;
	else
		return FALSE;
}
int CMultiOffsetPos::GetDatumType()	//������ת��Ϊ��һ����
{
	if(spatialOperationStyle==0)
		return 6;	//ָ����׼��
	else if(spatialOperationStyle<=3)
		return 1;	//ͶӰ
	else if(spatialOperationStyle<=6)
		return 2;	//ƽ��
	else if(spatialOperationStyle<=9)
		return 3;	//ͶӰ����
	else if(spatialOperationStyle<=12)
		return 4;	//ƽ�ƻ���
	else if(spatialOperationStyle<=14)
		return 5;	//ƫ��׼��
	else //if(spatialOperationStyle==15)
		return 6;	//ָ����׼��
}
BOOL CMultiOffsetPos::ChangeToDatumType(int prj1_faceoffset2_huoqu3and4_offsetzhun5)	//������ת��Ϊ��һ����
{
	if(spatialOperationStyle==0||spatialOperationStyle>14)
		return FALSE;
	switch(prj1_faceoffset2_huoqu3and4_offsetzhun5)
	{
	case 1:	//ת��Ϊ��ͨͶӰ
		if(spatialOperationStyle>=1&&spatialOperationStyle<=3)
			return TRUE;
		else if(spatialOperationStyle>=4&&spatialOperationStyle<=6)
		{
			spatialOperationStyle-=3;
			return TRUE;
		}
		else if(spatialOperationStyle>=7&&spatialOperationStyle<=9)
		{
			spatialOperationStyle-=6;
			return TRUE;
		}
		else if(spatialOperationStyle>=10&&spatialOperationStyle<=12)
		{
			spatialOperationStyle-=9;
			return TRUE;
		}
		else if(spatialOperationStyle>=13&&spatialOperationStyle<=14)
		{
			spatialOperationStyle-=11;
			return TRUE;
		}
		else if(spatialOperationStyle==17)
			spatialOperationStyle=16;
		else if(spatialOperationStyle==19)
			spatialOperationStyle=18;
		else
			return FALSE;
	case 2:	//ת��Ϊ��ͨƽ��
		if(face_offset_norm.IsZero())
			return FALSE;
		if(spatialOperationStyle>=1&&spatialOperationStyle<=3)
		{
			spatialOperationStyle+=3;
			return TRUE;
		}
		else if(spatialOperationStyle>=4&&spatialOperationStyle<=6)
			return TRUE;
		else if(spatialOperationStyle>=7&&spatialOperationStyle<=9)
		{
			spatialOperationStyle-=3;
			return TRUE;
		}
		else if(spatialOperationStyle>=10&&spatialOperationStyle<=12)
		{
			spatialOperationStyle-=6;
			return TRUE;
		}
		else if(spatialOperationStyle>=13&&spatialOperationStyle<=14)
		{
			spatialOperationStyle-=8;
			return TRUE;
		}
		else if(spatialOperationStyle==16)
			spatialOperationStyle=17;
		else if(spatialOperationStyle==18)
			spatialOperationStyle=19;
		else
			return FALSE;
	case 3:	//ת��ΪͶӰ������
		if(spatialOperationStyle>=1&&spatialOperationStyle<=3)
		{
			spatialOperationStyle+=6;
			return TRUE;
		}
		else if(spatialOperationStyle>=4&&spatialOperationStyle<=6)
		{
			spatialOperationStyle+=3;
			return TRUE;
		}
		else if(spatialOperationStyle>=7&&spatialOperationStyle<=9)
			return TRUE;
		else if(spatialOperationStyle>=10&&spatialOperationStyle<=12)
		{
			spatialOperationStyle-=3;
			return TRUE;
		}
		else if(spatialOperationStyle>=13&&spatialOperationStyle<=14)
		{
			spatialOperationStyle-=5;
			return TRUE;
		}
		else
			return FALSE;
	case 4:	//ת��Ϊƽ�ƻ�����
		if(spatialOperationStyle>=1&&spatialOperationStyle<=3)
		{
			spatialOperationStyle+=9;
			return TRUE;
		}
		else if(spatialOperationStyle>=4&&spatialOperationStyle<=6)
		{
			spatialOperationStyle+=6;
			return TRUE;
		}
		else if(spatialOperationStyle>=7&&spatialOperationStyle<=9)
		{
			spatialOperationStyle+=3;
			return TRUE;
		}
		else if(spatialOperationStyle>=10&&spatialOperationStyle<=12)
			return TRUE;
		else if(spatialOperationStyle>=13&&spatialOperationStyle<=14)
		{
			spatialOperationStyle-=2;
			return TRUE;
		}
		else
			return FALSE;
	case 5:	//ת��Ϊƫ��׼��
		if(spatialOperationStyle>=2&&spatialOperationStyle<=3)
		{
			spatialOperationStyle+=11;
			return TRUE;
		}
		else if(spatialOperationStyle>=5&&spatialOperationStyle<=6)
		{
			spatialOperationStyle+=8;
			return TRUE;
		}
		else if(spatialOperationStyle>=8&&spatialOperationStyle<=9)
		{
			spatialOperationStyle+=5;
			return TRUE;
		}
		else if(spatialOperationStyle>=11&&spatialOperationStyle<=12)
		{
			spatialOperationStyle+=2;
			return TRUE;
		}
		else if(spatialOperationStyle>=13&&spatialOperationStyle<=14)
			return TRUE;
		else
			return FALSE;
	default:
		return FALSE;
	}
}
bool CMultiOffsetPos::ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord)
{
	bool replaced=false;
	if(!replacer.SelectPerfectPartHandle(pickpos_coord))
		return false;
	replaced|=replacer.VerifyAndReplace(&datum_jg_h);
	replaced|=replacer.VerifyAndReplace(&huoqu_jg_h);
	if(spatialOperationStyle==16||spatialOperationStyle==17)
	{
		replaced|=datumLineStart.ReplaceReferenceHandle(replacer,NULL);
		replaced|=datumLineEnd.ReplaceReferenceHandle(replacer,NULL);
	}
	//��ȱ��idClassType�������ݲ��ڴ˵��á�wjh-2014.5.5
	//if(spatialOperationStyle==0)	//��׼��
	//	replaced|=datumPoint.ReplaceReferenceHandle(replacer,pickpos_coord);
	return replaced;
}
long CMultiOffsetPos::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	long nInitCount = pListHandlers->Count;
	if(datum_jg_h>0)
		pListHandlers->Append(HANDLE_ADDR(&datum_jg_h));
	if(huoqu_jg_h>0)
		pListHandlers->Append(HANDLE_ADDR(&huoqu_jg_h));
	if(hDatumLine>0)
		pListHandlers->Append(HANDLE_ADDR(&hDatumLine));
	if (spatialOperationStyle == 0)
		datumPoint.StatObjHandleReference(pListHandlers);
	else if(spatialOperationStyle==16 || spatialOperationStyle==17)
	{
		datumLineStart.StatObjHandleReference(pListHandlers);
		datumLineEnd.StatObjHandleReference(pListHandlers);
	}
	return pListHandlers->Count-nInitCount;
}
CLDSLineAngle::OPENING_CUT::OPENING_CUT()
{
	siCornerFilletR=50;
	dfLateralBtmXorY=dfLenPosFrom=dfLenPosTo=0;
	//��ֹ�㿪�ڶ�����֫��Ե������Եײ���ƫ������ʼ->��Ϊ����
	dfTipDeltaS=dfTipDeltaE=0;
}
CLDSLineAngle::KAIHE_SECTION::KAIHE_SECTION()
{
	ciOperWing=0;		//���ϽǵĲ���֫��0.X; 1.Y; 'X'; 'Y'
	dfFromX=dfToX=0;	//���ڵײ�����ֹ����λ�ã��ԽǸ���ʼ���棨��������ͷ��Ϊ���
	dfReservedSizeY=0;	//���Ͻ�ʱ�ĺ��������
	dfOpenAngle=0;		//>0�����Ƕ�����<0���ϽǶ�����0.���迪�Ͻ�
}
bool CLDSLineAngle::set_blOddByBoltCanAcrossMiddleSect(bool blValue)
{
	if(_blOddByBoltCanAcrossMiddleSect!=blValue)
		this->is_data_modified=true;
	return _blOddByBoltCanAcrossMiddleSect=blValue;
}
bool CLDSLineAngle::set_blScatterBoltLocaTolerance(bool blValue)
{
	if (_blScatterBoltLocaTolerance!=blValue)
		this->is_data_modified=true;
	return _blScatterBoltLocaTolerance=blValue;
}
double CLDSLineAngle::get_dfFilletROfSX(){return this->_dfFilletROfSX;}
double CLDSLineAngle::set_dfFilletRofSX(double dfFilletR){
	if(_dfFilletROfSX!=dfFilletR)
		SetModified();
	return _dfFilletROfSX=dfFilletR;
}
double CLDSLineAngle::get_dfFilletROfSY(){return this->_dfFilletROfSY;}
double CLDSLineAngle::set_dfFilletRofSY(double dfFilletR){
	if(_dfFilletROfSY!=dfFilletR)
		SetModified();
	return _dfFilletROfSY=dfFilletR;
}
double CLDSLineAngle::get_dfFilletROfEX(){return this->_dfFilletROfEX;}
double CLDSLineAngle::set_dfFilletRofEX(double dfFilletR){
	if(_dfFilletROfEX!=dfFilletR)
		SetModified();
	return _dfFilletROfEX=dfFilletR;
}
double CLDSLineAngle::get_dfFilletROfEY(){return this->_dfFilletROfEY;}
double CLDSLineAngle::set_dfFilletRofEY(double dfFilletR){
	if(_dfFilletROfEY!=dfFilletR)
		SetModified();
	return _dfFilletROfEY=dfFilletR;
}
bool CLDSLineAngle::IsLappedAnchorStart()	//ʼ���Ƿ�Ϊ��Ӷ�λ
{
	if(desStartPos.jgber_cal_style==1)
		return false;	//ָ�����궨λ
	if(desStartPos.origin_pos_style==1)
		return false;	//ָ����׼�㶨λ
	if(desStartPos.spatialOperationStyle>14)
		return false;
	else if(desStartPos.spatialOperationStyle==0)
		return false;	//ָ����׼��
	return true;
}
bool CLDSLineAngle::IsLappedAnchorEnd()	//�ն��Ƿ�Ϊ��Ӷ�λ
{
	if(desEndPos.jgber_cal_style==1)
		return false;	//ָ�����궨λ
	if(desEndPos.origin_pos_style==1)
		return false;	//ָ����׼�㶨λ
	if(desEndPos.spatialOperationStyle>14)
		return false;
	else if(desEndPos.spatialOperationStyle==0)
		return false;	//ָ����׼��
	return true;
}
BOOL CLDSLineAngle::IsInsideAngle(CLDSLineAngle* pAngle,f3dPoint norm,int *x_wing0_y_wing1)
{
	double ddx,ddy;
	ddx = pAngle->vxWingNorm*norm;
	ddy = pAngle->vyWingNorm*norm;
	if(fabs(ddx)>fabs(ddy))
	{
		if(x_wing0_y_wing1)
			*x_wing0_y_wing1 = 0;
		if(ddx>0)	//����
			return TRUE;
		else
			return FALSE;
	}
	else 
	{
		if(x_wing0_y_wing1)
			*x_wing0_y_wing1 = 1;
		if(ddy>0)	//����
			return TRUE;
		else
			return FALSE;
	}
}

double CLDSLineAngle::set_wing_wide(double wide)
{
	if(wide>0)
	{
		size_wide = wide;
		size_height=0;
		is_data_modified = TRUE;
		return size_wide;
	}
	else
		return size_wide;
}

double CLDSLineAngle::set_wing_thick(double thick)
{
	if(thick>0&&thick<size_wide)
	{
		size_thick = thick;
		is_data_modified = TRUE;
		return size_thick;
	}
	else
		return size_thick;
}

double CLDSLineAngle::set_y_wing_width(double y_wing_wide)
{
	if(y_wing_wide>0)
	{
		size_height = y_wing_wide;
		is_data_modified = TRUE;
		return size_height;
	}
	else
		return size_height;
}

BOOL CLDSLineAngle::m_bDisplaySolidAxisLine=TRUE;		//��ʾ�Ǹ����� wht 10-12-27
double CLDSLineAngle::KAIHEJIAO_ANGLE_THRESHOLD=2;		//��λ��
double CLDSLineAngle::KAIHEJIAO_CLEARANCE_THRESHOLD=1;	//1mm
CLDSLineAngle::CLDSLineAngle()
{
	size_idClassType=m_nClassTypeId=CLS_LINEANGLE;
	size_cSubClassType='L';
	//strcpy(m_sClassName,"CLDSLineAngle");
	_cnCalNormHits=0;
	dwLayStyle=0;
	m_bUserSpecStartCutAngle=m_bUserSpecEndCutAngle=FALSE;
	memset(cut_angle,0,sizeof(cut_angle));
	memset(cut_wing_para,0,sizeof(cut_wing_para));
	memset(cut_wing,0,sizeof(cut_wing));
	memset(&des_norm_x,0,sizeof(DESIGN_JGWING_NORM));
	memset(&des_norm_y,0,sizeof(DESIGN_JGWING_NORM));
	pOldDesStartPos=pOldDesEndPos=NULL;
	//des_start_pos.face_offset_norm.Set(0,0,1);	//Ĭ��Z��Ϊƽ���淨��
	//des_end_pos.face_offset_norm.Set(0,0,1);
	m_cPosCtrlDatumLineType=0;		//'X':X֫��׼���߿���;'Y':Y֫��׼���߿���;�������߿���
	m_cPosCtrlType=0;				//λ�ÿ�������:0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	if(sysPara==NULL||sysPara->IsPreferAngleSimplePosMode())
		desStartPos.jgber_cal_style=desEndPos.jgber_cal_style=2;//Ĭ��Ϊ�򵥶�λģʽ��wjh-2016.3.16
	desStartPos.face_offset_norm.Set(0,0,1);	//Ĭ��Z��Ϊƽ���淨��
	desEndPos.face_offset_norm.Set(0,0,1);

	cMaterial = org_material='S';
	org_wing_wide= 100;
	size_wide = 100;
	org_wing_thick=10;
	size_thick = 10;
	m_cHeJiaoReservedWidthType='I';
	m_fHeJiaoReservedWidth=0;
	is_visible = TRUE;
	strcpy(connectStart.grade,"6.8");
	strcpy(connectEnd.grade,"6.8");
	connectMid.d = connectStart.d = connectEnd.d = 20;
	connectMid.rows = connectStart.rows = connectEnd.rows = 1;
	connectMid.uiLegacyN = connectStart.uiLegacyN = connectEnd.uiLegacyN = 2;
	group_father_jg_h = 0;
	memset(son_jg_h,0,sizeof(long)*4);
	kaihe_base_wing_x0_y1=2;
	des_norm_x.hViceJg = des_norm_y.hViceJg = 0;
	getjgzj(xWingXZhunJu,size_wide);
	getjgzj(xWingYZhunJu,size_wide);
	m_uStatMatCoef = 1;
	_start_push_flat_x1_y2=_end_push_flat_x1_y2=_mid_push_flat_x1_y2=0;
	_start_push_flat_length = _end_push_flat_length = _mid_push_flat_length = _mid_push_flat_up_offset=0;
	mid_push_flat_node_handle=0;
	dwPermission=dwStartPermission=dwEndPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
	bHuoQuStart=bHuoQuEnd=FALSE;
	m_bEnableTeG=FALSE;
	m_bReplPart=FALSE;
	_bCutBer=_bCutRoot=m_bWeldPart=false;
	_fCutAngleCoefR=0;
	_nCutAngleSpaceOdd=0;
	nominal_length=0;
	huoqu_r=0;
	huoqu_base_wing_x0_y1_both2=0;
	huoqu_handle=0;
	m_bFootNail=FALSE;
	_dfFilletROfSX=_dfFilletROfSY=_dfFilletROfEX=_dfFilletROfEY=0;
	_blScatterBoltLocaTolerance=false;		//Ĭ�ϰ����ת������˨���ϴ�Ŀյ�
	_blOddByBoltCanAcrossMiddleSect=false;	//Ĭ�ϲ������Խ�������ļ���ȡʱ��Ĭ������
}
CLDSLineAngle::~CLDSLineAngle()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody = NULL;
	}
	if(pOldDesStartPos)
		delete 	pOldDesStartPos;
	if(pOldDesEndPos)
		delete pOldDesEndPos;
}
bool CLDSLineAngle::IsBelongTo(int iClassTypeId)const
{
	if(iClassTypeId==m_nClassTypeId||iClassTypeId==CLS_LINEPART||iClassTypeId==CLS_PART||iClassTypeId==CLS_DBOBJECT)
		return true;
	else
		return false;
}
//����һ��ԭʼ����α��һ���Ǹ֣��Խ���һЩ���Ǹֿɽ��еĲ��� wjh-2016.9.3
CLDSLineAngle* CLDSLineAngle::Fabricate(CLDSPart* pOrgPart,const double* xArcTagentPt/*=NULL*/)
{
	if(pOrgPart==NULL || pOrgPart->GetClassTypeId()==CLS_BOLT)
		return NULL;
	if(!pOrgPart->IsLinePart()&&!pOrgPart->IsArcPart())
		return NULL;
	m_pOriginalPart=pOrgPart;
	handle=pOrgPart->handle;
	m_pModel=pOrgPart->BelongModel();
	this->CopyModuleInstanceInfo(pOrgPart);
	if(pOrgPart->IsLinePart())
	{
		SmartRodPtr pRod=(CLDSLinePart*)pOrgPart;
		pStart=pRod->pStart;
		pEnd=pRod->pEnd;
		baseline.SetStart(pRod->Start());
		baseline.SetEnd(pRod->End());
	}
	else if(pOrgPart->IsArcPart())
	{
		SmartArcPtr pArc=(CLDSArcPart*)pOrgPart;
		xStartNode.pNode=xEndNode.pNode=NULL;
		GEPOINT tangent(xArcTagentPt);
		GEPOINT center=pArc->center;
		GEPOINT vArcNorm=pArc->worknorm;
		GEPOINT vRadialVec=tangent-center;
		double z=vRadialVec*vArcNorm;
		if(fabs(z)>0)
			vRadialVec-=z*pArc->worknorm;
		normalize(vRadialVec);
		GEPOINT midPos=center+vRadialVec*(pArc->radius+pArc->offset_h);
		norm_x_wing=vRadialVec;
		GEPOINT axis_z=vRadialVec;
		RotateVectorAroundVector(axis_z,1,0,vArcNorm);
		norm_y_wing=norm_x_wing^axis_z;
		if(pOrgPart->GetClassTypeId()==CLS_ARCANGLE||pOrgPart->GetClassTypeId()==CLS_ARCFLAT)
		{
			size.wide =pArc.ArcAnglePointer()->GetWidth();
			size.thick=pArc.ArcAnglePointer()->GetThick();
		}
		else if(pOrgPart->GetClassTypeId()==CLS_ARCSLOT)
		{
			size.wide =pArc.ArcSlotPointer()->GetHeight();
			size.thick=pArc.ArcSlotPointer()->GetThick();
		}
		_material=pArc->cMaterial;
		midPos+=norm_y_wing*pArc->offset_v;
		GEPOINT start=midPos-axis_z*1000;
		GEPOINT end  =midPos+axis_z*1000;
		ucs.origin=start;
		ucs.axis_z=axis_z;
		ucs.axis_x=-norm_y_wing;
		ucs.axis_y=-norm_x_wing;
		baseline.SetStart(start);
		baseline.SetEnd(end);
		desStartPos.jgber_cal_style=1;
		desEndPos.jgber_cal_style=1;
		des_norm_x.bSpecific=true;
		des_norm_y.bSpecific=true;
		des_norm_x.spec_norm.vector=norm_x_wing;
		des_norm_y.spec_norm.vector=norm_y_wing;
		//if(pOrgPart->o
	}
	return this;
}
int GetAngleCtrlPointParamStr(CMultiOffsetPos* pPosParam,char cCurrAngleWing,const double* vWorkWingNorm,char* valueStr,UINT nMaxStrBufLen=100)
{
	if( pPosParam->jgber_cal_style!=2&&(pPosParam->spatialOperationStyle!=15&&
		pPosParam->spatialOperationStyle!=15&&pPosParam->spatialOperationStyle!=19))
		return 0;
	cCurrAngleWing=toupper(cCurrAngleWing);
	CXhChar50 itemText;
	if(pPosParam->spatialOperationStyle==15)	//��֫�������
	{
		if(pPosParam->cMainCoordCtrlWing==cCurrAngleWing)
		{
			if(pPosParam->coordCtrlType==0)
				itemText.Copy("X,Z����");
			else if(pPosParam->coordCtrlType==1)
				itemText.Copy("Y,Z����");
			else if(pPosParam->coordCtrlType==2)
				itemText.Copy("X,Y����");
		}
		else
		{
			if(pPosParam->coordCtrlType==0)
				itemText.Copy("Y����");
			else if(pPosParam->coordCtrlType==1)
				itemText.Copy("X����");
			else if(pPosParam->coordCtrlType==2)
				itemText.Copy("Z����");
		}
	}
	else if(pPosParam->spatialOperationStyle==18||pPosParam->face_offset_norm.IsZero())
		itemText.Copy("�������");
	else //if(pPosParam->spatialOperationStyle==19)
	{
		GEPOINT vWingNorm(vWorkWingNorm);
		char cCurrWingNorm=0,cFaceOffset=0;
		if(fabs(pPosParam->face_offset_norm.x)+fabs(pPosParam->face_offset_norm.y)<EPS)
		{
			cFaceOffset='Z';
			if(fabs(vWingNorm.x)>fabs(vWingNorm.y))
				cCurrWingNorm='Y';
			else
				cCurrWingNorm='X';
		}
		else if(fabs(pPosParam->face_offset_norm.x)+fabs(pPosParam->face_offset_norm.z)<EPS)
		{
			cFaceOffset='Y';
			if(fabs(vWingNorm.x)>fabs(vWingNorm.z))
				cCurrWingNorm='Z';
			else
				cCurrWingNorm='X';
		}
		else if(fabs(pPosParam->face_offset_norm.y)+fabs(pPosParam->face_offset_norm.z)<EPS)
		{
			cFaceOffset='X';
			if(fabs(vWingNorm.z)>fabs(vWingNorm.y))
				cCurrWingNorm='Y';
			else
				cCurrWingNorm='Z';
		}
		else
			cFaceOffset='?';
		if(pPosParam->cFaceOffsetDatumLine!=cCurrAngleWing)
			itemText.Printf("%C����",cCurrWingNorm);
		else
		{
			char cMinCoord=min(cFaceOffset,cCurrWingNorm),cMaxCoord=max(cFaceOffset,cCurrWingNorm);
			if(cMinCoord=='?')
			{
				cMinCoord=cMaxCoord;
				cMaxCoord='?';
			}
			itemText.Printf("%C,%C����",cMinCoord,cMaxCoord);
		}
	}
	if(valueStr)
		StrCopy(valueStr,itemText,nMaxStrBufLen);
	return min(itemText.GetLength(),(int)(nMaxStrBufLen-1));
}
//�˺���Ϊ��̬����,��Ҫ��Ϊ������ϽǸֹ���
int CLDSLineAngle::GetPropValueStr(CLDSLineAngle* pLineAngle,long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[500]="";
	CXhString szText(sText,500);
	bool bContinueJustify=false;
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",pLineAngle->handle);
	else if(GetPropID("group_father_jg_h")==id)
		sprintf(sText,"0X%X",pLineAngle->group_father_jg_h);
	else if(GetPropID("layer")==id)
		strcpy(sText,pLineAngle->layer());
	else if(GetPropID("cMaterial")==id)
	{
		char matStr[20];
		if(QuerySteelMatMark(pLineAngle->cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(isalpha(pLineAngle->m_cQualityLevel))
			sprintf(sText,"%C",pLineAngle->m_cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"Ĭ��");
#endif
	}
	else if(GetPropID("specification")==id)
		pLineAngle->GetAngleSimpleSpec(sText);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",pLineAngle->m_hPartWeldParent);
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else
		if(pLineAngle->m_bUserSpecColor)
			sprintf(sText,"��");
		else
			sprintf(sText,"��");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",pLineAngle->crMaterial);
	else if(GetPropID("hStart")==id)
	{
		if(pLineAngle->pStart)
			sprintf(sText,"0X%X",pLineAngle->pStart->handle);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("hEnd")==id)
	{
		if(pLineAngle->pEnd)
			sprintf(sText,"0X%X",pLineAngle->pEnd->handle);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("startPos.x")==id)
	{
		sprintf(sText,"%f",pLineAngle->Start().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("startPos.y")==id)
	{
		sprintf(sText,"%f",pLineAngle->Start().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("startPos.z")==id)
	{
		sprintf(sText,"%f",pLineAngle->Start().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("endPos.x")==id)
	{
		sprintf(sText,"%f",pLineAngle->End().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("endPos.y")==id)
	{
		sprintf(sText,"%f",pLineAngle->End().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("endPos.z")==id)
	{
		sprintf(sText,"%f",pLineAngle->End().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.jgber_cal_style")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.jgber_cal_style==0)
			strcpy(sText,"Parametric");
		else if(pLineAngle->desStartPos.jgber_cal_style==1)
			strcpy(sText,"Specify");
		else //if(pLineAngle->desStartPos.jgber_cal_style==2)
			strcpy(sText,"Simple");
#else
		if(pLineAngle->desStartPos.jgber_cal_style==0)
			strcpy(sText,"��������λ");
		else if(pLineAngle->desStartPos.jgber_cal_style==1)
			strcpy(sText,"ָ������");
		else //if(pLineAngle->desStartPos.jgber_cal_style==2)
			strcpy(sText,"�򵥶�λ");
#endif
	}
	else if(GetPropID("desStartPos.SimpleDestLineMode")==id)
	{
		BYTE ciDestLineMode=pLineAngle->desStartPos.ciDestLineMode;
		if(pLineAngle->desStartPos.IsInHuoQuLine())
			ciDestLineMode=CMultiOffsetPos::TO_BENDLINE;
		else if(pLineAngle->desStartPos.IsInOffsetZhunLine())
			ciDestLineMode=CMultiOffsetPos::TO_OFFSETZHUN;
		else if(pLineAngle->desStartPos.IsInDatumJgBer())
			ciDestLineMode=CMultiOffsetPos::TO_BERLINE;
		else
			ciDestLineMode=CMultiOffsetPos::TO_AXISLINE;
#ifdef AFX_TARG_ENU_ENGLISH
			if(ciDestLineMode==CMultiOffsetPos::TO_BERLINE)
				strcpy(sText,"Ridge line");
			else if(ciDestLineMode==CMultiOffsetPos::TO_AXISLINE)
				strcpy(sText,"Axis line");
			else if(ciDestLineMode==CMultiOffsetPos::TO_BENDLINE)
				strcpy(sText,"Bend line");
			else if(ciDestLineMode==CMultiOffsetPos::TO_OFFSETZHUN)
				strcpy(sText,"Offset axis");
#else
		if(ciDestLineMode==CMultiOffsetPos::TO_BERLINE)
				strcpy(sText,"��������");
			else if(ciDestLineMode==CMultiOffsetPos::TO_AXISLINE)
				strcpy(sText,"��������");
			else if(ciDestLineMode==CMultiOffsetPos::TO_BENDLINE)
				strcpy(sText,"���ڻ�����");
			else if(ciDestLineMode==CMultiOffsetPos::TO_OFFSETZHUN)
				strcpy(sText,"����ƫ��׼��");
#endif
	}
	else if(GetPropID("desStartPos.SimpleWingXTipCtrlPoint")==id)
		return GetAngleCtrlPointParamStr(&pLineAngle->desStartPos,'X',pLineAngle->get_norm_x_wing(),valueStr,nMaxStrBufLen);
	else if(GetPropID("desStartPos.SimpleWingYTipCtrlPoint")==id)
		return GetAngleCtrlPointParamStr(&pLineAngle->desStartPos,'Y',pLineAngle->get_norm_y_wing(),valueStr,nMaxStrBufLen);
	else if(GetPropID("desEndPos.SimpleDestLineMode")==id)
	{
		BYTE ciDestLineMode=pLineAngle->desEndPos.ciDestLineMode;
		if(pLineAngle->desEndPos.IsInHuoQuLine())
			ciDestLineMode=CMultiOffsetPos::TO_BENDLINE;
		else if(pLineAngle->desEndPos.IsInOffsetZhunLine())
			ciDestLineMode=CMultiOffsetPos::TO_OFFSETZHUN;
		else if(pLineAngle->desEndPos.IsInDatumJgBer())
			ciDestLineMode=CMultiOffsetPos::TO_BERLINE;
		else
			ciDestLineMode=CMultiOffsetPos::TO_AXISLINE;
#ifdef AFX_TARG_ENU_ENGLISH
			if(ciDestLineMode==CMultiOffsetPos::TO_BERLINE)
				strcpy(sText,"Ridge line");
			else if(ciDestLineMode==CMultiOffsetPos::TO_AXISLINE)
				strcpy(sText,"Axis line");
			else if(ciDestLineMode==CMultiOffsetPos::TO_BENDLINE)
				strcpy(sText,"Bend line");
			else if(ciDestLineMode==CMultiOffsetPos::TO_OFFSETZHUN)
				strcpy(sText,"Offset axis");
#else
			if(ciDestLineMode==CMultiOffsetPos::TO_BERLINE)
				strcpy(sText,"��������");
			else if(ciDestLineMode==CMultiOffsetPos::TO_AXISLINE)
				strcpy(sText,"��������");
			else if(ciDestLineMode==CMultiOffsetPos::TO_BENDLINE)
				strcpy(sText,"���ڻ�����");
			else if(ciDestLineMode==CMultiOffsetPos::TO_OFFSETZHUN)
				strcpy(sText,"����ƫ��׼��");
#endif
	}
	else if(GetPropID("desEndPos.SimpleWingXTipCtrlPoint")==id)
		return GetAngleCtrlPointParamStr(&pLineAngle->desEndPos,'X',pLineAngle->get_norm_x_wing(),valueStr,nMaxStrBufLen);
	else if(GetPropID("desEndPos.SimpleWingYTipCtrlPoint")==id)
		return GetAngleCtrlPointParamStr(&pLineAngle->desEndPos,'Y',pLineAngle->get_norm_y_wing(),valueStr,nMaxStrBufLen);
	else if(GetPropID("desStartPos.origin_pos_style")==id)
	{
		if(pLineAngle->desStartPos.origin_pos_style==0)
		{
			CSuperSmartPtr<CLDSLinePart>pDatumRod = pLineAngle->GetStartDatumPart();
			if(pLineAngle->IsLappedAnchorStart()&&pDatumRod.IsHasPtr()&&pDatumRod->IsAngle()&&
				(pDatumRod.LineAnglePointer()->m_cPosCtrlDatumLineType=='X'||pDatumRod.LineAnglePointer()->m_cPosCtrlDatumLineType=='Y'))
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"2.Datum Angle Key Point");
			else
				strcpy(sText,"Node Position");
#else
				strcpy(sText,"2.��׼�Ǹֹؼ���");
			else
				strcpy(sText,"�ڵ�λ��");
#endif
		}
#ifdef AFX_TARG_ENU_ENGLISH
		else if(pLineAngle->desStartPos.origin_pos_style==1)
			strcpy(sText,"1.Datum Point");
		else if(pLineAngle->desStartPos.origin_pos_style==2)
			strcpy(sText,"2.Datum Angle Key Point");
		else if(pLineAngle->desStartPos.origin_pos_style==3)
			strcpy(sText,"3.Another Node Coordinate");
		else if(pLineAngle->desStartPos.origin_pos_style==4)
			strcpy(sText,"4.Another Ridge Vertex Coordinate");
		else if(pLineAngle->desStartPos.origin_pos_style==5)
			strcpy(sText,"Identify Intelligent");
#else
		else if(pLineAngle->desStartPos.origin_pos_style==1)
			strcpy(sText,"1.��׼��");
		else if(pLineAngle->desStartPos.origin_pos_style==2)
			strcpy(sText,"2.��׼�Ǹֹؼ���");
		else if(pLineAngle->desStartPos.origin_pos_style==3)
			strcpy(sText,"3.��һ�˽ڵ�����");
		else if(pLineAngle->desStartPos.origin_pos_style==4)
			strcpy(sText,"4.��һ���������");
		else if(pLineAngle->desStartPos.origin_pos_style==5)
			strcpy(sText,"����ʶ��");
#endif
	}
	else if(GetPropID("desStartPos.cDatumAngleWing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.cDatumAngleWing=='X')
			strcpy(sText,"X Leg");
		else if(pLineAngle->desStartPos.cDatumAngleWing=='Y')
			strcpy(sText,"Y Leg");
		else
			strcpy(sText,"Ridge Line Point");
#else
		if(pLineAngle->desStartPos.cDatumAngleWing=='X')
			strcpy(sText,"X֫���ߵ�");
		else if(pLineAngle->desStartPos.cDatumAngleWing=='Y')
			strcpy(sText,"Y֫���ߵ�");
		else
			strcpy(sText,"���ߵ�");
#endif
	}
	else if(GetPropID("desStartPos.datumWingOffset.operStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.datumWingOffset.operStyle==0)
			strcpy(sText,"0.Project");
		else if(pLineAngle->desStartPos.datumWingOffset.operStyle==1)
			strcpy(sText,"1.X-Y Plane Offset");
		else if(pLineAngle->desStartPos.datumWingOffset.operStyle==2)
			strcpy(sText,"2.Y-Z Plane Offset");
		else if(pLineAngle->desStartPos.datumWingOffset.operStyle==3)
			strcpy(sText,"3.X-Z Plane Offset");
#else
		if(pLineAngle->desStartPos.datumWingOffset.operStyle==0)
			strcpy(sText,"0.����ͶӰ");
		else if(pLineAngle->desStartPos.datumWingOffset.operStyle==1)
			strcpy(sText,"1.X-Yƽ��ƽ��");
		else if(pLineAngle->desStartPos.datumWingOffset.operStyle==2)
			strcpy(sText,"2.Y-Zƽ��ƽ��");
		else if(pLineAngle->desStartPos.datumWingOffset.operStyle==3)
			strcpy(sText,"3.X-Zƽ��ƽ��");
#endif
	}
	else if(GetPropID("desStartPos.datumWingOffset.gStyle")==id)
	{
		if(pLineAngle->desStartPos.datumWingOffset.gStyle==0)
			strcpy(sText,"g");
		else if(pLineAngle->desStartPos.datumWingOffset.gStyle==1)
			strcpy(sText,"g1");
		else if(pLineAngle->desStartPos.datumWingOffset.gStyle==2)
			strcpy(sText,"g2");
		else if(pLineAngle->desStartPos.datumWingOffset.gStyle==3)
			strcpy(sText,"g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desStartPos.datumWingOffset.offsetDist")==id)
	{
		if(pLineAngle->desStartPos.datumWingOffset.gStyle<4)
		{
			CSuperSmartPtr<CLDSLinePart> pDatumRod=pLineAngle->GetStartDatumPart();
			if(pDatumRod.IsHasPtr()&&pDatumRod->IsAngle()&&pLineAngle->desStartPos.cDatumAngleWing=='X')
				pLineAngle->desStartPos.datumWingOffset.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingXG(pLineAngle->desStartPos.datumWingOffset.gStyle);
			else if(pDatumRod.IsHasPtr()&&pDatumRod->IsAngle()&&pLineAngle->desStartPos.cDatumAngleWing=='Y')
				pLineAngle->desStartPos.datumWingOffset.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingYG(pLineAngle->desStartPos.datumWingOffset.gStyle);
			else
				pLineAngle->desStartPos.datumWingOffset.offsetDist=0;
		}
		sprintf(sText,"%f",pLineAngle->desStartPos.datumWingOffset.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.datum_jg_h")==id||GetPropID("desStartPos.SimpleDependRod")==id)
		sprintf(sText,"0X%X",pLineAngle->desStartPos.datum_jg_h);
	else if(GetPropID("desStartPos.spatialOperationStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.spatialOperationStyle==0)
			strcpy(sText,"Specify Datum Node");
		else if(pLineAngle->desStartPos.spatialOperationStyle==1)
			strcpy(sText,"Ridge Project");
		else if(pLineAngle->desStartPos.spatialOperationStyle==2)
			strcpy(sText,"X Leg Project");
		else if(pLineAngle->desStartPos.spatialOperationStyle==3)
			strcpy(sText,"Y Leg Project");
		else if(pLineAngle->desStartPos.spatialOperationStyle==4)
			strcpy(sText,"Ridge Plane Offset");
		else if(pLineAngle->desStartPos.spatialOperationStyle==5)
			strcpy(sText,"X Leg Plane Offset");
		else if(pLineAngle->desStartPos.spatialOperationStyle==6)
			strcpy(sText,"Y Leg Plane Offset");
		else if(pLineAngle->desStartPos.spatialOperationStyle==7)
			strcpy(sText,"Ridge Bend Line Project");
		else if(pLineAngle->desStartPos.spatialOperationStyle==8)
			strcpy(sText,"X Leg Bend Line Project");
		else if(pLineAngle->desStartPos.spatialOperationStyle==9)
			strcpy(sText,"Y Leg Bend Line Project");
		else if(pLineAngle->desStartPos.spatialOperationStyle==10)
			strcpy(sText,"Ridge Bend Line Plane Offset");
		else if(pLineAngle->desStartPos.spatialOperationStyle==11)
			strcpy(sText,"X Leg Bend Line Plane Offset");
		else if(pLineAngle->desStartPos.spatialOperationStyle==12)
			strcpy(sText,"Y Leg Bend Line Plane Offset");
		else if(pLineAngle->desStartPos.spatialOperationStyle==13)
			strcpy(sText,"X Leg Extend Line");
		else if(pLineAngle->desStartPos.spatialOperationStyle==14)
			strcpy(sText,"Y Leg Extend Line");
		else if(pLineAngle->desStartPos.spatialOperationStyle==15)
			strcpy(sText,"Legs Coordinate Control");
		else if(pLineAngle->desStartPos.spatialOperationStyle==16)
			strcpy(sText,"Project To Line");
		else if(pLineAngle->desStartPos.spatialOperationStyle==17)
			strcpy(sText,"Plane Offset To Line");
		else if(pLineAngle->desStartPos.spatialOperationStyle==18)
			strcpy(sText,"Node Project");
		else if(pLineAngle->desStartPos.spatialOperationStyle==19)
			strcpy(sText,"Node Plane Offset");
#else
		if(pLineAngle->desStartPos.spatialOperationStyle==0)
			strcpy(sText,"ָ����׼��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==1)
			strcpy(sText,"����(��)��ͶӰ");
		else if(pLineAngle->desStartPos.spatialOperationStyle==2)
			strcpy(sText,"X֫����ͶӰ");
		else if(pLineAngle->desStartPos.spatialOperationStyle==3)
			strcpy(sText,"Y֫����ͶӰ");
		else if(pLineAngle->desStartPos.spatialOperationStyle==4)
			strcpy(sText,"������ƽ��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==5)
			strcpy(sText,"X֫����ƽ��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==6)
			strcpy(sText,"Y֫����ƽ��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==7)
			strcpy(sText,"�����߻�����ͶӰ");
		else if(pLineAngle->desStartPos.spatialOperationStyle==8)
			strcpy(sText,"X֫���߻�����ͶӰ");
		else if(pLineAngle->desStartPos.spatialOperationStyle==9)
			strcpy(sText,"Y֫���߻�����ͶӰ");
		else if(pLineAngle->desStartPos.spatialOperationStyle==10)
			strcpy(sText,"�����߻�����ƽ��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==11)
			strcpy(sText,"X֫���߻�����ƽ��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==12)
			strcpy(sText,"Y֫���߻�����ƽ��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==13)
			strcpy(sText,"X֫ƫ��׼��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==14)
			strcpy(sText,"Y֫ƫ��׼��");
		else if(pLineAngle->desStartPos.spatialOperationStyle==15)
#ifndef __NEW_ANGLE_LOCA_MODE_
			strcpy(sText,"��֫�������");
#else
			strcpy(sText,"�ڵ��������");
#endif
		else if(pLineAngle->desStartPos.spatialOperationStyle==16)
			strcpy(sText,"ָ����ͶӰ");
		else if(pLineAngle->desStartPos.spatialOperationStyle==17)
			strcpy(sText,"ָ����ƽ��");
#ifndef __NEW_ANGLE_LOCA_MODE_
		else if(pLineAngle->desStartPos.spatialOperationStyle==18)
			strcpy(sText,"������Ƶ�ͶӰ");
		else if(pLineAngle->desStartPos.spatialOperationStyle==19)
			strcpy(sText,"������Ƶ�ƽ��");
#else
		else if (pLineAngle->desStartPos.spatialOperationStyle == 18|| pLineAngle->desStartPos.spatialOperationStyle == 19)
			strcpy(sText, "�ڵ��������");
#endif
#endif
	}
	else if(GetPropID("desStartPos.datum_offset_dist")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.datum_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.len_offset_type")==id||GetPropID("desStartPos.SimpleLenOffsetType")==id)
	{
		if(pLineAngle->desStartPos.len_offset_type==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Length Extend");
		else if(pLineAngle->desStartPos.len_offset_type==1)
			strcpy(sText,"1.Parallel Offset");
		else if(pLineAngle->desStartPos.len_offset_type=='X')
			strcpy(sText,"2.X");
		else if(pLineAngle->desStartPos.len_offset_type=='Y')
			strcpy(sText,"3.Y");
		else if(pLineAngle->desStartPos.len_offset_type=='Z')
			strcpy(sText,"4.Z");
#else
			strcpy(sText,"0.��������");
		else if(pLineAngle->desStartPos.len_offset_type==1)
			strcpy(sText,"1.����ƽ��ƫ��");
		else if(pLineAngle->desStartPos.len_offset_type=='X')
			strcpy(sText,"2.X�������");
		else if(pLineAngle->desStartPos.len_offset_type=='Y')
			strcpy(sText,"3.Y�������");
		else if(pLineAngle->desStartPos.len_offset_type=='Z')
			strcpy(sText,"4.Z�������");
#endif
	}
	else if(GetPropID("desStartPos.len_offset_dist")==id||GetPropID("desStartPos.SimpleLenOffset")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.SimpleNormOffset")==id)
	{
		char cSelfWorkWing=pLineAngle->GetSelfWorkWing(TRUE,NULL);
		double offsetDist=pLineAngle->desStartPos.wing_x_offset.offsetDist;
		if(cSelfWorkWing=='Y')
			offsetDist=pLineAngle->desStartPos.wing_y_offset.offsetDist;
		CLDSLinePart* pFatherRod=pLineAngle->GetStartDatumPart();
		if(pFatherRod&&pFatherRod->IsAngle())
		{
			CLDSLineAngle* pFatherAngle=(CLDSLineAngle*)pFatherRod;
			GEPOINT vCurrWingNorm =cSelfWorkWing=='X'?pLineAngle->vxWingNorm:pLineAngle->vyWingNorm;
			char cDatumAngleWing=pLineAngle->GetLappedWorkWing(TRUE,vCurrWingNorm);
			GEPOINT vDatumWingNorm=cDatumAngleWing=='X'?pFatherAngle->vxWingNorm:pFatherAngle->vyWingNorm;
			if(vCurrWingNorm*vDatumWingNorm<0)
				offsetDist*=-1;
		}
		if(fabs(offsetDist)<0.0001)
			offsetDist=0;
		sprintf(sText,"%f",offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.offset_zhun")==id||GetPropID("desStartPos.SimpleOffsetZhunju")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.offset_zhun);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.huoqu_jg_h")==id||GetPropID("desStartPos.SimpleBendLineAlongAngle")==id)
		sprintf(sText,"0X%X",pLineAngle->desStartPos.huoqu_jg_h);
	else if(GetPropID("desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2")==id||GetPropID("desStartPos.SimpleBendAngleWing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2==1)
			strcpy(sText,"X Leg");
		else if(pLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2==2)
			strcpy(sText,"Y Leg");
		else
			strcpy(sText,"Default");
#else
		if(pLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2==1)
			strcpy(sText,"1.X֫");
		else if(pLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2==2)
			strcpy(sText,"2.Y֫");
		else
			strcpy(sText,"0.�Զ��ж�");
#endif
	}
	else if(GetPropID("desStartPos.huoqu_dist")==id||GetPropID("desStartPos.SimpleBendClearance")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.huoqu_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.cMainCoordCtrlWing")==id)
	{	//��������֫
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.cMainCoordCtrlWing=='X')
			strcpy(sText,"X Leg");
		else if(pLineAngle->desStartPos.cMainCoordCtrlWing=='Y')
			strcpy(sText,"Y Leg");
		else //if(pLineAngle->desStartPos.cMainCoordCtrlWing==1)
			strcpy(sText,"Projection on dual leg");
#else
		if(pLineAngle->desStartPos.spatialOperationStyle==18)
			strcpy(sText,"ͶӰ���ߵ�");
		else if (pLineAngle->desStartPos.spatialOperationStyle == 19)
		{
			if (pLineAngle->desStartPos.cFaceOffsetDatumLine == 'X')
				strcpy(sText, "X֫���ߵ�");
			else if (pLineAngle->desStartPos.cFaceOffsetDatumLine == 'Y')
				strcpy(sText, "Y֫���ߵ�");
			else
				strcpy(sText, "ƽ�����ߵ�");
		}
		else if(pLineAngle->desStartPos.cMainCoordCtrlWing=='X')
			strcpy(sText,"X֫���ߵ�");
		else if(pLineAngle->desStartPos.cMainCoordCtrlWing=='Y')
			strcpy(sText,"Y֫���ߵ�");
		else //if(pLineAngle->desStartPos.cMainCoordCtrlWing==1)
			strcpy(sText,"˫֫ͶӰ��");
#endif
	}
	else if(GetPropID("desStartPos.coordCtrlType")==id)
	{	//����֫�����������
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.coordCtrlType==0)
			strcpy(sText,"0.X,Z");
		else if(pLineAngle->desStartPos.coordCtrlType==1)
			strcpy(sText,"1:Y,Z");
		else if(pLineAngle->desStartPos.coordCtrlType==1)
			strcpy(sText,"2.X,Y");
#else
		if(pLineAngle->desStartPos.coordCtrlType==0)
			strcpy(sText,"0.X,Z����");
		else if(pLineAngle->desStartPos.coordCtrlType==1)
			strcpy(sText,"1:Y,Z����");
		else if(pLineAngle->desStartPos.coordCtrlType==1)
			strcpy(sText,"2.X,Y����");
#endif
	}
	else if(GetPropID("desStartPos.ctrlWing.operStyle")==id)
	{	//����֫�ؼ��㶨λ��ʽ
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.ctrlWing.operStyle==0)
			strcpy(sText,"0.Project");
		else if(pLineAngle->desStartPos.ctrlWing.operStyle==1)
			strcpy(sText,"1.X-Y Plane Offset");
		else if(pLineAngle->desStartPos.ctrlWing.operStyle==2)
			strcpy(sText,"2.Y-Z Plane Offset");
		else if(pLineAngle->desStartPos.ctrlWing.operStyle==3)
			strcpy(sText,"3.X-Z Plane Offset");
#else
		if(pLineAngle->desStartPos.ctrlWing.operStyle==0)
			strcpy(sText,"0.����ͶӰ");
		else if(pLineAngle->desStartPos.ctrlWing.operStyle==1)
			strcpy(sText,"1.X-Yƽ��ƽ��");
		else if(pLineAngle->desStartPos.ctrlWing.operStyle==2)
			strcpy(sText,"2.Y-Zƽ��ƽ��");
		else if(pLineAngle->desStartPos.ctrlWing.operStyle==3)
			strcpy(sText,"3.X-Zƽ��ƽ��");
#endif
	}
	else if (GetPropID("desStartPos.ctrlWing.gStyle") == id)
	{	//����֫�ؼ��㶨λ��ʽ
		int giStyle = pLineAngle->desStartPos.xMasterWing.gStyle;
		if (giStyle == 0)
			strcpy(sText, "g");
		else if (giStyle == 1)
			strcpy(sText, "g1");
		else if (giStyle == 2)
			strcpy(sText, "g2");
		else if (giStyle == 3)
			strcpy(sText, "g3");
		else if (giStyle == 4)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText, "User-Defined");
#else
			strcpy(sText, "�Զ���");
#endif
	}
	else if(GetPropID("desStartPos.ctrlWing.offsetDist")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		if(pLineAngle->desStartPos.xMasterWing.gStyle<4)
		{
			CSuperSmartPtr<CLDSLinePart> pDatumRod=pLineAngle->GetStartDatumPart();
			if(pDatumRod.IsNULL())
				pLineAngle->Log2File()->Log("0X%X�Ǹ�ʼ�˶�λ�����˼�δ�ҵ�!",pLineAngle->handle);
			else if(pDatumRod->IsAngle()&&pLineAngle->desStartPos.cMainCoordCtrlWing=='X')
				pLineAngle->desStartPos.xMasterWing.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingXG(pLineAngle->desStartPos.xMasterWing.gStyle);
			else if(pDatumRod->IsAngle()&&pLineAngle->desStartPos.cMainCoordCtrlWing=='Y')
				pLineAngle->desStartPos.xMasterWing.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingYG(pLineAngle->desStartPos.xMasterWing.gStyle);
			else
				pLineAngle->desStartPos.xMasterWing.offsetDist=0;
		}
		sprintf(sText,"%f",pLineAngle->desStartPos.xMasterWing.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.otherWing.operStyle")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		if (pLineAngle->desStartPos.spatialOperationStyle == 18)
			strcpy(sText, "0.����ͶӰ");
		else if (pLineAngle->desStartPos.spatialOperationStyle == 19)
			strcpy(sText, "1.X-Yƽ��ƽ��");
		else if (pLineAngle->desStartPos.otherWing.operStyle == 0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Project");
		else if(pLineAngle->desStartPos.otherWing.operStyle==1)
			strcpy(sText,"1.X-Y Plane Offset");
		else if(pLineAngle->desStartPos.otherWing.operStyle==2)
			strcpy(sText,"2.Y-Z Plane Offset");
		else if(pLineAngle->desStartPos.otherWing.operStyle==3)
			strcpy(sText,"3.X-Z Plane Offset");
#else
			strcpy(sText,"0.����ͶӰ");
		else if(pLineAngle->desStartPos.otherWing.operStyle==1)
			strcpy(sText,"1.X-Yƽ��ƽ��");
		else if(pLineAngle->desStartPos.otherWing.operStyle==2)
			strcpy(sText,"2.Y-Zƽ��ƽ��");
		else if(pLineAngle->desStartPos.otherWing.operStyle==3)
			strcpy(sText,"3.X-Zƽ��ƽ��");
#endif
	}
	else if(GetPropID("desStartPos.otherWing.gStyle")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		int giStyle = pLineAngle->desStartPos.xSlaveWing.gStyle;//otherWing.gStyle;
		if (giStyle == 0)
			strcpy(sText, "g");
		else if (giStyle == 1)
			strcpy(sText, "g1");
		else if (giStyle == 2)
			strcpy(sText, "g2");
		else if (giStyle == 3)
			strcpy(sText, "g3");
		else if (giStyle == 4)
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(sText, "User-Defined");
#else
		strcpy(sText, "�Զ���");
#endif
	}
	else if(GetPropID("desStartPos.otherWing.offsetDist")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		if(pLineAngle->desStartPos.xSlaveWing.gStyle<4)
		{
			CSuperSmartPtr<CLDSLinePart> pDatumRod=pLineAngle->GetStartDatumPart();
			if(pDatumRod.IsNULL())
				pLineAngle->Log2File()->Log("0X%X�Ǹ�ʼ�˶�λ�����˼�δ�ҵ�!",pLineAngle->handle);
			else if(pDatumRod->IsAngle()&&pLineAngle->desStartPos.cMainCoordCtrlWing=='Y')
				pLineAngle->desStartPos.xSlaveWing.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingXG(pLineAngle->desStartPos.xSlaveWing.gStyle);
			else if(pDatumRod->IsAngle()&&pLineAngle->desStartPos.cMainCoordCtrlWing=='X')
				pLineAngle->desStartPos.xSlaveWing.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingYG(pLineAngle->desStartPos.xSlaveWing.gStyle);
			else
				pLineAngle->desStartPos.xSlaveWing.offsetDist=0;
		}
		sprintf(sText,"%f",pLineAngle->desStartPos.xSlaveWing.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.fLenOffsetOfCoordWing")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.fLenOffsetOfCoordWing);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.datumPoint.x")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.datumPoint.Position().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.datumPoint.y")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.datumPoint.Position().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.datumPoint.z")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.datumPoint.Position().z);
		SimplifiedNumString(sText);
	}
	/*else if(GetPropID("pOldDesStartPos->start_jg_h")==id)
		sprintf(sText,"0X%X",pLineAngle->pOldDesStartPos->start_jg_h);
	else if(GetPropID("pOldDesStartPos->iStartJgBerStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->pOldDesStartPos->iStartJgBerStyle==0)
			strcpy(sText,"Start Ridge Vertex");
		else
			strcpy(sText,"End Ridge Vertex");
#else
		if(pLineAngle->pOldDesStartPos->iStartJgBerStyle==0)
			strcpy(sText,"ʼ�����");
		else
			strcpy(sText,"�ն����");
#endif
	}
	else if(GetPropID("pOldDesStartPos->fStartJgLengOffsetX")==id)
	{
		sprintf(sText,"%f",pLineAngle->pOldDesStartPos->fStartJgLengOffsetX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pOldDesStartPos->fStartJgLengOffsetY")==id)
	{
		sprintf(sText,"%f",pLineAngle->pOldDesStartPos->fStartJgLengOffsetY);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pOldDesStartPos->end_jg_h")==id)
		sprintf(sText,"0X%X",pLineAngle->pOldDesStartPos->end_jg_h);
	else if(GetPropID("pOldDesStartPos->iEndJgBerStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->pOldDesStartPos->iEndJgBerStyle==0)
			strcpy(sText,"Start Ridge Vertex");
		else
			strcpy(sText,"End Ridge Vertex");
#else
		if(pLineAngle->pOldDesStartPos->iEndJgBerStyle==0)
			strcpy(sText,"ʼ�����");
		else
			strcpy(sText,"�ն����");
#endif
	}
	else if(GetPropID("pOldDesStartPos->fEndJgLengOffsetX")==id)
	{
		sprintf(sText,"%f",pLineAngle->pOldDesStartPos->fEndJgLengOffsetX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pOldDesStartPos->fEndJgLengOffsetY")==id)
	{
		sprintf(sText,"%f",pLineAngle->pOldDesStartPos->fEndJgLengOffsetY);
		SimplifiedNumString(sText);
	}*/
	else if(GetPropID("desStartPos.cFaceOffsetDatumLine")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.cFaceOffsetDatumLine=='X')
			strcpy(sText,"X Leg");
		else if(pLineAngle->desStartPos.cFaceOffsetDatumLine=='Y')
			strcpy(sText,"Y Leg");
		else
			strcpy(sText,"Ridge Line");
#else
		if(pLineAngle->desStartPos.cFaceOffsetDatumLine=='X')
			strcpy(sText,"X֫����");
		else if(pLineAngle->desStartPos.cFaceOffsetDatumLine=='Y')
			strcpy(sText,"Y֫����");
		else
			strcpy(sText,"����");
#endif
	}
	else if(GetPropID("desStartPos.face_offset_norm")==id)
	{
		double x=fabs(pLineAngle->desStartPos.face_offset_norm.x);
		double y=fabs(pLineAngle->desStartPos.face_offset_norm.y);
		double z=fabs(pLineAngle->desStartPos.face_offset_norm.z);
#ifdef AFX_TARG_ENU_ENGLISH
		if(x>EPS&&y<EPS&&z<EPS)
			strcpy(sText,"Y-Z Plane");
		else if(x<EPS&&y>EPS&&z<EPS)
			strcpy(sText,"X-Z Plane");
		else if(x<EPS&&y<EPS&&z>EPS)
			strcpy(sText,"X-Y Plane");
		else
			strcpy(sText,"User-Defined");
#else
		if(x>EPS&&y<EPS&&z<EPS)
			strcpy(sText,"Y-Zƽ��");
		else if(x<EPS&&y>EPS&&z<EPS)
			strcpy(sText,"X-Zƽ��");
		else if(x<EPS&&y<EPS&&z>EPS)
			strcpy(sText,"X-Yƽ��");
		else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desStartPos.face_offset_norm.x")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.face_offset_norm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.face_offset_norm.y")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.face_offset_norm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.face_offset_norm.z")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.face_offset_norm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.cWorkWingStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->ciStartAnchorWing==0)//desStartPos.cWorkWingStyle==0)
			strcpy(sText,"Default");
		else if(pLineAngle->ciStartAnchorWing==1)//desStartPos.cWorkWingStyle==1)
			strcpy(sText,"X Leg");
		else if(pLineAngle->ciStartAnchorWing==2)//desStartPos.cWorkWingStyle==2)
			strcpy(sText,"Y Leg");
		else if(pLineAngle->ciStartAnchorWing==3)//desStartPos.cWorkWingStyle==3)
			strcpy(sText,"X&Y Leg");
#else
		if(pLineAngle->ciStartAnchorWing==0)//desStartPos.cWorkWingStyle==0)
			strcpy(sText,"�����ж�");
		else if(pLineAngle->ciStartAnchorWing==1)//desStartPos.cWorkWingStyle==1)
			strcpy(sText,"X֫��λ");
		else if(pLineAngle->ciStartAnchorWing==2)//desStartPos.cWorkWingStyle==2)
			strcpy(sText,"Y֫��λ");
		else if(pLineAngle->ciStartAnchorWing==3)//desStartPos.cWorkWingStyle==3)
			strcpy(sText,"˫֫��λ");
#endif
	}
	else if(GetPropID("desStartPos.wing_x_offset.gStyle")==id)
	{
		if(pLineAngle->desStartPos.wing_x_offset.gStyle==0)
			strcpy(sText,"g");
		else if(pLineAngle->desStartPos.wing_x_offset.gStyle==1)
			strcpy(sText,"g1");
		else if(pLineAngle->desStartPos.wing_x_offset.gStyle==2)
			strcpy(sText,"g2");
		else if(pLineAngle->desStartPos.wing_x_offset.gStyle==3)
			strcpy(sText,"g3");
		else if(pLineAngle->desStartPos.wing_x_offset.gStyle==4)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desStartPos.wing_x_offset.offsetDist")==id)
	{
		pLineAngle->desStartPos.wing_x_offset.offsetDist = pLineAngle->GetStartWingNormOffset(0);
		sprintf(sText,"%f",pLineAngle->desStartPos.wing_x_offset.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.wing_y_offset.gStyle")==id)
	{
		if(pLineAngle->desStartPos.wing_y_offset.gStyle==0)
			strcpy(sText,"g");
		else if(pLineAngle->desStartPos.wing_y_offset.gStyle==1)
			strcpy(sText,"g1");
		else if(pLineAngle->desStartPos.wing_y_offset.gStyle==2)
			strcpy(sText,"g2");
		else if(pLineAngle->desStartPos.wing_y_offset.gStyle==3)
			strcpy(sText,"g3");
		else if(pLineAngle->desStartPos.wing_y_offset.gStyle==4)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desStartPos.wing_y_offset.offsetDist")==id)
	{
		pLineAngle->desStartPos.wing_y_offset.offsetDist = pLineAngle->GetStartWingNormOffset(1);
		sprintf(sText,"%f",pLineAngle->desStartPos.wing_y_offset.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.fEccentricDist")==id)
	{	//ƫ�ľ�
		sprintf(sText,"%f",pLineAngle->desStartPos.fEccentricDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.hDatumLine")==id)
	{	
		if(pLineAngle->desStartPos.hDatumLine>0X20)
			sprintf(sText,"0X%X",pLineAngle->desStartPos.hDatumLine);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"<Local Define...>");
#else
			strcpy(sText,"<�ֲ�����...>");
#endif
	}
	else if(GetPropID("desStartPos.cMirTransType")==id)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.cMirTransType=='X')
			strcpy(sText,"1.X Symmetry");
		else if(pLineAngle->desStartPos.cMirTransType=='Y')
			strcpy(sText,"2.Y Symmetry");
		else if(pLineAngle->desStartPos.cMirTransType=='Z')
			strcpy(sText,"3.Z Symmetry");
		else if(pLineAngle->desStartPos.cMirTransType==0)
			strcpy(sText,"0.None");
#else
		if(pLineAngle->desStartPos.cMirTransType=='X')
			strcpy(sText,"1.X��Գ�");
		else if(pLineAngle->desStartPos.cMirTransType=='Y')
			strcpy(sText,"2.Y��Գ�");
		else if(pLineAngle->desStartPos.cMirTransType=='Z')
			strcpy(sText,"3.Z��Գ�");
		else if(pLineAngle->desStartPos.cMirTransType==0)
			strcpy(sText,"0.��");
#endif
	}
	else if(GetPropID("desStartPos.datumLine.startPos.x")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desStartPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->start_pos.x);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desStartPos.datumLine.startPos.y")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desStartPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->start_pos.y);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desStartPos.datumLine.startPos.z")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desStartPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->start_pos.z);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desStartPos.datumLine.endPos.x")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desStartPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->end_pos.x);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desStartPos.datumLine.endPos.y")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desStartPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->end_pos.y);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desStartPos.datumLine.endPos.z")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desStartPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->end_pos.z);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desEndPos.hDatumLine")==id)
	{
		if(pLineAngle->desEndPos.hDatumLine>0X20)
			sprintf(sText,"0X%X",pLineAngle->desEndPos.hDatumLine);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"<Local Define...>");
#else
			strcpy(sText,"<�ֲ�����...>");
#endif
	}
	else if(GetPropID("desEndPos.cMirTransType")==id)
	{	
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desEndPos.cMirTransType=='X')
			strcpy(sText,"1.X Symmetry");
		else if(pLineAngle->desEndPos.cMirTransType=='Y')
			strcpy(sText,"2.Y Symmetry");
		else if(pLineAngle->desEndPos.cMirTransType=='Z')
			strcpy(sText,"3.Z Symmetry");
		else if(pLineAngle->desEndPos.cMirTransType==0)
			strcpy(sText,"0.None");
#else
		if(pLineAngle->desEndPos.cMirTransType=='X')
			strcpy(sText,"1.X��Գ�");
		else if(pLineAngle->desEndPos.cMirTransType=='Y')
			strcpy(sText,"2.Y��Գ�");
		else if(pLineAngle->desEndPos.cMirTransType=='Z')
			strcpy(sText,"3.Z��Գ�");
		else if(pLineAngle->desEndPos.cMirTransType==0)
			strcpy(sText,"0.��");
#endif
	}
	else if(GetPropID("desEndPos.datumLine.startPos.x")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desEndPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->start_pos.x);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desEndPos.datumLine.startPos.y")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desEndPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->start_pos.y);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desEndPos.datumLine.startPos.z")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desEndPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->start_pos.z);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desEndPos.datumLine.endPos.x")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desEndPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->end_pos.x);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desEndPos.datumLine.endPos.y")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desEndPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->end_pos.y);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desEndPos.datumLine.endPos.z")==id)
	{
		CLDSLine* pDatumLine=pLineAngle->BelongModel()->FromLineHandle(pLineAngle->desEndPos.hDatumLine);
		if(pDatumLine)
		{
			pDatumLine->UpdateLine(pLineAngle->BelongModel());
			sprintf(sText,"%f",pDatumLine->end_pos.z);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("desEndPos.jgber_cal_style")==id)
	{
		if(pLineAngle->desEndPos.jgber_cal_style==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Parametric");
		else if(pLineAngle->desEndPos.jgber_cal_style==1)
			strcpy(sText,"Specify");
		else //if(pLineAngle->desEndPos.jgber_cal_style==2)
			strcpy(sText,"Simple");
#else
			strcpy(sText,"��������λ");
		else if(pLineAngle->desEndPos.jgber_cal_style==1)
			strcpy(sText,"ָ������");
		else //if(pLineAngle->desEndPos.jgber_cal_style==2)
			strcpy(sText,"�򵥶�λ");
#endif
	}
	else if(GetPropID("desStartPos.SimpleDatumPoint")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desStartPos.m_iSimpleDatumPoint==0)
			strcpy(sText,"Specified");
		else if(pLineAngle->desStartPos.m_iSimpleDatumPoint==1)
			strcpy(sText,"By Node");
		else if(pLineAngle->desStartPos.m_iSimpleDatumPoint==2)
			strcpy(sText,"Lapped");
		else //if(pLineAngle->desStartPos.m_iSimpleDatumPoint==3)
			strcpy(sText,"Co-linear");
#else
		if(pLineAngle->desStartPos.m_iSimpleDatumPoint==0)
			strcpy(sText,"ָ������");
		else if(pLineAngle->desStartPos.m_iSimpleDatumPoint==1)
			strcpy(sText,"�ڵ����");
		else if(pLineAngle->desStartPos.m_iSimpleDatumPoint==2)
			strcpy(sText,"��Ӷ�λ");
		else //if(pLineAngle->desStartPos.m_iSimpleDatumPoint==3)
			strcpy(sText,"���߶�λ");
#endif
	}
	else if(GetPropID("desEndPos.SimpleDatumPoint")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desEndPos.m_iSimpleDatumPoint==0)
			strcpy(sText,"Specified");
		else if(pLineAngle->desEndPos.m_iSimpleDatumPoint==1)
			strcpy(sText,"By Node");
		else if(pLineAngle->desEndPos.m_iSimpleDatumPoint==2)
			strcpy(sText,"Lapped");
		else //if(pLineAngle->desEndPos.m_iSimpleDatumPoint==3)
			strcpy(sText,"Co-linear");
#else
		if(pLineAngle->desEndPos.m_iSimpleDatumPoint==0)
			strcpy(sText,"ָ������");
		else if(pLineAngle->desEndPos.m_iSimpleDatumPoint==1)
			strcpy(sText,"�ڵ����");
		else if(pLineAngle->desEndPos.m_iSimpleDatumPoint==2)
			strcpy(sText,"��Ӷ�λ");
		else //if(pLineAngle->desEndPos.m_iSimpleDatumPoint==3)
			strcpy(sText,"���߶�λ");
#endif
	}
	else if (GetPropID("desStartPos.LappedDatumWingG")==id)
	{
		if (pLineAngle->desStartPos.datumWingOffset.gStyle==4)
			szText.Printf("%g",pLineAngle->desStartPos.datumWingOffset.offsetDist);
		else
		{
			SmartRodPtr pParentAngle=pLineAngle->GetStartDatumPart();
			if (pParentAngle.blValid&&pParentAngle->IsAngle())
			{
				double dfSpecG=0;
				if (pLineAngle->desStartPos.IsInDatumJgWingX())
					dfSpecG=pParentAngle.pAngle->GetNodeWingXG(pLineAngle->pStart);	//Ĭ��׼��
				else if (pLineAngle->desStartPos.IsInDatumJgWingY())
					dfSpecG=pParentAngle.pAngle->GetNodeWingYG(pLineAngle->pStart);	//Ĭ��׼��
				szText.Printf("%g",dfSpecG);
			}
		}
	}
	else if(GetPropID("desEndPos.LappedDatumWingG")==id)
	{
		if (pLineAngle->desEndPos.datumWingOffset.gStyle==4)
			szText.Printf("%g",pLineAngle->desEndPos.datumWingOffset.offsetDist);
		else
		{
			SmartRodPtr pParentAngle=pLineAngle->GetEndDatumPart();
			if (pParentAngle.blValid&&pParentAngle->IsAngle())
			{
				double dfSpecG=0;
				if (pLineAngle->desEndPos.IsInDatumJgWingX())
					dfSpecG=pParentAngle.pAngle->GetNodeWingXG(pLineAngle->pEnd);	//Ĭ��׼��
				else if (pLineAngle->desEndPos.IsInDatumJgWingY())
					dfSpecG=pParentAngle.pAngle->GetNodeWingYG(pLineAngle->pEnd);	//Ĭ��׼��
				szText.Printf("%g",dfSpecG);
			}
		}
	}
	else if(GetPropID("desStartPos.SimpleDenpendWing")==id)
	{
		char cDatumAngleWing=pLineAngle->GetLappedWorkWing(TRUE,NULL);
		if(cDatumAngleWing=='X')
			strcpy(sText,"X֫");
		else if(cDatumAngleWing=='Y')
			strcpy(sText,"Y֫");
		else
			strcpy(sText,"�����ж�");
	}
	else if(GetPropID("desEndPos.SimpleDenpendWing")==id)
	{
		char cDatumAngleWing=pLineAngle->GetLappedWorkWing(FALSE,NULL);
		if(cDatumAngleWing=='X')
			strcpy(sText,"X֫");
		else if(cDatumAngleWing=='Y')
			strcpy(sText,"Y֫");
		else
			strcpy(sText,"�����ж�");
	}
	else if(GetPropID("desStartPos.SimpleSelfWorkWing")==id)
	{
		char cSelfWorkWing=pLineAngle->GetSelfWorkWing(TRUE,NULL);
		if(cSelfWorkWing=='X')
			strcpy(sText,"X֫");
		else if(cSelfWorkWing=='Y')
			strcpy(sText,"Y֫");
		else //if(cSelfWorkWing==3)
			strcpy(sText,"˫֫");
	}
	else if(GetPropID("desEndPos.SimpleSelfWorkWing")==id)
	{
		char cSelfWorkWing=pLineAngle->GetSelfWorkWing(FALSE,NULL);
		if(cSelfWorkWing=='X')
			strcpy(sText,"X֫");
		else if(cSelfWorkWing=='Y')
			strcpy(sText,"Y֫");
		else //if(cSelfWorkWing==3)
			strcpy(sText,"˫֫");
	}
	else
		bContinueJustify=TRUE;
	if(!bContinueJustify)
	{	//if-else�ж�̫�����ͨ����ȥ������ֻ�����м�Ͽ�
#ifdef AFX_TARG_ENU_ENGLISH
		if(strlen(sText)>=nMaxStrBufLen)
			pLineAngle->Log2File()->Log("0X%X Angle's property##d string value out of length",pLineAngle->handle,id);
#else
		if(strlen(sText)>=nMaxStrBufLen)
			pLineAngle->Log2File()->Log("�Ǹ�0X%X����#%d�ַ���������������",pLineAngle->handle,id);
#endif
		if(valueStr)
			StrCopy(valueStr,sText,nMaxStrBufLen);
		return strlen(sText);
	}
	bContinueJustify=FALSE;
	if(GetPropID("desEndPos.origin_pos_style")==id)
	{
		if(pLineAngle->desEndPos.origin_pos_style==0)
		{
			CSuperSmartPtr<CLDSLinePart>pDatumRod = pLineAngle->GetEndDatumPart();
			if(pLineAngle->IsLappedAnchorEnd()&&pDatumRod.IsHasPtr()&&pDatumRod->IsAngle()&&
				(pDatumRod.LineAnglePointer()->m_cPosCtrlDatumLineType=='X'||pDatumRod.LineAnglePointer()->m_cPosCtrlDatumLineType=='Y'))
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"2.Datum Angle Key Point");
			else
				strcpy(sText,"Node Position");
#else
				strcpy(sText,"2.��׼�Ǹֹؼ���");
			else
				strcpy(sText,"�ڵ�λ��");
#endif
		}
		else if(pLineAngle->desEndPos.origin_pos_style==1)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"1.Datum Point");
		else if(pLineAngle->desEndPos.origin_pos_style==2)
			strcpy(sText,"2.Datum Angle Key Point");
		else if(pLineAngle->desEndPos.origin_pos_style==3)
			strcpy(sText,"3.Another Node Coordinate");
		else if(pLineAngle->desEndPos.origin_pos_style==4)
			strcpy(sText,"4.Another Ridge Vertex Coordinate");
		else if(pLineAngle->desEndPos.origin_pos_style==5)
			strcpy(sText,"Identify Intelligent");
#else
			strcpy(sText,"1.��׼��");
		else if(pLineAngle->desEndPos.origin_pos_style==2)
			strcpy(sText,"2.��׼�Ǹֹؼ���");
		else if(pLineAngle->desEndPos.origin_pos_style==3)
			strcpy(sText,"3.��һ�˽ڵ�����");
		else if(pLineAngle->desEndPos.origin_pos_style==4)
			strcpy(sText,"4.��һ���������");
		else if(pLineAngle->desEndPos.origin_pos_style==5)
			strcpy(sText,"����ʶ��");
#endif
	}
	else if(GetPropID("desEndPos.cDatumAngleWing")==id)
	{
		if(pLineAngle->desEndPos.cDatumAngleWing=='X')
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else if(pLineAngle->desEndPos.cDatumAngleWing=='Y')
			strcpy(sText,"Y Leg");
		else
			strcpy(sText,"Ridge Line Point");
#else
			strcpy(sText,"X֫���ߵ�");
		else if(pLineAngle->desEndPos.cDatumAngleWing=='Y')
			strcpy(sText,"Y֫���ߵ�");
		else
			strcpy(sText,"���ߵ�");
#endif
	}
	else if(GetPropID("desEndPos.datumWingOffset.operStyle")==id)
	{
		if(pLineAngle->desEndPos.datumWingOffset.operStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Project");
		else if(pLineAngle->desEndPos.datumWingOffset.operStyle==1)
			strcpy(sText,"0.X-Y Plane Offset");
		else if(pLineAngle->desEndPos.datumWingOffset.operStyle==2)
			strcpy(sText,"1.Y-Z Plane Offset");
		else if(pLineAngle->desEndPos.datumWingOffset.operStyle==3)
			strcpy(sText,"2.X-Z Plane Offset");
#else
			strcpy(sText,"����ͶӰ");
		else if(pLineAngle->desEndPos.datumWingOffset.operStyle==1)
			strcpy(sText,"0.X-Yƽ��ƽ��");
		else if(pLineAngle->desEndPos.datumWingOffset.operStyle==2)
			strcpy(sText,"1.Y-Zƽ��ƽ��");
		else if(pLineAngle->desEndPos.datumWingOffset.operStyle==3)
			strcpy(sText,"2.X-Zƽ��ƽ��");
#endif
	}
	else if(GetPropID("desEndPos.datumWingOffset.gStyle")==id)
	{
		if(pLineAngle->desEndPos.datumWingOffset.gStyle==0)
			strcpy(sText,"g");
		else if(pLineAngle->desEndPos.datumWingOffset.gStyle==1)
			strcpy(sText,"g1");
		else if(pLineAngle->desEndPos.datumWingOffset.gStyle==2)
			strcpy(sText,"g2");
		else if(pLineAngle->desEndPos.datumWingOffset.gStyle==3)
			strcpy(sText,"g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desEndPos.datumWingOffset.offsetDist")==id)
	{
		if(pLineAngle->desEndPos.datumWingOffset.gStyle<4)
		{
			CSuperSmartPtr<CLDSLinePart> pDatumRod=pLineAngle->GetEndDatumPart();
			if(pDatumRod.IsHasPtr()&&pDatumRod->IsAngle()&&pLineAngle->desEndPos.cDatumAngleWing=='X')
				pLineAngle->desEndPos.datumWingOffset.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingXG(pLineAngle->desEndPos.datumWingOffset.gStyle);
			else if(pDatumRod.IsHasPtr()&&pDatumRod->IsAngle()&&pLineAngle->desEndPos.cDatumAngleWing=='Y')
				pLineAngle->desEndPos.datumWingOffset.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingYG(pLineAngle->desEndPos.datumWingOffset.gStyle);
			else
				pLineAngle->desEndPos.datumWingOffset.offsetDist=0;
		}
		sprintf(sText,"%f",pLineAngle->desEndPos.datumWingOffset.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.datum_jg_h")==id||GetPropID("desEndPos.SimpleDependRod")==id)
		sprintf(sText,"0X%X",pLineAngle->desEndPos.datum_jg_h);
	else if(GetPropID("desEndPos.spatialOperationStyle")==id)
	{
		if(pLineAngle->desEndPos.spatialOperationStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Specify Datum Node");
		else if(pLineAngle->desEndPos.spatialOperationStyle==1)
			strcpy(sText,"Ridge Project");
		else if(pLineAngle->desEndPos.spatialOperationStyle==2)
			strcpy(sText,"X Leg Project");
		else if(pLineAngle->desEndPos.spatialOperationStyle==3)
			strcpy(sText,"Y Leg Project");
		else if(pLineAngle->desEndPos.spatialOperationStyle==4)
			strcpy(sText,"Ridge Plane Offset");
		else if(pLineAngle->desEndPos.spatialOperationStyle==5)
			strcpy(sText,"X Leg Plane Offset");
		else if(pLineAngle->desEndPos.spatialOperationStyle==6)
			strcpy(sText,"Y Leg Plane Offset");
		else if(pLineAngle->desEndPos.spatialOperationStyle==7)
			strcpy(sText,"Ridge Bend Line Project");
		else if(pLineAngle->desEndPos.spatialOperationStyle==8)
			strcpy(sText,"X Leg Bend Line Project");
		else if(pLineAngle->desEndPos.spatialOperationStyle==9)
			strcpy(sText,"Y Leg Bend Line Project");
		else if(pLineAngle->desEndPos.spatialOperationStyle==10)
			strcpy(sText,"Ridge Bend Line Plane Offset");
		else if(pLineAngle->desEndPos.spatialOperationStyle==11)
			strcpy(sText,"X Leg Bend Line Plane Offset");
		else if(pLineAngle->desEndPos.spatialOperationStyle==12)
			strcpy(sText,"Y Leg Bend Line Plane Offset");
		else if(pLineAngle->desEndPos.spatialOperationStyle==13)
			strcpy(sText,"X Leg Extend Line");
		else if(pLineAngle->desEndPos.spatialOperationStyle==14)
			strcpy(sText,"Y Leg Extend Line");
		else if(pLineAngle->desEndPos.spatialOperationStyle==15)
			strcpy(sText,"Legs Coordinate Control");
		else if(pLineAngle->desEndPos.spatialOperationStyle==16)
			strcpy(sText,"Project To Line");
		else if(pLineAngle->desEndPos.spatialOperationStyle==17)
			strcpy(sText,"Plane Offset To Line");
		else if(pLineAngle->desEndPos.spatialOperationStyle==18)
			strcpy(sText,"Node Project");
		else if(pLineAngle->desEndPos.spatialOperationStyle==19)
			strcpy(sText,"Node Plane Offset");
#else
			strcpy(sText,"ָ����׼��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==1)
			strcpy(sText,"����(��)��ͶӰ");
		else if(pLineAngle->desEndPos.spatialOperationStyle==2)
			strcpy(sText,"X֫����ͶӰ");
		else if(pLineAngle->desEndPos.spatialOperationStyle==3)
			strcpy(sText,"Y֫����ͶӰ");
		else if(pLineAngle->desEndPos.spatialOperationStyle==4)
			strcpy(sText,"������ƽ��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==5)
			strcpy(sText,"X֫����ƽ��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==6)
			strcpy(sText,"Y֫����ƽ��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==7)
			strcpy(sText,"�����߻�����ͶӰ");
		else if(pLineAngle->desEndPos.spatialOperationStyle==8)
			strcpy(sText,"X֫���߻�����ͶӰ");
		else if(pLineAngle->desEndPos.spatialOperationStyle==9)
			strcpy(sText,"Y֫���߻�����ͶӰ");
		else if(pLineAngle->desEndPos.spatialOperationStyle==10)
			strcpy(sText,"�����߻�����ƽ��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==11)
			strcpy(sText,"X֫���߻�����ƽ��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==12)
			strcpy(sText,"Y֫���߻�����ƽ��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==13)
			strcpy(sText,"X֫ƫ��׼��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==14)
			strcpy(sText,"Y֫ƫ��׼��");
		else if(pLineAngle->desEndPos.spatialOperationStyle==15)
#ifndef __NEW_ANGLE_LOCA_MODE_
			strcpy(sText,"��֫�������");
#else
			strcpy(sText,"�ڵ��������");
#endif
		else if(pLineAngle->desEndPos.spatialOperationStyle==16)
			strcpy(sText,"ָ����ͶӰ");
		else if(pLineAngle->desEndPos.spatialOperationStyle==17)
			strcpy(sText,"ָ����ƽ��");
#ifndef __NEW_ANGLE_LOCA_MODE_
		else if(pLineAngle->desEndPos.spatialOperationStyle==18)
			strcpy(sText,"������Ƶ�ͶӰ");
		else if(pLineAngle->desEndPos.spatialOperationStyle==19)
			strcpy(sText,"������Ƶ�ƽ��");
#else
		else if (pLineAngle->desEndPos.spatialOperationStyle == 18 || pLineAngle->desEndPos.spatialOperationStyle == 19)
			strcpy(sText, "�ڵ��������");
#endif
#endif
	}
	else if(GetPropID("desEndPos.datum_offset_dist")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.datum_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.len_offset_type")==id||GetPropID("desEndPos.SimpleLenOffsetType")==id)
	{
		if(pLineAngle->desEndPos.len_offset_type==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Length Extend");
		else if(pLineAngle->desEndPos.len_offset_type==1)
			strcpy(sText,"1.Parallel Offset");
		else if(pLineAngle->desEndPos.len_offset_type=='X')
			strcpy(sText,"2.X");
		else if(pLineAngle->desEndPos.len_offset_type=='Y')
			strcpy(sText,"3.Y");
		else if(pLineAngle->desEndPos.len_offset_type=='Z')
			strcpy(sText,"4.Z");
#else
			strcpy(sText,"0.��������");
		else if(pLineAngle->desEndPos.len_offset_type==1)
			strcpy(sText,"1.����ƽ��ƫ��");
		else if(pLineAngle->desEndPos.len_offset_type=='X')
			strcpy(sText,"2.X�������");
		else if(pLineAngle->desEndPos.len_offset_type=='Y')
			strcpy(sText,"3.Y�������");
		else if(pLineAngle->desEndPos.len_offset_type=='Z')
			strcpy(sText,"4.Z�������");
#endif
	}
	else if(GetPropID("desEndPos.len_offset_dist")==id||GetPropID("desEndPos.SimpleLenOffset")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.SimpleNormOffset")==id)
	{
		char cSelfWorkWing=pLineAngle->GetSelfWorkWing(FALSE,NULL);
		double offsetDist=pLineAngle->desEndPos.wing_x_offset.offsetDist;
		if(cSelfWorkWing=='Y')
			offsetDist=pLineAngle->desEndPos.wing_y_offset.offsetDist;
		CLDSLinePart* pFatherRod=pLineAngle->GetEndDatumPart();
		if(pFatherRod&&pFatherRod->IsAngle())
		{
			CLDSLineAngle* pFatherAngle=(CLDSLineAngle*)pFatherRod;
			GEPOINT vCurrWingNorm =cSelfWorkWing=='X'?pLineAngle->vxWingNorm:pLineAngle->vyWingNorm;
			char cDatumAngleWing=pLineAngle->GetLappedWorkWing(FALSE,vCurrWingNorm);
			GEPOINT vDatumWingNorm=cDatumAngleWing=='X'?pFatherAngle->vxWingNorm:pFatherAngle->vyWingNorm;
			if(vCurrWingNorm*vDatumWingNorm<0)
				offsetDist*=-1;
		}
		if(fabs(offsetDist)<0.0001)
			offsetDist=0;
		sprintf(sText,"%f",offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.offset_zhun")==id||GetPropID("desEndPos.SimpleOffsetZhunju")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.offset_zhun);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.huoqu_jg_h")==id||GetPropID("desEndPos.SimpleBendLineAlongAngle")==id)
		sprintf(sText,"0X%X",pLineAngle->desEndPos.huoqu_jg_h);
	else if(GetPropID("desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2")==id||GetPropID("desEndPos.SimpleBendAngleWing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2==1)
			strcpy(sText,"X Leg");
		else if(pLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2==2)
			strcpy(sText,"Y Leg");
		else
			strcpy(sText,"Default");
#else
		if(pLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2==1)
			strcpy(sText,"0.X֫");
		else if(pLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2==2)
			strcpy(sText,"1.Y֫");
		else
			strcpy(sText,"2.�Զ��ж�");
#endif
	}
	else if(GetPropID("desEndPos.huoqu_dist")==id||GetPropID("desEndPos.SimpleBendClearance")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.huoqu_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.cMainCoordCtrlWing")==id)
	{	//��������֫
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->desEndPos.cMainCoordCtrlWing=='X')
			strcpy(sText,"X Leg");
		else if(pLineAngle->desEndPos.cMainCoordCtrlWing=='Y')
			strcpy(sText,"Y Leg");
		else //if(pLineAngle->desEndPos.cMainCoordCtrlWing==1)
			strcpy(sText,"Projection on dual leg");
#else
		if (pLineAngle->desEndPos.spatialOperationStyle == 18)
			strcpy(sText, "ͶӰ���ߵ�");
		else if (pLineAngle->desEndPos.spatialOperationStyle == 19)
		{
			if (pLineAngle->desEndPos.cFaceOffsetDatumLine == 'X')
				strcpy(sText, "X֫���ߵ�");
			else if (pLineAngle->desEndPos.cFaceOffsetDatumLine == 'Y')
				strcpy(sText, "Y֫���ߵ�");
			else
				strcpy(sText, "ƽ�����ߵ�");
		}
		else if (pLineAngle->desEndPos.cMainCoordCtrlWing == 'X')
			strcpy(sText,"X֫���ߵ�");
		else if(pLineAngle->desEndPos.cMainCoordCtrlWing=='Y')
			strcpy(sText,"Y֫���ߵ�");
		else //if(pLineAngle->desEndPos.cMainCoordCtrlWing==1)
			strcpy(sText,"˫֫ͶӰ��");
#endif
	}
	else if(GetPropID("desEndPos.coordCtrlType")==id)
	{	//����֫�����������
		if(pLineAngle->desEndPos.coordCtrlType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.X,Z");
		else if(pLineAngle->desEndPos.coordCtrlType==1)
			strcpy(sText,"1:Y,Z");
		else if(pLineAngle->desEndPos.coordCtrlType==1)
			strcpy(sText,"2.X,Y");
#else
			strcpy(sText,"0.X,Z����");
		else if(pLineAngle->desEndPos.coordCtrlType==1)
			strcpy(sText,"1:Y,Z����");
		else if(pLineAngle->desEndPos.coordCtrlType==1)
			strcpy(sText,"2.X,Y����");
#endif
	}
	else if(GetPropID("desEndPos.ctrlWing.operStyle")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		if (pLineAngle->desEndPos.spatialOperationStyle == 18)
			strcpy(sText, "0.����ͶӰ");
		else if (pLineAngle->desEndPos.spatialOperationStyle == 19)
			strcpy(sText, "1.X-Yƽ��ƽ��");
		else if(pLineAngle->desEndPos.ctrlWing.operStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Project");
		else if(pLineAngle->desEndPos.ctrlWing.operStyle==1)
			strcpy(sText,"1.X-Y Plane Offset");
		else if(pLineAngle->desEndPos.ctrlWing.operStyle==2)
			strcpy(sText,"2.Y-Z Plane Offset");
		else if(pLineAngle->desEndPos.ctrlWing.operStyle==3)
			strcpy(sText,"3.X-Z Plane Offset");
#else
			strcpy(sText,"0.����ͶӰ");
		else if(pLineAngle->desEndPos.ctrlWing.operStyle==1)
			strcpy(sText,"1.X-Yƽ��ƽ��");
		else if(pLineAngle->desEndPos.ctrlWing.operStyle==2)
			strcpy(sText,"2.Y-Zƽ��ƽ��");
		else if(pLineAngle->desEndPos.ctrlWing.operStyle==3)
			strcpy(sText,"3.X-Zƽ��ƽ��");
#endif
	}
	else if(GetPropID("desEndPos.ctrlWing.gStyle")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		int giStyle = pLineAngle->desEndPos.xMasterWing.gStyle;
		if(giStyle ==0)
			strcpy(sText,"g");
		else if(giStyle ==1)
			strcpy(sText,"g1");
		else if(giStyle ==2)
			strcpy(sText,"g2");
		else if(giStyle ==3)
			strcpy(sText,"g3");
		else if(giStyle ==4)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desEndPos.ctrlWing.offsetDist")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		if(pLineAngle->desEndPos.xMasterWing.gStyle<4)
		{
			CSuperSmartPtr<CLDSLinePart> pDatumRod=pLineAngle->GetEndDatumPart();
			if(pDatumRod.IsNULL())
				pLineAngle->Log2File()->Log("0X%X�Ǹ��ն˶�λ�����˼�δ�ҵ�!",pLineAngle->handle);
			else if(pDatumRod->IsAngle()&&pLineAngle->desEndPos.cMainCoordCtrlWing=='X')
				pLineAngle->desEndPos.xMasterWing.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingXG(pLineAngle->desEndPos.xMasterWing.gStyle);
			else if(pDatumRod->IsAngle()&&pLineAngle->desEndPos.cMainCoordCtrlWing=='Y')
				pLineAngle->desEndPos.xMasterWing.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingYG(pLineAngle->desEndPos.xMasterWing.gStyle);
			else
				pLineAngle->desEndPos.xMasterWing.offsetDist=0;
		}
		sprintf(sText,"%f",pLineAngle->desEndPos.xMasterWing.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.otherWing.operStyle")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		if (pLineAngle->desEndPos.spatialOperationStyle == 18)
			strcpy(sText, "0.����ͶӰ");
		else if (pLineAngle->desEndPos.spatialOperationStyle == 19)
			strcpy(sText, "1.X-Yƽ��ƽ��");
		else if(pLineAngle->desEndPos.otherWing.operStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Project");
		else if(pLineAngle->desEndPos.otherWing.operStyle==1)
			strcpy(sText,"1.X-Y Plane Offset");
		else if(pLineAngle->desEndPos.otherWing.operStyle==2)
			strcpy(sText,"2.Y-Z Plane Offset");
		else if(pLineAngle->desEndPos.otherWing.operStyle==3)
			strcpy(sText,"3.X-Z Plane Offset");
#else
			strcpy(sText,"0.����ͶӰ");
		else if(pLineAngle->desEndPos.otherWing.operStyle==1)
			strcpy(sText,"1.X-Yƽ��ƽ��");
		else if(pLineAngle->desEndPos.otherWing.operStyle==2)
			strcpy(sText,"2.Y-Zƽ��ƽ��");
		else if(pLineAngle->desEndPos.otherWing.operStyle==3)
			strcpy(sText,"3.X-Zƽ��ƽ��");
#endif
	}
	else if(GetPropID("desEndPos.otherWing.gStyle")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		int giStyle = pLineAngle->desEndPos.xSlaveWing.gStyle;
		if (giStyle == 0)
			strcpy(sText, "g");
		else if (giStyle == 1)
			strcpy(sText, "g1");
		else if (giStyle == 2)
			strcpy(sText, "g2");
		else if (giStyle == 3)
			strcpy(sText, "g3");
		else if (giStyle == 4)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desEndPos.otherWing.offsetDist")==id)
	{	//����֫�ؼ��㶨λ��ʽ
		if(pLineAngle->desEndPos.xSlaveWing.gStyle<4)
		{
			CSuperSmartPtr<CLDSLinePart> pDatumRod=pLineAngle->GetEndDatumPart();
			if(pDatumRod.IsNULL())
				pLineAngle->Log2File()->Log("0X%X�Ǹ��ն˶�λ�����˼�δ�ҵ�!",pLineAngle->handle);
			else if(pDatumRod->IsAngle()&&pLineAngle->desEndPos.cMainCoordCtrlWing=='Y')
				pLineAngle->desEndPos.xSlaveWing.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingXG(pLineAngle->desEndPos.xSlaveWing.gStyle);
			else if(pDatumRod->IsAngle()&&pLineAngle->desEndPos.cMainCoordCtrlWing=='X')
				pLineAngle->desEndPos.xSlaveWing.offsetDist=
				pDatumRod.LineAnglePointer()->GetWingYG(pLineAngle->desEndPos.xSlaveWing.gStyle);
			else
				pLineAngle->desEndPos.xSlaveWing.offsetDist=0;
		}
		sprintf(sText,"%f",pLineAngle->desEndPos.xSlaveWing.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.fLenOffsetOfCoordWing")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.fLenOffsetOfCoordWing);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desStartPos.bendingLenOffsetComplement")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartPos.datumPoint.des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.bendingLenOffsetComplement")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.datumPoint.des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.datumPoint.x")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.datumPoint.Position().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.datumPoint.y")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.datumPoint.Position().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.datumPoint.z")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.datumPoint.Position().z);
		SimplifiedNumString(sText);
	}
	/*else if(GetPropID("pOldDesEndPos->start_jg_h")==id)
		sprintf(sText,"0X%X",pLineAngle->pOldDesEndPos->start_jg_h);
	else if(GetPropID("pOldDesEndPos->iStartJgBerStyle")==id)
	{
		if(pLineAngle->pOldDesEndPos->iStartJgBerStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Start Ridge Vertex");
		else
			strcpy(sText,"End Ridge Vertex");
#else
			strcpy(sText,"ʼ�����");
		else
			strcpy(sText,"�ն����");
#endif
	}
	else if(GetPropID("pOldDesEndPos->fStartJgLengOffsetX")==id)
	{
		sprintf(sText,"%f",pLineAngle->pOldDesEndPos->fStartJgLengOffsetX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pOldDesEndPos->fStartJgLengOffsetY")==id)
	{
		sprintf(sText,"%f",pLineAngle->pOldDesEndPos->fStartJgLengOffsetY);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pOldDesEndPos->end_jg_h")==id)
		sprintf(sText,"0X%X",pLineAngle->pOldDesEndPos->end_jg_h);
	else if(GetPropID("pOldDesEndPos->iEndJgBerStyle")==id)
	{
		if(pLineAngle->pOldDesEndPos->iEndJgBerStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Start Ridge Vertex");
		else
			strcpy(sText,"End Ridge Vertex");
#else
			strcpy(sText,"ʼ�����");
		else
			strcpy(sText,"�ն����");
#endif
	}
	else if(GetPropID("pOldDesEndPos->fEndJgLengOffsetX")==id)
	{
		sprintf(sText,"%f",pLineAngle->pOldDesEndPos->fEndJgLengOffsetX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pOldDesEndPos->fEndJgLengOffsetY")==id)
	{
		sprintf(sText,"%f",pLineAngle->pOldDesEndPos->fEndJgLengOffsetY);
		SimplifiedNumString(sText);
	}*/
	else if(GetPropID("desEndPos.cFaceOffsetDatumLine")==id)
	{
		if(pLineAngle->desEndPos.cFaceOffsetDatumLine=='X')
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else if(pLineAngle->desEndPos.cFaceOffsetDatumLine=='Y')
			strcpy(sText,"Y Leg");
		else
			strcpy(sText,"Ridge Line");
#else
			strcpy(sText,"X֫����");
		else if(pLineAngle->desEndPos.cFaceOffsetDatumLine=='Y')
			strcpy(sText,"Y֫����");
		else
			strcpy(sText,"����");
#endif
	}
	else if(GetPropID("desEndPos.face_offset_norm")==id)
	{
		double x=fabs(pLineAngle->desEndPos.face_offset_norm.x);
		double y=fabs(pLineAngle->desEndPos.face_offset_norm.y);
		double z=fabs(pLineAngle->desEndPos.face_offset_norm.z);
		if(x>EPS&&y<EPS&&z<EPS)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Y-Z Plane");
		else if(x<EPS&&y>EPS&&z<EPS)
			strcpy(sText,"X-Z Plane");
		else if(x<EPS&&y<EPS&&z>EPS)
			strcpy(sText,"X-Y Plane");
		else
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"Y-Zƽ��");
		else if(x<EPS&&y>EPS&&z<EPS)
			strcpy(sText,"X-Zƽ��");
		else if(x<EPS&&y<EPS&&z>EPS)
			strcpy(sText,"X-Yƽ��");
		else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desEndPos.face_offset_norm.x")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.face_offset_norm.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.face_offset_norm.y")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.face_offset_norm.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.face_offset_norm.z")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndPos.face_offset_norm.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.cWorkWingStyle")==id)
	{
		if(pLineAngle->ciEndAnchorWing==0)//desEndPos.cWorkWingStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
		else if(pLineAngle->ciEndAnchorWing==1)//desEndPos.cWorkWingStyle==1)
			strcpy(sText,"X Leg");
		else if(pLineAngle->ciEndAnchorWing==2)//desEndPos.cWorkWingStyle==2)
			strcpy(sText,"Y Leg");
		else if(pLineAngle->ciEndAnchorWing==2)//desEndPos.cWorkWingStyle==3)
			strcpy(sText,"X&Y Leg");
#else
			strcpy(sText,"�����ж�");
		else if(pLineAngle->ciEndAnchorWing==1)//desEndPos.cWorkWingStyle==1)
			strcpy(sText,"X֫��λ");
		else if(pLineAngle->ciEndAnchorWing==2)//desEndPos.cWorkWingStyle==2)
			strcpy(sText,"Y֫��λ");
		else if(pLineAngle->ciEndAnchorWing==3)//desEndPos.cWorkWingStyle==3)
			strcpy(sText,"˫֫��λ");
#endif
	}
	else if(GetPropID("desEndPos.wing_x_offset.gStyle")==id)
	{
		if(pLineAngle->desEndPos.wing_x_offset.gStyle==0)
			strcpy(sText,"g");
		else if(pLineAngle->desEndPos.wing_x_offset.gStyle==1)
			strcpy(sText,"g1");
		else if(pLineAngle->desEndPos.wing_x_offset.gStyle==2)
			strcpy(sText,"g2");
		else if(pLineAngle->desEndPos.wing_x_offset.gStyle==3)
			strcpy(sText,"g3");
		else if(pLineAngle->desEndPos.wing_x_offset.gStyle==4)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desEndPos.wing_x_offset.offsetDist")==id)
	{
		pLineAngle->desEndPos.wing_x_offset.offsetDist = pLineAngle->GetEndWingNormOffset(0);
		sprintf(sText,"%f",pLineAngle->desEndPos.wing_x_offset.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.wing_y_offset.gStyle")==id)
	{
		if(pLineAngle->desEndPos.wing_y_offset.gStyle==0)
			strcpy(sText,"g");
		else if(pLineAngle->desEndPos.wing_y_offset.gStyle==1)
			strcpy(sText,"g1");
		else if(pLineAngle->desEndPos.wing_y_offset.gStyle==2)
			strcpy(sText,"g2");
		else if(pLineAngle->desEndPos.wing_y_offset.gStyle==3)
			strcpy(sText,"g3");
		else if(pLineAngle->desEndPos.wing_y_offset.gStyle==4)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
#else
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("desEndPos.wing_y_offset.offsetDist")==id)
	{
		pLineAngle->desEndPos.wing_y_offset.offsetDist = pLineAngle->GetEndWingNormOffset(1);
		sprintf(sText,"%f",pLineAngle->desEndPos.wing_y_offset.offsetDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("desEndPos.fEccentricDist")==id)
	{	//ƫ�ľ�
		sprintf(sText,"%f",pLineAngle->desEndPos.fEccentricDist);
		SimplifiedNumString(sText);
	}
	else
		bContinueJustify=TRUE;
	if(!bContinueJustify)
	{	//if-else�ж�̫�����ͨ����ȥ������ֻ�����м�Ͽ�
#ifdef AFX_TARG_ENU_ENGLISH
		if(strlen(sText)>=nMaxStrBufLen)
			pLineAngle->Log2File()->Log("0X%X Angle's property##d string value out of length",pLineAngle->handle,id);
#else
		if(strlen(sText)>=nMaxStrBufLen)
			pLineAngle->Log2File()->Log("�Ǹ�0X%X����#%d�ַ���������������",pLineAngle->handle,id);
#endif
		if(valueStr)
			StrCopy(valueStr,sText,nMaxStrBufLen);
		return strlen(sText);
	}
	bContinueJustify=FALSE;
	if(GetPropID("oddStart")==id)
	{
		sprintf(sText,"%f",pLineAngle->startOdd());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("oddStart.m_iOddCalStyle")==id)
	{
		if(pLineAngle->desStartOdd.m_iOddCalStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Collide");
		else if(pLineAngle->desStartOdd.m_iOddCalStyle==1)
			strcpy(sText,"1.Bolt Check");
		else //if(pLineAngle->desStartOdd.m_iOddCalStyle==2)
			strcpy(sText,"2.Specify");
#else
			strcpy(sText,"0.��ײ����");
		else if(pLineAngle->desStartOdd.m_iOddCalStyle==1)
			strcpy(sText,"1.��˨���");
		else //if(pLineAngle->desStartOdd.m_iOddCalStyle==2)
			strcpy(sText,"2.ָ����ֵ");
#endif
	}
	else if(GetPropID("oddStart.m_fCollideDist")==id)
	{
		sprintf(sText,"%f",pLineAngle->desStartOdd.m_fCollideDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("oddStart.m_hRefPart1")==id)
		sprintf(sText,"0X%X",pLineAngle->desStartOdd.m_hRefPart1);
	else if(GetPropID("oddStart.m_hRefPart2")==id)
		sprintf(sText,"0X%X",pLineAngle->desStartOdd.m_hRefPart2);
	else if(GetPropID("oddStartExtra")==id)
	{
		double odd_ber=pLineAngle->GetStartExtraOdd(2);
		if(odd_ber!=0)
			sprintf(sText,"%.1f/%.1f:%.1f",pLineAngle->GetStartExtraOdd(0),pLineAngle->GetStartExtraOdd(1),odd_ber);
		else
			sprintf(sText,"%.1f/%.1f",pLineAngle->GetStartExtraOdd(0),pLineAngle->GetStartExtraOdd(1));
	}
	else if(GetPropID("oddEnd")==id)
	{
		sprintf(sText,"%f",pLineAngle->endOdd());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("oddEnd.m_iOddCalStyle")==id)
	{
		if(pLineAngle->desEndOdd.m_iOddCalStyle==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Collide");
		else if(pLineAngle->desEndOdd.m_iOddCalStyle==1)
			strcpy(sText,"1.Bolt Check");
		else //if(pLineAngle->desEndOdd.m_iOddCalStyle==2)
			strcpy(sText,"2.Specify");
#else
			strcpy(sText,"0.��ײ����");
		else if(pLineAngle->desEndOdd.m_iOddCalStyle==1)
			strcpy(sText,"1.��˨���");
		else //if(pLineAngle->desEndOdd.m_iOddCalStyle==2)
			strcpy(sText,"2.ָ����ֵ");
#endif
	}
	else if(GetPropID("oddEnd.m_fCollideDist")==id)
	{
		sprintf(sText,"%f",pLineAngle->desEndOdd.m_fCollideDist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("oddEnd.m_hRefPart1")==id)
		sprintf(sText,"0X%X",pLineAngle->desEndOdd.m_hRefPart1);
	else if(GetPropID("oddEnd.m_hRefPart2")==id)
		sprintf(sText,"0X%X",pLineAngle->desEndOdd.m_hRefPart2);
	else if(GetPropID("oddEndExtra")==id)
	{
		double odd_ber=pLineAngle->GetEndExtraOdd(2);
		if(odd_ber!=0)
			sprintf(sText,"%.1f/%.1f:%.1f",pLineAngle->GetEndExtraOdd(0),pLineAngle->GetEndExtraOdd(1),odd_ber);
		else
			sprintf(sText,"%.1f/%.1f",pLineAngle->GetEndExtraOdd(0),pLineAngle->GetEndExtraOdd(1));
	}
	else if(GetPropID("StartOddCanAcrossMiddleSect")==id||GetPropID("EndOddCanAcrossMiddleSect")==id)
	{
		if(pLineAngle->m_blOddByBoltCanAcrossMiddleSect)
			strcpy(sText,"1.����");
		else
			strcpy(sText,"0.������");
	}
	else if(GetPropID("blScatterBoltLocaTolerance")==id)
	{
		if(pLineAngle->m_blScatterBoltLocaTolerance)
			strcpy(sText,"1.��֤��λ");
		else
			strcpy(sText,"0.��֤���");
	}
	else if(GetPropID("connectStart.grade")==id)
		sprintf(sText,"%s",pLineAngle->connectStart.grade);
	else if(GetPropID("connectEnd.grade")==id)
		sprintf(sText,"%s",pLineAngle->connectEnd.grade);
	else if(GetPropID("connectStart.m_iConnectWing")==id)
	{
		if(pLineAngle->connectStart.m_iConnectWing==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Two Leg Connect");
		else
			strcpy(sText,"1.One Leg Connect");
#else 
			strcpy(sText,"0.˫֫����");
		else
			strcpy(sText,"1.��֫����");
#endif
	}
	else if(GetPropID("connectEnd.m_iConnectWing")==id)
	{
		if(pLineAngle->connectEnd.m_iConnectWing==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Two Leg Connect");
		else
			strcpy(sText,"1.One Leg Connect");
#else 
			strcpy(sText,"0.˫֫����");
		else
			strcpy(sText,"1.��֫����");
#endif
	}
	else if(GetPropID("connectStart.d")==id)
		sprintf(sText,"M%d", pLineAngle->connectStart.d);
	else if(GetPropID("connectEnd.d")==id)
		sprintf(sText,"M%d", pLineAngle->connectEnd.d);
	else if(GetPropID("connectStart.N")==id)
		sprintf(sText,"%d", pLineAngle->connectStart.wnConnBoltN);
	else if(GetPropID("connectStart.summBoltN")==id)
		sprintf(sText,"%d",pLineAngle->connectStart.wnSummBoltN);
	else if(GetPropID("connectEnd.N")==id)
		sprintf(sText,"%d", pLineAngle->connectEnd.wnConnBoltN);
	else if(GetPropID("connectEnd.summBoltN")==id)
		sprintf(sText,"%d",pLineAngle->connectEnd.wnSummBoltN);
	else if(GetPropID("connectStart.rows")==id)
		sprintf(sText,"%d", pLineAngle->connectStart.rows);
	else if(GetPropID("connectEnd.rows")==id)
		sprintf(sText,"%d", pLineAngle->connectEnd.rows);
	else if(GetPropID("norm_x_wing")==id)
	{
		if(!pLineAngle->des_norm_x.bByOtherWing&&!pLineAngle->des_norm_x.bSpecific&&pLineAngle->des_norm_x.hViceJg>0x20)	//֫���߷���Ϊ���˼������淨��
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sText,499,"Coplanar With 0X%X",pLineAngle->des_norm_x.hViceJg);
		else if(pLineAngle->des_norm_x.bByOtherWing&&!pLineAngle->des_norm_x.bSpecific)
			strcpy(sText,"Vertical To Another Leg");
		else 
			strcpy(sText,"Specify Direction");
#else
			_snprintf(sText,499,"��0X%X����",pLineAngle->des_norm_x.hViceJg);
		else if(pLineAngle->des_norm_x.bByOtherWing&&!pLineAngle->des_norm_x.bSpecific)
			strcpy(sText,"��ֱ����һ֫");
		else 
			strcpy(sText,"ָ������");
#endif
	}
	else if(GetPropID("norm_x_wing.x")==id)
	{
		sprintf(sText,"%f",pLineAngle->get_norm_x_wing().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm_x_wing.y")==id)
	{
		sprintf(sText,"%f",pLineAngle->get_norm_x_wing().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm_x_wing.z")==id)
	{
		sprintf(sText,"%f",pLineAngle->get_norm_x_wing().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm_x_wing.hViceJg")==id)
	{
		if(!pLineAngle->des_norm_x.IsSpecNormActual())
			sprintf(sText,"0X%X",pLineAngle->des_norm_x.hViceJg);
		else if(pLineAngle->des_norm_x.spec_norm.norm_style==2)
		{
			if(pLineAngle->des_norm_x.spec_norm.hVicePart!=pLineAngle->handle)
				sprintf(sText,"0X%X",pLineAngle->des_norm_x.spec_norm.hVicePart);
			else
				sprintf(sText,"0X%X",pLineAngle->des_norm_x.spec_norm.hCrossPart);
		}
		else if(pLineAngle->des_norm_x.spec_norm.norm_style>0)
			sprintf(sText,"0X%X",pLineAngle->des_norm_x.spec_norm.hVicePart);
	}
	else if(GetPropID("norm_y_wing")==id)
	{
		if(!pLineAngle->des_norm_y.bByOtherWing&&!pLineAngle->des_norm_y.bSpecific&&pLineAngle->des_norm_y.hViceJg>0x20)	//֫���߷���Ϊ���˼������淨��
#ifdef AFX_TARG_ENU_ENGLISH
			_snprintf(sText,499,"Coplanar With 0X%X",pLineAngle->des_norm_y.hViceJg);
		else if(pLineAngle->des_norm_y.bByOtherWing&&!pLineAngle->des_norm_y.bSpecific)
			strcpy(sText,"Vertical To Another Leg");
		else 
			strcpy(sText,"Special Direction");
#else
			_snprintf(sText,499,"��0X%X����",pLineAngle->des_norm_y.hViceJg);
		else if(pLineAngle->des_norm_y.bByOtherWing&&!pLineAngle->des_norm_y.bSpecific)
			strcpy(sText,"��ֱ����һ֫");
		else 
			strcpy(sText,"ָ������");
#endif
	}
	else if(GetPropID("norm_y_wing.x")==id)
	{
		sprintf(sText,"%f",pLineAngle->get_norm_y_wing().x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm_y_wing.y")==id)
	{
		sprintf(sText,"%f",pLineAngle->get_norm_y_wing().y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm_y_wing.z")==id)
	{
		sprintf(sText,"%f",pLineAngle->get_norm_y_wing().z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm_y_wing.hViceJg")==id)
	{
		if(!pLineAngle->des_norm_y.IsSpecNormActual())
			sprintf(sText,"0X%X",pLineAngle->des_norm_y.hViceJg);
		else if(pLineAngle->des_norm_y.spec_norm.norm_style==2)
		{
			if(pLineAngle->des_norm_y.spec_norm.hVicePart!=pLineAngle->handle)
				sprintf(sText,"0X%X",pLineAngle->des_norm_y.spec_norm.hVicePart);
			else
				sprintf(sText,"0X%X",pLineAngle->des_norm_y.spec_norm.hCrossPart);
		}
		else if(pLineAngle->des_norm_y.spec_norm.norm_style>0)
			sprintf(sText,"0X%X",pLineAngle->des_norm_y.spec_norm.hVicePart);
	}
	else if(GetPropID("work_wing")==id)
		strcpy(sText,"���˼������淨��");
	else if(GetPropID("work_wing.hViceJg")==id)
	{
		if(pLineAngle->GetWorkWingX0Y1()==0)
			sprintf(sText,"0X%X",pLineAngle->des_norm_x.hViceJg);
		else if(pLineAngle->GetWorkWingX0Y1()==1)
			sprintf(sText,"0X%X",pLineAngle->des_norm_y.hViceJg);
	}
	else if(GetPropID("XorYMirLayStyle")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pLineAngle->MirLayStyle==CLDSLineAngle::MIRXorY_SAMEDIRECT)
			strcpy(sText,"1.Same direction");
		else if(pLineAngle->MirLayStyle==CLDSLineAngle::MIRXorY_INVERT)
			strcpy(sText,"2.Invert direction");
		else //if(!pLineAngle->MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			strcpy(sText,"0.Unchanged");
#else
		if(pLineAngle->MirLayStyle==CLDSLineAngle::MIRXorY_SAMEDIRECT)
			strcpy(sText,"1.����ͬ��");
		else if(pLineAngle->MirLayStyle==CLDSLineAngle::MIRXorY_INVERT)
			strcpy(sText,"2.���ⷴ��");
		else //if(!pLineAngle->MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			strcpy(sText,"0.����ԭ����");
#endif
	}
	else if(GetPropID("decWingAngle")==id)
	{
		sprintf(sText,"%.1f",pLineAngle->GetDecWingAngle());
		SimplifiedNumString(sText);
		//ʹ���û����õĿ��Ͻ���ֵ�жϽǸ��Ƿ���Ҫ���п��Ͻ� wht 11-05-06
		double fThreshold= library!=NULL?library->GetJgKaiHeJiaoThreshold():2;
		if(atof(sText)>(90+fThreshold))
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"%s��(Leg Open)",sText);
#else
			sprintf(sText,"%s��(���Ǵ���)",sText);
#endif
		else if(atof(sText)>(90-fThreshold))
			sprintf(sText,"%s��",sText);
		else if(atof(sText)>(90+fThreshold))
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sText,"%s��(Leg Close)",sText);
#else
			sprintf(sText,"%s��(�ϽǴ���)",sText);
#endif
	}
	else if(GetPropID("m_cHeJiaoReservedWidthType")==id)
	{
		if(pLineAngle->m_cHeJiaoReservedWidthType=='O')
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"External Width");
		else
			strcpy(sText,"Internal Width");
#else
			strcpy(sText,"���");
		else
			strcpy(sText,"�ڲ�");
#endif
	}
	else if(GetPropID("m_fHeJiaoReservedWidth")==id)
	{
		sprintf(sText,"%f",pLineAngle->m_fHeJiaoReservedWidth);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("kaihe_base_wing_x0_y1")==id)
	{
		int kaihe_base_wing_x0_y1 = pLineAngle->GetKaiHeWingX0Y1();
		if(kaihe_base_wing_x0_y1==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else if(kaihe_base_wing_x0_y1==1)
			strcpy(sText,"Y Leg");
		else 
			strcpy(sText,"None");
#else
			strcpy(sText,"X֫");
		else if(kaihe_base_wing_x0_y1==1)
			strcpy(sText,"Y֫");
		else 
			strcpy(sText,"������");
#endif
	}
	else if(GetPropID("wingXG")==id)
	{
		JGZJ jgzjx=pLineAngle->GetZhunJu('X');
		if(pLineAngle->m_bEnableTeG)
			jgzjx=pLineAngle->xWingXZhunJu;
		sprintf(sText,"%d",jgzjx.g);
	}
	else if(GetPropID("wingYG")==id)
	{
		JGZJ jgzjy=pLineAngle->GetZhunJu('Y');
		if(pLineAngle->m_bEnableTeG)
			jgzjy=pLineAngle->xWingYZhunJu;
		sprintf(sText,"%d",jgzjy.g);
	}
	else if(GetPropID("m_bEnableTeG")==id)
	{
		if(pLineAngle->m_bEnableTeG)
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
	else if(GetPropID("m_cPosCtrlDatumLineType")==id)
	{
		if(pLineAngle->m_cPosCtrlDatumLineType=='X')
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else if(pLineAngle->m_cPosCtrlDatumLineType=='Y')
			strcpy(sText,"Y Leg");
		else
			strcpy(sText,"None");
#else
			strcpy(sText,"X֫����");
		else if(pLineAngle->m_cPosCtrlDatumLineType=='Y')
			strcpy(sText,"Y֫����");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("m_cPosCtrlType")==id)
	{
		if(pLineAngle->m_cPosCtrlType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"0.Project");
		else if(pLineAngle->m_cPosCtrlType==1)
			strcpy(sText,"1.X-Y Plane Offset");
		else if(pLineAngle->m_cPosCtrlType==2)
			strcpy(sText,"2.Y-Z Plane Offset");
		else if(pLineAngle->m_cPosCtrlType==3)
			strcpy(sText,"3.X-Z Plane Offset");
		else
			strcpy(sText,"No");
#else
			strcpy(sText,"0.����ͶӰ");
		else if(pLineAngle->m_cPosCtrlType==1)
			strcpy(sText,"1.X-Yƽ��ƽ��");
		else if(pLineAngle->m_cPosCtrlType==2)
			strcpy(sText,"2.Y-Zƽ��ƽ��");
		else if(pLineAngle->m_cPosCtrlType==3)
			strcpy(sText,"3.X-Zƽ��ƽ��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("start_push_flat_x1_y2")==id)
	{
		if(pLineAngle->start_push_flat_x1_y2==1)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else if(pLineAngle->start_push_flat_x1_y2==2)
			strcpy(sText,"Y Leg");
		else //if(start_push_flat_x1_y2==0)
			strcpy(sText,"None");
#else
			strcpy(sText,"X֫");
		else if(pLineAngle->start_push_flat_x1_y2==2)
			strcpy(sText,"Y֫");
		else //if(start_push_flat_x1_y2==0)
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("start_push_flat_length")==id)
	{
		sprintf(sText,"%f",pLineAngle->start_push_flat_length);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("mid_push_flat_x1_y2")==id)
	{
		if(pLineAngle->mid_push_flat_x1_y2==1)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else if(pLineAngle->mid_push_flat_x1_y2==2)
			strcpy(sText,"Y Leg");
		else //if(mid_push_flat_x1_y2==0)
			strcpy(sText,"None");
#else
			strcpy(sText,"X֫");
		else if(pLineAngle->mid_push_flat_x1_y2==2)
			strcpy(sText,"Y֫");
		else //if(mid_push_flat_x1_y2==0)
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("mid_push_flat_length")==id)
	{
		sprintf(sText,"%f",pLineAngle->mid_push_flat_length);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("mid_push_flat_start_offset")==id)
	{
		sprintf(sText,"%f",pLineAngle->mid_push_flat_start_offset);
		SimplifiedNumString(sText);
	}
	else if (GetPropID("mid_push_flat_end_offset")==id)
	{
		sprintf(sText,"%f",pLineAngle->mid_push_flat_end_offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("mid_push_flat_node_handle")==id)
		sprintf(sText,"0X%X",pLineAngle->mid_push_flat_node_handle);
	else if(GetPropID("FilletROfSX")==id)
	{
		sprintf(sText,"%f",pLineAngle->dfFilletROfSX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("FilletROfSY")==id)
	{
		sprintf(sText,"%f",pLineAngle->dfFilletROfSY);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("FilletROfEX")==id)
	{
		sprintf(sText,"%f",pLineAngle->dfFilletROfEX);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("FilletROfEY")==id)
	{
		sprintf(sText,"%f",pLineAngle->dfFilletROfEY);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("end_push_flat_x1_y2")==id)
	{
		if(pLineAngle->end_push_flat_x1_y2==1)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"X Leg");
		else if(pLineAngle->end_push_flat_x1_y2==2)
			strcpy(sText,"Y Leg");
		else //if(end_push_flat_x1_y2==0)
			strcpy(sText,"None");
#else
			strcpy(sText,"X֫");
		else if(pLineAngle->end_push_flat_x1_y2==2)
			strcpy(sText,"Y֫");
		else //if(end_push_flat_x1_y2==0)
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("end_push_flat_length")==id)
	{
		sprintf(sText,"%f",pLineAngle->end_push_flat_length);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fCutAngleCoefR")==id)
	{
		CXhChar16 scaleStr=pLineAngle->GetCutAngleSpaceExpr();
		//CExpression::ToExpression(pLineAngle->m_fCutAngleCoefR,3,scaleStr,8);
		sprintf(sText,"%s",(char*)scaleStr);
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)pLineAngle->iSeg.ToString()); 
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)pLineAngle->GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(pLineAngle->cfgword,valueStr);
	{
		if(pLineAngle->layer(0)=='L')	//���ȹ���
			return pLineAngle->cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return pLineAngle->cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("relativeObjs")==id)
	{
		for(RELATIVE_OBJECT *pRelaObj=pLineAngle->relativeObjs.GetFirst();pRelaObj;pRelaObj=pLineAngle->relativeObjs.GetNext())
		{
			if(strlen(sText)==0)
				_snprintf(sText,499,"0X%X",pRelaObj->hObj);
			else
				_snprintf(sText,499,"%s,0X%X",sText,pRelaObj->hObj);
		}
	}
	else if(GetPropID("shadowInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("m_hMotherObj")==id)
	{
		CLDSLineAngle *pMotherAngle=(CLDSLineAngle*)(pLineAngle->MotherObject(false));
		if(pMotherAngle!=pLineAngle)
			sprintf(sText,"0X%X",pMotherAngle->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_bSyncShadowPartNo")==id)
	{
		if(pLineAngle->m_bSyncShadowPartNo)
			strcpy(sText,"��");
		else 
			strcpy(sText,"��");
	}
	else if(GetPropID("m_hBlockRef")==id)
	{
		CBlockParameter blockPara;
		if (pLineAngle->GetBlockParameter(blockPara))
			sprintf(sText,"0X%X",blockPara.m_pBlockRef->handle);
		else 
			strcpy(sText,"0X0");
	}
	else if(GetPropID("m_hBlock")==id)
		strcpy(sText,pLineAngle->GetBlockIdName());
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",pLineAngle->m_uStatMatCoef);
	else if(GetPropID("startCutWing")==id)
	{
		if(pLineAngle->m_bUserSpecStartCutAngle)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
		else
			strcpy(sText,"Default");
#else
			strcpy(sText,"�û�ָ��");
		else
			strcpy(sText,"�Զ�����");
#endif
	}
	else if(GetPropID("endCutWing")==id)
	{
		if(pLineAngle->m_bUserSpecEndCutAngle)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User-Defined");
		else
			strcpy(sText,"Default");
#else
			strcpy(sText,"�û�ָ��");
		else
			strcpy(sText,"�Զ�����");
#endif
	}
	else if(GetPropID("startCutWingX")==id)
	{	//ʼ����֫�н�
		CXhChar50 cutWingStr,cutAngleStrX;
		if(pLineAngle->cut_wing[0]==1)	
		{	//ʼ��X֫��֫
			cutWingStr.Printf("%.fx%.fx%.f",pLineAngle->cut_wing_para[0][1],
				pLineAngle->cut_wing_para[0][0],pLineAngle->cut_wing_para[0][2]);
		}
		if(pLineAngle->cut_angle[0][0]>0)
		{	//ʼ��X֫�н�
			cutAngleStrX.Printf("%.fx%.f",pLineAngle->cut_angle[0][1],pLineAngle->cut_angle[0][0]);
			if(cutWingStr.GetLength()>0)
				cutWingStr.Append(',');
			cutWingStr.Append(cutAngleStrX);
		}
		strcpy(sText,cutWingStr);
	}
	else if(GetPropID("startCutWingY")==id)
	{	//ʼ����֫�н�
		CXhChar50 cutWingStr,cutAngleStrY;
		if(pLineAngle->cut_wing[0]==2)	
		{	//ʼ��Y֫��֫
			cutWingStr.Printf("%.fx%.fx%.f",pLineAngle->cut_wing_para[0][1],
				pLineAngle->cut_wing_para[0][0],pLineAngle->cut_wing_para[0][2]);
		}
		if(pLineAngle->cut_angle[1][0]>0)
		{	//ʼ��Y֫�н�
			cutAngleStrY.Printf("%.fx%.f",pLineAngle->cut_angle[1][1],pLineAngle->cut_angle[1][0]);
			if(cutWingStr.GetLength()>0)
				cutWingStr.Append(',');
			cutWingStr.Append(cutAngleStrY);
		}
		strcpy(sText,cutWingStr);
	}
	else if(GetPropID("endCutWingX")==id)
	{	//�ն���֫�н�
		CXhChar50 cutWingStr,cutAngleStrX;
		if(pLineAngle->cut_wing[1]==1)	
		{	//�ն�X֫��֫
			cutWingStr.Printf("%.fx%.fx%.f",pLineAngle->cut_wing_para[1][1],
				pLineAngle->cut_wing_para[1][0],pLineAngle->cut_wing_para[1][2]);
		}
		if(pLineAngle->cut_angle[2][0]>0)
		{	//�ն�X֫�н�
			cutAngleStrX.Printf("%.fx%.f",pLineAngle->cut_angle[2][1],pLineAngle->cut_angle[2][0]);
			if(cutWingStr.GetLength()>0)
				cutWingStr.Append(',');
			cutWingStr.Append(cutAngleStrX);
		}
		strcpy(sText,cutWingStr);
	}
	else if(GetPropID("endCutWingY")==id)
	{	//�ն���֫�н�
		CXhChar50 cutWingStr,cutAngleStrY;
		if(pLineAngle->cut_wing[1]==2)	
		{	//�ն�Y֫��֫
			cutWingStr.Printf("%.fx%.fx%.f",pLineAngle->cut_wing_para[1][1],
				pLineAngle->cut_wing_para[1][0],pLineAngle->cut_wing_para[1][2]);
		}
		if(pLineAngle->cut_angle[3][0]>0)
		{	//�ն�Y֫�н�
				cutAngleStrY.Printf("%.fx%.f",pLineAngle->cut_angle[3][1],pLineAngle->cut_angle[3][0]);
			if(cutWingStr.GetLength()>0)
				cutWingStr.Append(',');
			cutWingStr.Append(cutAngleStrY);
		}
		strcpy(sText,cutWingStr);
	}
	/*else if(GetPropID("m_bUserSpecStartCutAngle")==id)
	{
		if(pLineAngle->m_bUserSpecStartCutAngle)
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
	else if(GetPropID("start_cut_angle_type")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(sText,"6.Default");	//Ĭ��Ϊ����֫�н�
#else
		strcpy(sText,"6.����֫�н�");	//Ĭ��Ϊ����֫�н�
#endif
		if(pLineAngle->cut_angle[0][0]<EPS&&pLineAngle->cut_angle[0][1]<EPS&&
			pLineAngle->cut_angle[1][0]<EPS&&pLineAngle->cut_angle[1][1]<EPS)
		{	//����֫���
			if(pLineAngle->cut_wing[0]==1)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"3.X Leg Cut");
			else if(pLineAngle->cut_wing[0]==2)
				strcpy(sText,"4.Y Leg Cut");
#else
				strcpy(sText,"3.X֫��֫");
			else if(pLineAngle->cut_wing[0]==2)
				strcpy(sText,"4.Y֫��֫");
#endif
		}
		else if(pLineAngle->cut_wing[0]==0&&(pLineAngle->cut_angle[0][0]>0||pLineAngle->cut_angle[1][0]>0))
		{	//���н����
			if(pLineAngle->cut_angle[1][0]>0&&fabs(pLineAngle->cut_angle[1][0]-pLineAngle->cut_angle[1][1])<EPS
				&&fabs(pLineAngle->cut_angle[0][0])<EPS&&fabs(pLineAngle->cut_angle[0][1])<EPS)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"1.Y Leg Cut 45��");
			else if(pLineAngle->cut_angle[0][0]>0&&fabs(pLineAngle->cut_angle[0][0]-pLineAngle->cut_angle[0][1])<EPS
				&&fabs(pLineAngle->cut_angle[1][0])<EPS&&fabs(pLineAngle->cut_angle[1][1])<EPS)
				strcpy(sText,"0.X Leg Cut 45��");
			else 
				strcpy(sText,"2.Leg Cut");
#else
				strcpy(sText,"1.Y֫45���н�");
			else if(pLineAngle->cut_angle[0][0]>0&&fabs(pLineAngle->cut_angle[0][0]-pLineAngle->cut_angle[0][1])<EPS
				&&fabs(pLineAngle->cut_angle[1][0])<EPS&&fabs(pLineAngle->cut_angle[1][1])<EPS)
				strcpy(sText,"0.X֫45���н�");
			else 
				strcpy(sText,"2.��ͨ�н�");
#endif
		}
		else if(pLineAngle->cut_wing[0]>0&&(pLineAngle->cut_angle[0][0]>0||pLineAngle->cut_angle[0][1]>0||
			pLineAngle->cut_angle[1][0]>0||pLineAngle->cut_angle[1][1]>0))
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"5.Complex Leg Cut");
#else
			strcpy(sText,"5.������֫�н�");
#endif
	}
	else if(GetPropID("cut_wing_para[0][0]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_wing_para[0][0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_wing_para[0][1]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_wing_para[0][1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_wing_para[0][2]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_wing_para[0][2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_angle[0][0]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_angle[0][0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_angle[0][1]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_angle[0][1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_angle[1][0]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_angle[1][0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_angle[1][1]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_angle[1][1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecEndCutAngle")==id)
	{
		if(pLineAngle->m_bUserSpecEndCutAngle)
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
	else if(GetPropID("end_cut_angle_type")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		strcpy(sText,"6.Default");	//Ĭ��Ϊ����֫�н�
#else
		strcpy(sText,"6.����֫�н�");	//Ĭ��Ϊ����֫�н�
#endif
		if(pLineAngle->cut_angle[2][0]<EPS&&pLineAngle->cut_angle[2][1]<EPS&&
			pLineAngle->cut_angle[3][0]<EPS&&pLineAngle->cut_angle[3][1]<EPS)
		{	//����֫���
			if(pLineAngle->cut_wing[1]==1)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"3.X Leg Cut");
			else if(pLineAngle->cut_wing[1]==2)
				strcpy(sText,"4.Y Leg Cut");
#else
				strcpy(sText,"3.X֫��֫");
			else if(pLineAngle->cut_wing[1]==2)
				strcpy(sText,"4.Y֫��֫");
#endif
		}
		else if(pLineAngle->cut_wing[1]==0&&(pLineAngle->cut_angle[2][0]>0||pLineAngle->cut_angle[3][0]>0))
		{	//���н����
			if(pLineAngle->cut_angle[3][0]>0&&fabs(pLineAngle->cut_angle[3][0]-pLineAngle->cut_angle[3][1])<EPS
				&&fabs(pLineAngle->cut_angle[2][0])<EPS&&fabs(pLineAngle->cut_angle[2][1])<EPS)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(sText,"1.Y Leg Cut 45��");
			else if(pLineAngle->cut_angle[2][0]>0&&fabs(pLineAngle->cut_angle[2][0]-pLineAngle->cut_angle[2][1])<EPS
				&&fabs(pLineAngle->cut_angle[3][0])<EPS&&fabs(pLineAngle->cut_angle[3][1])<EPS)
				strcpy(sText,"0.X Leg Cut 45��");
			else 
				strcpy(sText,"2.Leg Cut");
#else
				strcpy(sText,"1.Y֫45���н�");
			else if(pLineAngle->cut_angle[2][0]>0&&fabs(pLineAngle->cut_angle[2][0]-pLineAngle->cut_angle[2][1])<EPS
				&&fabs(pLineAngle->cut_angle[3][0])<EPS&&fabs(pLineAngle->cut_angle[3][1])<EPS)
				strcpy(sText,"0.X֫45���н�");
			else 
				strcpy(sText,"2.��ͨ�н�");
#endif
		}
		else if(pLineAngle->cut_wing[1]>0&&(pLineAngle->cut_angle[2][0]>0||pLineAngle->cut_angle[2][1]>0||
			pLineAngle->cut_angle[3][0]>0||pLineAngle->cut_angle[3][1]>0))
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"5.Complex Cut Leg");
#else
			strcpy(sText,"5.������֫�н�");
#endif
	}
	else if(GetPropID("cut_wing_para[1][0]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_wing_para[1][0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_wing_para[1][1]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_wing_para[1][1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_wing_para[1][2]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_wing_para[1][2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_angle[2][0]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_angle[2][0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_angle[2][1]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_angle[2][1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_angle[3][0]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_angle[3][0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("cut_angle[3][1]")==id)
	{
		sprintf(sText,"%f",pLineAngle->cut_angle[3][1]);
		SimplifiedNumString(sText);
	}*/
	else if(GetPropID("m_bBendPart")==id)
	{
		if(pLineAngle->bHuoQuStart||pLineAngle->bHuoQuEnd)
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
#ifdef __LDS_
	else if(GetPropID("size.idClassType")==id)
	{
		if(pLineAngle->size_idClassType==CLS_LINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText, "Same Leg Angle");
		else if(pLineAngle->size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(pLineAngle->size_cSubClassType=='T')
				strcpy(sText, "T Group");
			else if(pLineAngle->size_cSubClassType=='D')
				strcpy(sText, "Diagonal Group");
			else if(pLineAngle->size_cSubClassType=='X')
				strcpy(sText, "Cross Group");
#else
			strcpy(sText, "��֫�Ǹ�");
		else if(pLineAngle->size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(pLineAngle->size_cSubClassType=='T')
				strcpy(sText, "T�����");
			else if(pLineAngle->size_cSubClassType=='D')
				strcpy(sText, "�Խ����");
			else if(pLineAngle->size_cSubClassType=='X')
				strcpy(sText, "ʮ�����");
#endif
		}
		else if(pLineAngle->size_idClassType==CLS_LINETUBE)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText, "Tube");
		else if(pLineAngle->size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "Flat");
		else if(pLineAngle->size_idClassType==CLS_LINESLOT)
			strcpy(sText, "Slot");
		else if(pLineAngle->size_idClassType==0)
			strcpy(sText, "User-Defined");
#else
			strcpy(sText, "�ֹ�");
		else if(pLineAngle->size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "���");
		else if(pLineAngle->size_idClassType==CLS_LINESLOT)
			strcpy(sText, "�۸�");
		else if(pLineAngle->size_idClassType==0)
			strcpy(sText, "�û�����");
#endif
	}
	else if(GetPropID("m_uStatMatNo")==id)
		sprintf(sText,"%d",pLineAngle->m_uStatMatNo);
	else if(GetPropID("m_fWeightRaiseCoef")==id)
	{
		sprintf(sText,"%f",pLineAngle->m_fWeightRaiseCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iElemType")==id)
	{
		if(pLineAngle->m_iElemType==0)
			strcpy(sText,"0.�Զ��ж�");
		else if(pLineAngle->m_iElemType==1)
			strcpy(sText,"1.������");
		else //if(pLineAngle->m_iElemType==2)
			strcpy(sText,"2.������");
	}
	else if(GetPropID("sel_mat_method")==id)	//�Զ��жϵ�Ԫ����
	{
		if(!pLineAngle->IsAuxPole())
			strcpy(sText,"");
		else if(pLineAngle->sel_mat_method==AUX_AUTODESIGN)
			strcpy(sText,"�Զ����");
		else if(pLineAngle->sel_mat_method==AUX_BY_START)
			strcpy(sText,"����ʼ��");
		else if(pLineAngle->sel_mat_method==AUX_BY_END)
			strcpy(sText,"�����ն�");
		else if(pLineAngle->sel_mat_method==AUX_ONLY_PRIMARY)
			strcpy(sText,"����б��");
		else
			strcpy(sText,"");
	}
	else if(GetPropID("m_bNeedSelMat")==id)
	{
		if(pLineAngle->m_bNeedSelMat)
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
	else if(GetPropID("m_bNeedSelSize")==id)
	{
		if(pLineAngle->m_bNeedSelSize)
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
	else if(GetPropID("hole_out")==id)
	{
		sprintf(sText,"%f",pLineAngle->hole_out);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bAutoCalHoleOutCoef")==id)
	{
		if(pLineAngle->m_bAutoCalHoleOutCoef)
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
	else if(GetPropID("CalLenCoef.iTypeNo")==id)
	{
		if(pLineAngle->CalLenCoef.iTypeNo==0)
			strcpy(sText,"0.�����ж�");
		else if(pLineAngle->CalLenCoef.iTypeNo==1)
			strcpy(sText,"1.Vб��(��Ų�����)");
		else if(pLineAngle->CalLenCoef.iTypeNo==2)
			strcpy(sText,"2.Vб��(V������)");
		else if(pLineAngle->CalLenCoef.iTypeNo==3)
			strcpy(sText,"3.Vб��3�ڼ��������");
		else if(pLineAngle->CalLenCoef.iTypeNo==4)
			strcpy(sText,"4.Vб��4�ڼ��������");
		else if(pLineAngle->CalLenCoef.iTypeNo==5)
			strcpy(sText,"5.ָ���˼����㳤��");
	}
	else if(GetPropID("CalLenCoef.pRefPole")==id)
	{
		if(pLineAngle->CalLenCoef.hRefPole>0x20)
			sprintf(sText,"0X%X",pLineAngle->CalLenCoef.hRefPole);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("CalLenCoef.pHorizPole")==id)
	{
		if(pLineAngle->CalLenCoef.hHorizPole>0x20)
			sprintf(sText,"0X%X",pLineAngle->CalLenCoef.hHorizPole);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("CalLenCoef.spec_callenMinR")==id)
	{
		sprintf(sText,"%f",pLineAngle->CalSpecLamdaLenMinR());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.minR.hStartNode")==id)
	{
		sprintf(sText,"0X%X",pLineAngle->CalLenCoef.minR.hStartNode);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.minR.hEndNode")==id)
	{
		sprintf(sText,"0X%X",pLineAngle->CalLenCoef.minR.hEndNode);
	}
	else if(GetPropID("CalLenCoef.minR.coef")==id)
	{
		sprintf(sText,"%f",pLineAngle->CalLenCoef.minR.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.spec_callenParaR")==id)
	{
		sprintf(sText,"%f",pLineAngle->CalSpecLamdaLenParaR());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.paraR.hStartNode")==id)
	{
		sprintf(sText,"0X%X",pLineAngle->CalLenCoef.paraR.hStartNode);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.paraR.hEndNode")==id)
	{
		sprintf(sText,"0X%X",pLineAngle->CalLenCoef.paraR.hEndNode);
	}
	else if(GetPropID("CalLenCoef.paraR.coef")==id)
	{
		sprintf(sText,"%f",pLineAngle->CalLenCoef.paraR.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_joint_type")==id)
	{
		if(pLineAngle->start_joint_type==JOINT_HINGE)
			strcpy(sText,"�½�");
		else if(pLineAngle->start_joint_type==JOINT_RIGID)
			strcpy(sText,"�ս�");
		else //if(pLineAngle->start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"�Զ��ж�");
	}
	else if(GetPropID("start_force_type")==id)
	{
		if(pLineAngle->start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"ƫ������");
		else //if(pLineAngle->start_force_type==CENTRIC_FORCE)
			strcpy(sText,"��������");
	}
	else if(GetPropID("end_joint_type")==id)
	{
		if(pLineAngle->end_joint_type==JOINT_HINGE)
			strcpy(sText,"�½�");
		else if(pLineAngle->end_joint_type==JOINT_RIGID)
			strcpy(sText,"�ս�");
		else //if(pLineAngle->end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"�Զ��ж�");
	}
	else if(GetPropID("end_force_type")==id)
	{
		if(pLineAngle->end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"ƫ������");
		else //if(pLineAngle->end_force_type==CENTRIC_FORCE)
			strcpy(sText,"��������");
	}
#endif
	else if(GetPropID("m_bDisplayHole")==id)
	{
		if(pLineAngle->m_bDisplayHole)
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
	else if(GetPropID("m_bVirtualPart")==id)
	{
		if(pLineAngle->m_bVirtualPart)
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
	else if(GetPropID("m_bFootNail")==id)
	{
		if(pLineAngle->m_bFootNail)
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
	else if(GetPropID("length")==id)
	{
		sprintf(sText,"%f",pLineAngle->GetLength());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("sNotes")==id)
		sprintf(sText,"%s",pLineAngle->sNotes);
	else if(GetPropID("m_bCutRoot")==id)
	{
		if(pLineAngle->m_bCutRoot)
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
	else if(GetPropID("m_bCutBer")==id)
	{
		if(pLineAngle->m_bCutBer)
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
	else if(GetPropID("m_bWeldPart")==id)
	{
		if(pLineAngle->m_bWeldPart)
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
	else if(GetPropID("m_hArcLift")==id)		//Ԥ����
	{
		CLDSArcLift *pLift=pLineAngle->BelongModel()->FromArcLiftHandle(pLineAngle->m_hArcLift);
		if(pLift)
		{
			CString sLiftName;
			sLiftName.Format("%s(%d)",pLift->name,pLift->iNo);
			strncpy(sText,sLiftName,50);
		}
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"None");
#else
			strcpy(sText,"��Ԥ��");
#endif
	}
	else if(GetPropID("m_xStartBeforeLift.x")==id)	//���Ԥ��ǰλ��
	{
		sprintf(sText,"%f",pLineAngle->m_xStartBeforeLift.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xStartBeforeLift.y")==id)	
	{
		sprintf(sText,"%f",pLineAngle->m_xStartBeforeLift.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xStartBeforeLift.z")==id)	
	{
		sprintf(sText,"%f",pLineAngle->m_xStartBeforeLift.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xEndBeforeLift.x")==id)	//�ն�Ԥ��ǰλ��
	{
		sprintf(sText,"%f",pLineAngle->m_xEndBeforeLift.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xEndBeforeLift.y")==id)	
	{
		sprintf(sText,"%f",pLineAngle->m_xEndBeforeLift.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_xEndBeforeLift.z")==id)	
	{
		sprintf(sText,"%f",pLineAngle->m_xEndBeforeLift.z);
		SimplifiedNumString(sText);
	}
	else
		strcpy(sText,"");
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		pLineAngle->Log2File()->Log("0X%X Angle's property##d string value out of length",pLineAngle->handle,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		pLineAngle->Log2File()->Log("�Ǹ�0X%X����#%d�ַ���������������",pLineAngle->handle,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}

int CLDSLineAngle::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	return CLDSLineAngle::GetPropValueStr(this,id,valueStr,nMaxStrBufLen);
}

long CLDSLineAngle::GetPropID(char* propStr)
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
	{
		/*
#ifdef AFX_TARG_ENU_ENGLISH
		CString prompt="Didn't find property ID named\"";
		prompt+=propStr;
		prompt+="\"��";
#else
		CString prompt="û�ҵ���Ϊ\"";
		prompt+=propStr;
		prompt+="\"������ID��";
#endif
		AfxMessageBox(prompt);
		*/
		return 0;
	}
}

BOOL CLDSLineAngle::IsPropEqual(HASHOBJSET &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSLineAngle *pLineAngle=NULL, *pFirstLineAngle = (CLDSLineAngle*)selectObjs.GetFirst();
	pFirstLineAngle->GetPropValueStr(idProp,valueStr);
	for(pLineAngle=(CLDSLineAngle*)selectObjs.GetNext();pLineAngle;pLineAngle=(CLDSLineAngle*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pLineAngle->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSLineAngle::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSLineAngle::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CLDSLineAngle::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSLineAngle::propStatusHashtable.SetValueAt(id,dwStatus);
}

BOOL CLDSLineAngle::GetObjPropReadOnlyDefaultState(long id)
{
	return GetObjPropReadOnlyDefaultState(id,ShadowMode());
}
BOOL CLDSLineAngle::GetObjPropReadOnlyDefaultState(long id,BYTE shadow_mode)
{
	if (shadow_mode==CLDSObject::SHADOWMODE_COMMON)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else if(GetPropID("cMaterial")==id)
			return FALSE;
		else if(GetPropID("m_cQualityLevel")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else if(GetPropID("cfgword")==id)
			return FALSE;
		else if(GetPropID("sPartNo")==id)
			return FALSE;
		else if(GetPropID("iSeg")==id)
			return FALSE;
		else if(GetPropID("m_uStatMatCoef")==id)
			return FALSE;
		else if(GetPropID("relativeObjs")==id)
			return FALSE;
		else if(GetPropID("sNotes")==id)
			return FALSE;
		else if(GetPropID("SetupPosInfo")==id)
			return FALSE;
		else if(GetPropID("m_bEnableTeG")==id)
			return TRUE;
		else if(GetPropID("teGTable")==id)
			return TRUE;
		else if(GetPropID("m_hArcLift")==id)
			return TRUE;
		else
			return TRUE;
	}
	else if(shadow_mode==CLDSObject::SHADOWMODE_BLOCK)
	{
		if(GetPropID("layer")==id)
			return FALSE;
		else if(GetPropID("m_hPartWeldParent")==id)
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(GetPropID("handle")==id)
			return TRUE;
		else if(GetPropID("length")==id)
			return TRUE;
		//���Ǹ�����group_style����
		//else if(GetPropID("group_style")==id)
		//	return TRUE;
		else if(GetPropID("decWingAngle")==id)
			return TRUE;
		else if(GetPropID("m_bBendPart")==id)
			return TRUE;
		else if(GetPropID("work_wing")==id)
			return TRUE;
		else
			return FALSE;
	}
}

DWORD CLDSLineAngle::SetPermission(DWORD dwPerm)
{
	dwPermission=dwStartPermission=dwEndPermission=dwPerm;
	return dwPerm;
}

void CLDSLineAngle::ClearFlag()
{
	UnlockStart();
	UnlockEnd();
	_cnCalNormHits = 0;
}

CLDSLineAngle & CLDSLineAngle::operator=(const CLDSLineAngle &jg)
{
	CLsRefList *pOldLsRefList = pLsRefList;
	CSolidBody *pOldSolidBody = pSolidBody;
	memmove(this,&jg,sizeof(CLDSLineAngle));
	pLsRefList = pOldLsRefList;
	pSolidBody = pOldSolidBody;
	return *this;
}
void CLDSLineAngle::SetModified(BOOL bTrue/*=TRUE*/,BOOL bRenderSolid/*=TRUE*/)
{
	is_data_modified = (bTrue!=0);
	if(bTrue)
	{
		is_undo_modified = TRUE;
		is_solid_modified|=(bRenderSolid!=0);
	}
	else
		is_solid_modified=bRenderSolid&&is_data_modified;
	if(m_pModel)
	{
		SetDocModifiedFlag();
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(bTrue&&huoqu_handle>0x20)
		{
			//���������б�ǰӦ��ѹջ wht 12-03-05
			BOOL pushed=m_pModel->PushPartStack();
			for(CLDSPolyJoint *pJoint=(CLDSPolyJoint*)m_pModel->EnumPartFirst(CLS_POLYJOINT);
			pJoint;pJoint=(CLDSPolyJoint*)m_pModel->EnumPartNext(CLS_POLYJOINT))
			{
				if(pJoint->pPrevLinePart==this||pJoint->pNextLinePart==this)
					pJoint->SetModified();
			}
			m_pModel->PopPartStack(pushed);
		}
#endif
	}
}

f3dPoint CLDSLineAngle::get_start_in_ber()
{
	f3dPoint bi_vec = norm_x_wing+norm_y_wing;
	normalize(bi_vec);
	double cosa=bi_vec*norm_x_wing;
	double offset = size_thick/cosa;
	return Start()-bi_vec*offset;
}

f3dPoint CLDSLineAngle::get_end_in_ber()
{
	f3dPoint bi_vec = norm_x_wing+norm_y_wing;
	normalize(bi_vec);
	double cosa=bi_vec*norm_x_wing;
	double offset = size_thick/cosa;
	return End()-bi_vec*offset;
}
BOOL CLDSLineAngle::IsLsInWing(CLDSBolt *pLs,int *x_wing0_y_wing1/*=NULL*/,bool bNeglectLenLimit/*=false*/,
		double* pRsltIntersPoint/*=NULL*/)
{
	if(pLs==NULL)
		return FALSE;
	int wingx0_y1=0;
	GEPOINT inters,xLocalPoint;
	if(!GetBoltIntersPos(pLs,inters,&wingx0_y1))
		return false;
	if (pRsltIntersPoint)
	{
		pRsltIntersPoint[0] = inters.x;
		pRsltIntersPoint[1] = inters.y;
		pRsltIntersPoint[2] = inters.z;
	}
	if(x_wing0_y_wing1)
		*x_wing0_y_wing1=wingx0_y1;
	GEPOINT lenStdVec=this->xPosEnd-this->xPosStart;
	double length=lenStdVec.mod();
	lenStdVec/=length;
	GEPOINT vOffsetVec=inters-this->xPosStart;
	xLocalPoint.z=vOffsetVec*lenStdVec;
	if(!bNeglectLenLimit&&(xLocalPoint.z<-this->start_oddment||xLocalPoint.z>length+this->end_oddment))
		return false;	//�������ȷ�Χ
	if(wingx0_y1==0)
	{
		xLocalPoint.x=vOffsetVec*this->GetWingVecX();
		if(xLocalPoint.x<0||xLocalPoint.x>size.wide)
			return false;
	}
	else
	{
		xLocalPoint.y=vOffsetVec*this->GetWingVecY();
		if(xLocalPoint.y<0||xLocalPoint.y>GetWidthWingY())
			return false;
	}
	return true;
	/*f3dPoint ls_pos=pLs->ucs.origin;
	UCS_STRU ucs;
	double length=DISTANCE(Start(),End());
	double R=pLs->get_d()/2.0;
	double H;
	if(pLs->get_d()==12)
		H=8;
	else if(pLs->get_d()==16)
		H=10;
	else if(pLs->get_d()==20)
		H=13;
	else// if(pLs->get_d()==24)
		H=15;
	if(fabs(pLs->get_norm()*this->vxWingNorm)>EPS_COS2)//get_norm_x_wing())>EPS_COS2)	//������X֫�ϵ���˨
	{
		getUCS(ucs,0);
		coord_trans(ls_pos,ucs,FALSE);
		if(ls_pos.z<-startOdd()-R||ls_pos.z>length+endOdd()+R)
			return FALSE;		//0##�������ȷ�Χ
		if( ls_pos.x>0&&ls_pos.x<GetWidth())
		{
			if(ls_pos.y>-H&&ls_pos.y<GetWidth())
			{
				if(x_wing0_y_wing1)
					*x_wing0_y_wing1=0;
				return TRUE;
			}
			if(pLs->get_norm()*get_norm_x_wing()>0)	//�������⴩
				ls_pos.y-=GetWidth();
			else									//�������ﴩ
				ls_pos.y+=GetWidth();
			if(ls_pos.y>-H&&ls_pos.y<GetWidth())
			{
				if(x_wing0_y_wing1)
					*x_wing0_y_wing1=0;
				return TRUE;
			}
			else
				return FALSE;	//1##������˨��ȷ�Χ
		}
		else
			return FALSE;		//2##����֫��Χ
	}
	if(fabs(pLs->get_norm()*get_norm_y_wing())>EPS_COS2)	//������Y֫�ϵ���˨
	{
		getUCS(ucs,1);
		coord_trans(ls_pos,ucs,FALSE);
		if(ls_pos.z<-startOdd()||ls_pos.z>length+endOdd())
			return FALSE;		//0##�������ȷ�Χ
		if( ls_pos.y>0&&ls_pos.y<GetWidth())
		{//����Ĵ�����Ҫ����������˨�ڽǸ�֫�ڵķ�Χ
			if(ls_pos.x>-H&&ls_pos.x<GetWidth())
			{
				if(x_wing0_y_wing1)
					*x_wing0_y_wing1=1;
				return TRUE;
			}
			if(pLs->get_norm()*get_norm_y_wing()>0)	//�������⴩
				ls_pos.x-=GetWidth();
			else									//�������ﴩ
				ls_pos.x+=GetWidth();
			if(ls_pos.x>-H&&ls_pos.x<GetWidth())
			{
				if(x_wing0_y_wing1)
					*x_wing0_y_wing1=1;
				return TRUE;
			}
			else
				return FALSE;	//1##������˨��ȷ�Χ
		}
		else
			return FALSE;		//2##����֫��Χ
	}
	return FALSE;				//3##���򲻷�
	*/
}

f3dPoint CLDSLineAngle::set_norm_x_wing(f3dPoint norm_x,BOOL bSpecific)
{
	des_norm_x.bSpecific = bSpecific;
	if(des_norm_x.bSpecific)
		des_norm_x.spec_norm.vector=norm_x;
	if(norm_x.mod()<eps)
		return norm_x_wing;
	else
	{
		//����֫����
		f3dPoint axis_z = End()-Start();
		if(axis_z.mod()>EPS)
		{
			f3dPoint axis_y = axis_z^norm_x;
			norm_x = axis_y^axis_z;
		}

		norm_x_wing = norm_x;
		normalize(norm_x_wing);
		des_norm_x.spec_norm.vector=norm_x_wing;
		SetModified(TRUE);
		UpdateParentGroupAngleByWingNormX();
		return norm_x_wing;
	}
}

f3dPoint CLDSLineAngle::get_norm_x_wing()
{
	//����֫����
	f3dPoint axis_z = End()-Start();
	if(axis_z.mod()>EPS)
	{
		f3dPoint axis_y = axis_z^norm_x_wing;
		norm_x_wing = axis_y^axis_z;
	}
	normalize(norm_x_wing);
	return norm_x_wing;	
}

f3dPoint CLDSLineAngle::set_norm_y_wing(f3dPoint norm_y,BOOL bSpecific)
{
	des_norm_y.bSpecific = bSpecific;
	if(des_norm_y.bSpecific)
		des_norm_y.spec_norm.vector=norm_y;
	if(norm_y.mod()<eps)
		return norm_y_wing;
	else
	{
		//����֫����
		f3dPoint axis_z = End()-Start();
		if(axis_z.mod()>EPS)
		{
			f3dPoint axis_x = norm_y^axis_z;
			norm_y = axis_z^axis_x;
		}

		norm_y_wing = norm_y;
		normalize(norm_y_wing);
		des_norm_y.spec_norm.vector=norm_y_wing;
		SetModified(TRUE);
		UpdateParentGroupAngleByWingNormY();
		return norm_y_wing;
	}
}
void CLDSLineAngle::SetBriefWingNormX(const double* wingnorm,bool bToSpecNorm/*=true*/)
{
	GEPOINT wing_norm(wingnorm);
	if(norm_x_wing*wing_norm>0.999998)	//cos(1��)=0.999998
		bToSpecNorm=false;	//��𲻴�ʱ�ݲ�ǿ����Ϊָ�����ߣ����ⶪʧ��ϽǸֱ������Ʋ���
	des_norm_x.spec_norm.vector=des_norm_x.near_norm=norm_x_wing=wing_norm;
	if(bToSpecNorm)
		des_norm_x.bSpecific=TRUE;
	SetModified();
}
void CLDSLineAngle::SetBriefWingNormY(const double* wingnorm,bool bToSpecNorm/*=true*/)
{
	GEPOINT wing_norm(wingnorm);
	if(norm_y_wing*wing_norm>0.999998)	//cos(1��)=0.999998
		bToSpecNorm=false;	//��𲻴�ʱ�ݲ�ǿ����Ϊָ�����ߣ����ⶪʧ��ϽǸֱ������Ʋ���
	des_norm_y.spec_norm.vector=des_norm_y.near_norm=norm_y_wing=wing_norm;
	if(bToSpecNorm)
		des_norm_y.bSpecific=TRUE;
	SetModified();
}
bool CLDSLineAngle::UpdateParentGroupAngleByWingNormX()
{
	if(group_father_jg_h<=0x20)
		return false;
	CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(group_father_jg_h,CLS_GROUPLINEANGLE);
	if(pGroupAngle==NULL)
	{
		Log2File()->Log("0x%X�Ǹ���Ϊ�ӽǸ֣���������ϽǸ�0x%X�ѱ�ɾ��!",handle,group_father_jg_h);
		return false;
	}
	if(pGroupAngle->ciMasterDirectIndex<=0||pGroupAngle->son_jg_h[pGroupAngle->ciMasterDirectIndex-1]!=handle)
		return false;	//��ǰ�ӽǸֲ�����ϽǸֵ�ǰ֫���߼����׼�ӽǸ�
	BYTEFLAG flag(0xff);
	int i;
	for(i=0;pGroupAngle&&i<4;i++)
	{
		if(pGroupAngle->son_jg_h[i]==handle)
		{
			flag.SetBitState(i,false);
			break;
		}
	}
	if((BYTE)flag==0xff)
		return false;
	if(pGroupAngle->size_cSubClassType=='D')
	{
		if(i==0)
			pGroupAngle->SetBriefWingNormX(norm_x_wing);
		else //if(i==1)
			pGroupAngle->SetBriefWingNormX(-norm_x_wing);
	}
	else if(pGroupAngle->size_cSubClassType=='T')
	{
		if(i==pGroupAngle->ciMasterIndex)
			pGroupAngle->SetBriefWingNormX(norm_x_wing);
		else if(i==0)	//1��Ϊ������׼�ӽǸ�
			pGroupAngle->SetBriefWingNormY(-norm_x_wing);
		else //if(i==1)
			pGroupAngle->SetBriefWingNormY(norm_x_wing);
	}
	else if(pGroupAngle->size_cSubClassType=='X')
	{
		if(i==0)
			pGroupAngle->SetBriefWingNormX( norm_x_wing);
		else if(i==1)
			pGroupAngle->SetBriefWingNormY( norm_x_wing);
		else if(i==2)
			pGroupAngle->SetBriefWingNormX(-norm_x_wing);
		else //if(i==3)
			pGroupAngle->SetBriefWingNormY(-norm_x_wing);
	}
	pGroupAngle->UpdateSonJgWingNorm(flag);
	return true;
}
bool CLDSLineAngle::UpdateParentGroupAngleByWingNormY()
{
	if(group_father_jg_h<=0x20)
		return false;
	CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(group_father_jg_h,CLS_GROUPLINEANGLE);
	if(pGroupAngle==NULL)
	{
		Log2File()->Log("0x%X�Ǹ���Ϊ�ӽǸ֣���������ϽǸ�0x%X�ѱ�ɾ��!",handle,group_father_jg_h);
		return false;
	}
	if(pGroupAngle->ciMasterDirectIndex<=0||pGroupAngle->son_jg_h[pGroupAngle->ciMasterDirectIndex-1]!=handle)
		return false;	//��ǰ�ӽǸֲ�����ϽǸֵ�ǰ֫���߼����׼�ӽǸ�
	BYTEFLAG flag(0xff);
	int i;
	for(i=0;pGroupAngle&&i<4;i++){
		if(pGroupAngle->son_jg_h[i]==handle)
		{
			flag.SetBitState(i,false);
			break;
		}
	}
	if((BYTE)flag==0xff)
		return false;
	if(pGroupAngle->size_cSubClassType=='D')
	{
		if(i==0)
			pGroupAngle->SetBriefWingNormY(norm_y_wing);
		else //if(i==1)
			pGroupAngle->SetBriefWingNormY(-norm_y_wing);
	}
	else if(pGroupAngle->size_cSubClassType=='T')
	{
		if(i==pGroupAngle->ciMasterIndex)
			pGroupAngle->SetBriefWingNormY(norm_y_wing);
		else if(i==0)	//1��Ϊ������׼�ӽǸ�
			pGroupAngle->SetBriefWingNormX( norm_y_wing);
		else //if(i==1)
			pGroupAngle->SetBriefWingNormX(-norm_y_wing);
	}
	else if(pGroupAngle->size_cSubClassType=='X')
	{
		if(i==0)
			pGroupAngle->SetBriefWingNormY( norm_y_wing);
		else if(i==1)
			pGroupAngle->SetBriefWingNormX(-norm_y_wing);
		else if(i==2)
			pGroupAngle->SetBriefWingNormY(-norm_y_wing);
		else //if(i==3)
			pGroupAngle->SetBriefWingNormX( norm_y_wing);
	}
	pGroupAngle->UpdateSonJgWingNorm(flag);
	return true;
}

f3dPoint CLDSLineAngle::get_norm_y_wing()
{	
	//����֫����
	f3dPoint axis_z = End()-Start();
	if(axis_z.mod()>EPS)
	{
		f3dPoint axis_x = norm_y_wing^axis_z;
		norm_y_wing = axis_z^axis_x;
	}
	normalize(norm_y_wing);
	return norm_y_wing;	
}
f3dPoint CLDSLineAngle::GetWingVecX()
{
	f3dPoint axis_z = End()-Start();
	f3dPoint wing_vec = axis_z^norm_x_wing;
	f3dPoint vStdNormal=wing_vec.normalized();
	if(vStdNormal.x>-EPS&&vStdNormal.x<0)
		vStdNormal.x=0;
	if(vStdNormal.y>-EPS&&vStdNormal.y<0)
		vStdNormal.y=0;
	if(vStdNormal.z>-EPS&&vStdNormal.z<0)
		vStdNormal.z=0;
	return vStdNormal;
}

f3dPoint CLDSLineAngle::GetWingVecY()
{
	f3dPoint axis_z = End()-Start();
	f3dPoint wing_vec = norm_y_wing^axis_z;
	f3dPoint vStdNormal=wing_vec.normalized();
	if(vStdNormal.x>-EPS&&vStdNormal.x<0)
		vStdNormal.x=0;
	if(vStdNormal.y>-EPS&&vStdNormal.y<0)
		vStdNormal.y=0;
	if(vStdNormal.z>-EPS&&vStdNormal.z<0)
		vStdNormal.z=0;
	return vStdNormal;
}

f3dPoint CLDSLineAngle::GetDatumPos(CLDSNode* pNode){return GetDatumPosBer(pNode,0);}
bool CLDSLineAngle::TestVisitLockLoop(long hNode)
{
	//TODO:��δ������arrDatumBerLocks��PRESET_ARRAY����Ӧ���ϡ�wjh-2018.1.25
	for(int i=0;i<4;i++)
	{
		long hLockNode=arrDatumBerLocks.At(i);
		if(hLockNode==hNode)
			return true;
		else if(hLockNode==0)
		{
			arrDatumBerLocks[i]=hNode;
			return false;
		}
	}
	return false;
}
bool CLDSLineAngle::IsLenOffsetNode(CLDSNode *pNode)
{
	if(pNode->m_cPosCalType!=CLDSNode::COORD_OFFSET||pStart==NULL||pEnd==NULL)
		return false;
	CLDSNode* pDatumNode=BelongModel()->FromNodeHandle(pNode->arrRelationNode[0]);
	CLDSNode* pLenStartNode=BelongModel()->FromNodeHandle(pNode->arrRelationNode[1]);
	CLDSNode* pLenEndNode=BelongModel()->FromNodeHandle(pNode->arrRelationNode[2]);
	if(pDatumNode==NULL||pLenStartNode==NULL||pLenEndNode==NULL)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("0X%X offset node's parameter is missing!",pNode->handle);
#else 
		logerr.Log("0X%Xƫ�ƽڵ��������׼�ڵ����ʼ�˽ڵ����ֹ�˽ڵ㣩ȱʧ!",pNode->handle);
#endif
		return false;
	}
	GEPOINT lenStdVec=pEnd->xOriginalPos-pStart->xOriginalPos;
	normalize(lenStdVec);
	GEPOINT lenStdVec2=pLenEndNode->xOriginalPos-pLenStartNode->xOriginalPos;
	normalize(lenStdVec2);
	GEPOINT dv=pDatumNode->xOriginalPos-pStart->xOriginalPos;
	dv-=(dv*lenStdVec)*lenStdVec;
	if(!dv.IsZero())	//��ʼ�㲻�ڵ�ǰ�˼�����
		return false;
	if(fabs(lenStdVec*lenStdVec2)<EPS_COS)
		return false;	//���췽���뵱ǰ�˼�����һ��
	return true;
}
//cSpecDatumWing='X'��ʾX֫Ϊ��׼;'Y'��ʾY֫Ϊ��׼������Ϊ���ߣ��ò������ڻ�׼�Ǹ�Ϊ�б���֫�������Ͻǣ�
//�ڻ�ͼʱ�������ͶӰλ��ʱ��Чwjh-2011.10.21
f3dPoint CLDSLineAngle::GetDatumPosBer(CLDSNode *pNode,char cSpecDatumWing/*=0*/,bool bClearLocks/*=true*/)//,BOOL bFatherJgPos/*=TRUE*/)
{
	if(bClearLocks)
		arrDatumBerLocks.ZeroPresetMemory();//memset(arrDatumBerLocks, 0, 16);
	f3dPoint datum_point;
	CLDSArcLift* pLift=NULL;
	if(pNode==pStart||(pNode!=NULL&&pStart!=NULL&&pNode->Position().IsEqual(pStart->Position())))
	{	//�ڵ�Ϊʼ�˽ڵ����ʼ�˽ڵ��غ�(�����ظ��ڵ㵫���Ƕ˽ڵ�������ֵ��ж�ʧ��)
		if(bHuoQuStart)
		{
			SnapPerp(&datum_point,Start(),End(),pre_huoqu_start_xyz,NULL);
			return datum_point;
		}
		else
		{
			double odd=0;
			if(m_cPosCtrlDatumLineType=='X')
				odd=GetStartExtraOdd(0);
			else if(m_cPosCtrlDatumLineType=='Y')
				odd=GetStartExtraOdd(1);
			else
				odd=GetStartExtraOdd(2);
			if(odd==0)
				datum_point = Start();
			else
			{
				f3dPoint direct=Start()-End();
				normalize(direct);
				datum_point = Start()-direct*odd;
			}
		}
	}
	else if(pNode==pEnd||(pNode!=NULL&&pEnd!=NULL&&pNode->Position().IsEqual(pEnd->Position())))
	{	//�ն˽ڵ�
		if(bHuoQuEnd)
		{
			SnapPerp(&datum_point,Start(),End(),pre_huoqu_end_xyz,NULL);
			return datum_point;
		}
		else
		{
			double odd=0;
			if(m_cPosCtrlDatumLineType=='X')
				odd=GetEndExtraOdd(0);
			else if(m_cPosCtrlDatumLineType=='Y')
				odd=GetEndExtraOdd(1);
			else
				odd=GetEndExtraOdd(2);
			if(odd==0)
				datum_point = End();
			else
			{
				f3dPoint direct=End()-Start();
				normalize(direct);
				datum_point = End()-direct*odd;
			}
		}
	}
	else if(m_pModel)
	{	//�м�ڵ��г�����㣬��������ڵ㸸�Ǹ־�ӦΪ��ǰ�Ǹ�
		CSmartPtr<CLDSNode> pAttachNode1,pAttachNode2,pAttachNode3;
		CSuperSmartPtr<CLDSPart> pPart;
		f3dPoint start_pos,end_pos,offset_vec,perp_vec,face_norm;
		f3dPoint init_pos=pNode->Position(true);
		//ͶӰʱ�趨��Ӷ�λ��׼֫�Լ������߻�׼λ����Ч��
		//��Ҫ�����ڼ��㵱ǰ�̶�����ͷ�Ա���������ȷ�\ƫ�ƵȽڵ�
		if(m_cPosCtrlType!=0)
		{
			f3dPoint face_norm;
			if(m_cPosCtrlType==1)
				face_norm.Set(0,0,1);	//X-Yƽ��ƽ��
			else if(m_cPosCtrlType==2)
				face_norm.Set(1,0,0);	//Y-Zƽ��ƽ��
			else if(m_cPosCtrlType==3)
				face_norm.Set(0,1,0);	//X-Zƽ��ƽ��
			if(!face_norm.IsZero())
			{
				if(m_cPosCtrlDatumLineType=='X')
					init_pos=GetDatumPosWingXByFaceOffset(pNode,face_norm);
				else if(m_cPosCtrlDatumLineType=='Y')
					init_pos=GetDatumPosWingYByFaceOffset(pNode,face_norm);
			}
		}
		char ciNodePosCalType=pNode->m_cPosCalType;
		if(ciNodePosCalType==CLDSNode::COORD_OFFSET&&!IsLenOffsetNode(pNode))
			ciNodePosCalType=0;
		switch(ciNodePosCalType)
		{
		case 4:		//���Ǹֽ��潻��
			if(pNode->GetIntersZhun(&datum_point))
				SnapPerp(&datum_point,Start(),End(),datum_point,NULL);
			else
				datum_point=pNode->Position(false);
			break;
		case 5:		//�Ǹ��ϵı����ȷֵ�
			pAttachNode1=BelongModel()->FromNodeHandle(pNode->arrRelationNode[0]);
			pAttachNode2=BelongModel()->FromNodeHandle(pNode->arrRelationNode[1]);

			if(!pAttachNode1.IsNULL()&&!pAttachNode2.IsNULL())
			{
				if(TestVisitLockLoop(pAttachNode1->handle))
					SnapPerp(&start_pos,Start(),End(),pAttachNode1->Position());
				else if(IsNodeInJg(pAttachNode1)>0)	//����ֱ���ڻ�˵���
					start_pos=GetDatumPosBer(pAttachNode1,cSpecDatumWing,false);
				else
				{
					BOOL pushed=BelongModel()->PushPartStack();
					for(pPart=BelongModel()->EnumPartFirst();pPart.IsHasPtr();pPart=BelongModel()->EnumPartNext())
					{
						if(pPart->GetClassTypeId()!=CLS_LINEANGLE&&pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
							continue;
						if(pPart.LineAnglePointer()->pStart==NULL||pPart.LineAnglePointer()->pEnd==NULL)
							continue;
						if(pPart.LineAnglePointer()->pStart==pAttachNode1&&IsNodeInJg(pPart.LineAnglePointer()->pEnd)!=0)
						{	//���Ǹֹ���
							start_pos=pPart.LineAnglePointer()->GetDatumPosBer(pAttachNode1,cSpecDatumWing,false);
							break;
						}
						else if(pPart.LineAnglePointer()->pEnd==pAttachNode1&&IsNodeInJg(pPart.LineAnglePointer()->pStart)!=0)
						{	//���Ǹֹ���
							start_pos=pPart.LineAnglePointer()->GetDatumPosBer(pAttachNode1,cSpecDatumWing,false);
							break;
						}
					}
					BelongModel()->PopPartStack(pushed);
					if(pPart.IsHasPtr())	//�ҵ�����Ǹ�
						SnapPerp(&start_pos,Start(),End(),start_pos);
					else			//�����쳣
						start_pos=GetDatumPosBer(pAttachNode1,cSpecDatumWing,false);
				}
				if(TestVisitLockLoop(pAttachNode2->handle))
					SnapPerp(&end_pos,Start(),End(),pAttachNode2->Position());
				else if(IsNodeInJg(pAttachNode2)>0)	//����ֱ���ڻ�˵���
					end_pos=GetDatumPosBer(pAttachNode2,cSpecDatumWing,false);
				else
				{
					BOOL pushed=BelongModel()->PushPartStack();
					for(pPart=BelongModel()->EnumPartFirst();pPart.IsHasPtr();pPart=BelongModel()->EnumPartNext())
					{
						if(pPart->GetClassTypeId()!=CLS_LINEANGLE&&pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
							continue;
						if(pPart.LineAnglePointer()->pStart==NULL||pPart.LineAnglePointer()->pEnd==NULL)
							continue;
						if(pPart.LineAnglePointer()->pStart==pAttachNode2&&IsNodeInJg(pPart.LineAnglePointer()->pEnd)!=0)
						{	//���Ǹֹ���
							end_pos=pPart.LineAnglePointer()->GetDatumPosBer(pAttachNode2,cSpecDatumWing,false);
							break;
						}
						else if(pPart.LineAnglePointer()->pEnd==pAttachNode2&&IsNodeInJg(pPart.LineAnglePointer()->pStart)!=0)
						{	//���Ǹֹ���
							end_pos=pPart.LineAnglePointer()->GetDatumPosBer(pAttachNode2,cSpecDatumWing,false);
							break;
						}
					}
					BelongModel()->PopPartStack(pushed);
					if(pPart.IsHasPtr())	//�ҵ�����Ǹ�
						SnapPerp(&end_pos,Start(),End(),end_pos);
					else			//�����쳣
						end_pos=GetDatumPosBer(pAttachNode2,cSpecDatumWing,false);
				}
				datum_point=start_pos+pNode->attach_scale*(end_pos-start_pos);
			}
			else
				SnapPerp(&datum_point,Start(),End(),pNode->Position(true),NULL);
			break;
		case 7:		//�ؽǸ���ĳ�ڵ��ƫ�Ƶ�
			pAttachNode1=BelongModel()->FromNodeHandle(pNode->arrRelationNode[0]);
			if(pAttachNode1)
			{
				if(TestVisitLockLoop(pAttachNode1->handle))
					SnapPerp(&start_pos,Start(),End(),pAttachNode1->Position());
				else if(IsNodeInJg(pAttachNode1)>0)	//����ֱ���ڻ�˵���
					start_pos=GetDatumPosBer(pAttachNode1,cSpecDatumWing,false);
				else
				{	//����ֱ����,��ȥ���ֲ�֪��ʱ�ǳ��ںο��ǣ����ھ���������
					BOOL pushed=BelongModel()->PushPartStack();
					for(pPart=BelongModel()->EnumPartFirst();pPart.IsHasPtr();pPart=BelongModel()->EnumPartNext())
					{
						if(pPart->GetClassTypeId()!=CLS_LINEANGLE&&pPart->GetClassTypeId()!=CLS_GROUPLINEANGLE)
							continue;
						if(pPart.LineAnglePointer()->pStart==NULL||pPart.LineAnglePointer()->pEnd==NULL)
							continue;
						if(pPart.LineAnglePointer()->pStart==pAttachNode1&&IsNodeInJg(pPart.LineAnglePointer()->pEnd)!=0)
						{	//���Ǹֹ���
							start_pos=pPart.LineAnglePointer()->GetDatumPosBer(pAttachNode1,cSpecDatumWing,false);
							break;
						}
						else if(pPart.LineAnglePointer()->pEnd==pAttachNode1&&IsNodeInJg(pPart.LineAnglePointer()->pStart)!=0)
						{	//���Ǹֹ���
							start_pos=pPart.LineAnglePointer()->GetDatumPosBer(pAttachNode1,cSpecDatumWing,false);
							break;
						}
					}
					BelongModel()->PopPartStack(pushed);
					if(pPart.IsHasPtr())	//�ҵ�����Ǹ�
						SnapPerp(&start_pos,Start(),End(),start_pos);
					else			//�����쳣
						start_pos=GetDatumPosBer(pAttachNode1,cSpecDatumWing,false);
				}
				
				offset_vec=End()-Start();
				normalize(offset_vec);
				pAttachNode2=BelongModel()->FromNodeHandle(pNode->arrRelationNode[1]);
				pAttachNode3=BelongModel()->FromNodeHandle(pNode->arrRelationNode[2]);
				if(pAttachNode2.IsHasPtr()&&pAttachNode3.IsHasPtr())
				{
					f3dPoint valid_vec=pAttachNode3->Position(true)-pAttachNode2->Position(true);
					if(offset_vec*valid_vec<0)
						offset_vec*=-1.0;
				}
				double cosa=1.0;
				if(m_hArcLift>0x20)
					pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
				if(pLift!=NULL)
				{
					if(pNode->m_cPosCalViceType=='X')
						face_norm=pLift->LiftVecTrans(f3dPoint(1,0,0),pNode->Position(false));
					else if(pNode->m_cPosCalViceType=='Y')
						face_norm=pLift->LiftVecTrans(f3dPoint(0,1,0),pNode->Position(false));
					else if(pNode->m_cPosCalViceType=='Z')
						face_norm=pLift->LiftVecTrans(f3dPoint(0,0,1),pNode->Position(false));
					if(pNode->m_cPosCalViceType>='X'&&pNode->m_cPosCalViceType<='Z')
						cosa=fabs(offset_vec*face_norm);
					datum_point=start_pos+offset_vec*(pNode->attach_offset/cosa);
				}
				else
				{
					if(pNode->m_cPosCalViceType=='X')
					{
						if(fabs(offset_vec.x)<EPS)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							logerr.Log("0X%X Node invalid relates X axis offset along 0X%X angle��",pNode->handle,handle);
#else
							logerr.Log("0X%X�ڵ���0X%X�Ǹֽ�������Ч��X����ƫ�ƹ�����",pNode->handle,handle);
#endif
							datum_point=start_pos;
						}
						else
							datum_point=start_pos+offset_vec*(pNode->attach_offset/fabs(offset_vec.x));
					}
					else if(pNode->m_cPosCalViceType=='Y')
					{
						if(fabs(offset_vec.y)<EPS)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							logerr.Log("0X%X Node invalid relates Y axis offset along 0X%X angle��",pNode->handle,handle);
#else
							logerr.Log("0X%X�ڵ���0X%X�Ǹֽ�������Ч��Y����ƫ�ƹ�����",pNode->handle,handle);
#endif
							datum_point=start_pos;
						}
						else
							datum_point=start_pos+offset_vec*(pNode->attach_offset/fabs(offset_vec.y));
					}
					else if(pNode->m_cPosCalViceType=='Z')
					{
						if(fabs(offset_vec.z)<EPS)
						{
#ifdef AFX_TARG_ENU_ENGLISH
							logerr.Log("0X%X Node invalid relates Z axis offset along 0X%X angle��",pNode->handle,handle);
#else
							logerr.Log("0X%X�ڵ���0X%X�Ǹֽ�������Ч��Z����ƫ�ƹ�����",pNode->handle,handle);
#endif
							datum_point=start_pos;
						}
						else
							datum_point=start_pos+offset_vec*(pNode->attach_offset/fabs(offset_vec.z));
					}
					else
						datum_point=start_pos+offset_vec*pNode->attach_offset;
				}
			}
			else
				SnapPerp(&datum_point,Start(),End(),pNode->Position(true),NULL);
			break;
		default:
			SnapPerp(&datum_point,Start(),End(),init_pos,NULL);
			break;
		}
	}
	else
		datum_point = pNode->Position(true);
	//TODO:Ŀǰ�������ڽڵ����ƶ�����һ���𼶵��������ڵ��GetDatumPosBer,���п��ܵ���arrDatumBerLocksԤ������Ԫ�ز�����
	//�������±��÷��ֵ�����ѭ��δ�ܼ�⵽���Ժ�Ӧ��һ���Ľ���wjh-2016.4.25
	for(int i=0;i<4;i++)
	{
		if(arrDatumBerLocks[i]==pNode->handle)
		{	//�粻���㣬���������ӽڵ�������ڵ��ظ�����ͬһ�ڵ㣬����TestVisitLockLoop�жϴ��������λ������� wjh-2016.4.25
			arrDatumBerLocks[i]=0;
			break;
		}
	}
	return datum_point+CalWingOffsetVec(cSpecDatumWing,0);
}

double CLDSLineAngle::GetNodeWingXG(CLDSNode *pNode)
{
	//TODO:����ȡ��getjgzjû�����⣬����Ҫ����
	JGZJ jgzj=GetAngleZJ(size_wide);
	if(pNode==NULL)
	{
		if(m_bEnableTeG)
			return xWingXZhunJu.g;
		else
			return jgzj.g;
	}
	int offset_X_dist_style=0;
	if(pNode->hFatherPart==handle)
		offset_X_dist_style=pNode->xFatherAngleZhunJu.offset_X_dist_style;
	if(offset_X_dist_style==0)
	{
		if(m_bEnableTeG)
			return xWingXZhunJu.g;
		else
			return jgzj.g;
	}
	else if(offset_X_dist_style==1)
	{
		if(m_bEnableTeG)
			return xWingXZhunJu.g1;
		else
			return jgzj.g1;
	}
	else if(offset_X_dist_style==2)
	{
		if(m_bEnableTeG)
			return xWingXZhunJu.g2;
		else
			return jgzj.g2;
	}
	else if(offset_X_dist_style==3)
	{
		if(m_bEnableTeG)
			return xWingXZhunJu.g3;
		else
			return jgzj.g3;
	}
	else// if(offset_X_dist_style==4)
	{
		return pNode->xFatherAngleZhunJu.offset_X_dist;
	}
}

double CLDSLineAngle::GetNodeWingYG(CLDSNode *pNode)
{
	JGZJ jgzj;
	//TODO:����ȡ��getjgzjû�����⣬����Ҫ����
	if(IsUnequalAngle())
		jgzj=GetAngleZJ(size_height);
	else 
		jgzj=GetAngleZJ(size_wide);
	if(pNode==NULL)
	{
		if(m_bEnableTeG)
			return xWingYZhunJu.g;
		else
			return jgzj.g;
	}
	int offset_Y_dist_style=0;
	if(pNode->hFatherPart==handle)
		offset_Y_dist_style=pNode->xFatherAngleZhunJu.offset_Y_dist_style;
	if(offset_Y_dist_style==0)
	{
		if(m_bEnableTeG)
			return xWingYZhunJu.g;
		else
			return jgzj.g;
	}
	else if(offset_Y_dist_style==1)
	{
		if(m_bEnableTeG)
			return xWingYZhunJu.g1;
		else
			return jgzj.g1;
	}
	else if(offset_Y_dist_style==2)
	{
		if(m_bEnableTeG)
			return xWingYZhunJu.g2;
		else
			return jgzj.g2;
	}
	else if(offset_Y_dist_style==3)
	{
		if(m_bEnableTeG)
			return xWingYZhunJu.g3;
		else
			return jgzj.g3;
	}
	else// if(offset_Y_dist_style==4)
	{
		return pNode->xFatherAngleZhunJu.offset_Y_dist;
	}
}
f3dPoint CLDSLineAngle::GetDatumPosWingX(CLDSNode *pNode,double spec_g/*=0*/)//,BOOL bFatherJgPos/*=TRUE*/)
{
	if(spec_g==0)	//Ĭ��׼��
		spec_g=GetNodeWingXG(pNode);
	return GetDatumPosBer(pNode)+CalWingOffsetVec('X',spec_g);
}
f3dPoint CLDSLineAngle::GetDatumPosWingY(CLDSNode *pNode,double spec_g/*=0*/)//,BOOL bFatherJgPos/*=TRUE*/)
{
	if(spec_g==0)	//Ĭ��׼��
		spec_g=GetNodeWingYG(pNode);
	return GetDatumPosBer(pNode)+CalWingOffsetVec('Y',spec_g);
}
//0:X֫Ϊ��ǰ�˹���֫;1:Y֫Ϊ��ǰ�˹���֫;2:˫֫��Ϊ����֫���������ļ�����˫֫���ӹ���
int CLDSLineAngle::GetStartWorkWing()
{
	CSuperSmartPtr<CLDSLinePart>pDatumPart=GetStartDatumPart();
	if(pDatumPart.IsNULL()||!pDatumPart->IsAngle())
		return 2;	//�޻�׼����ʱ��Ϊ��˫֫����
	if(desStartPos.spatialOperationStyle==16||desStartPos.spatialOperationStyle==17)
		return 2;	//����ͶӰ��ƽ��ʱ��Ϊ��˫֫����
	if(pDatumPart->handle==handle)
		return 2;	//�ڵ㸸�˼�Ϊ��ǰ�Ǹֱ����޷��жϹ���ֻ֫�ܰ�˫֫���Ӵ���
	f3dPoint datum_norm;
	if(desStartPos.jgber_cal_style!=1&&desStartPos.IsInDatumJgWingX())
		datum_norm=pDatumPart.LineAnglePointer()->get_norm_x_wing();
	else if(desStartPos.jgber_cal_style!=1&&desStartPos.IsInDatumJgWingY())
		datum_norm=pDatumPart.LineAnglePointer()->get_norm_y_wing();
	else if(desStartPos.jgber_cal_style!=1&&desStartPos.spatialOperationStyle==0&&desStartPos.datumPoint.datum_pos_style==3)
	{
		CLDSLinePart* pRod1=(CLDSLinePart*)BelongModel()->FromRodHandle(desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		CLDSLinePart* pRod2=(CLDSLinePart*)BelongModel()->FromRodHandle(desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum2);
		if(pRod1==NULL||pRod2==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Position datum point of 0X%X angle's start lacks of crossing line part",handle);
#else
			logerr.Log("0X%X�Ǹֵ�ʼ�˶�λ��׼��ȱ�ٽ���˼�",handle);
#endif
			return 2;
		}
		f3dPoint lineVec1=pRod1->End()-pRod1->Start();
		f3dPoint lineVec2=pRod2->End()-pRod2->Start();
		datum_norm=lineVec1^lineVec2;
		normalize(datum_norm);
	}
	else
	{
		f3dPoint datum_vec = pDatumPart->End()-pDatumPart->Start();
		f3dPoint line_vec = End()-Start();
		datum_norm = datum_vec^line_vec;
		normalize(datum_norm);
	}
	if(datum_norm.IsZero())
		return 2;
	if(fabs(datum_norm*get_norm_x_wing())>fabs(datum_norm*get_norm_y_wing()))
		return 0;
	else
		return 1;
}
//0:X֫Ϊ��ǰ�˹���֫;1:Y֫Ϊ��ǰ�˹���֫;2:˫֫��Ϊ����֫���������ļ�����˫֫���ӹ���
int CLDSLineAngle::GetEndWorkWing()
{
	CSuperSmartPtr<CLDSLinePart>pDatumPart=GetEndDatumPart();
	if(pDatumPart.IsNULL()||!pDatumPart->IsAngle())
		return 2;	//�޻�׼����ʱ��Ϊ��˫֫����
	if(desEndPos.spatialOperationStyle==16||desEndPos.spatialOperationStyle==17)
		return 2;	//����ͶӰ��ƽ��ʱ��Ϊ��˫֫����
	if(pDatumPart->handle==handle)
		return 2;	//�ڵ㸸�˼�Ϊ��ǰ�Ǹֱ����޷��жϹ���ֻ֫�ܰ�˫֫���Ӵ���
	f3dPoint datum_norm;
	if(desEndPos.jgber_cal_style!=1&&desEndPos.IsInDatumJgWingX())
		datum_norm=pDatumPart.LineAnglePointer()->get_norm_x_wing();
	else if(desEndPos.jgber_cal_style!=1&&desEndPos.IsInDatumJgWingY())
		datum_norm=pDatumPart.LineAnglePointer()->get_norm_y_wing();
	else if(desEndPos.jgber_cal_style!=1&&desEndPos.spatialOperationStyle==0&&desEndPos.datumPoint.datum_pos_style==3)
	{
		CLDSLinePart* pRod1=(CLDSLinePart*)BelongModel()->FromRodHandle(desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1);
		CLDSLinePart* pRod2=(CLDSLinePart*)BelongModel()->FromRodHandle(desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum2);
		if(pRod1==NULL||pRod2==NULL)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Position datum point of 0X%X angle's start lacks of crossing line part",handle);
#else
			logerr.Log("0X%X�Ǹֵ�ʼ�˶�λ��׼��ȱ�ٽ���˼�",handle);
#endif
			return 2;
		}
		f3dPoint lineVec1=pRod1->End()-pRod1->Start();
		f3dPoint lineVec2=pRod2->End()-pRod2->Start();
		datum_norm=lineVec1^lineVec2;
		normalize(datum_norm);
	}
	else
	{
		f3dPoint datum_vec = pDatumPart->End()-pDatumPart->Start();
		f3dPoint line_vec = End()-Start();
		datum_norm = datum_vec^line_vec;
		normalize(datum_norm);
	}
	if(datum_norm.IsZero())
		return 2;
	if(fabs(datum_norm*get_norm_x_wing())>fabs(datum_norm*get_norm_y_wing()))
		return 0;
	else
		return 1;
}

double CLDSLineAngle::GetStartExtraOdd(int wing_x0_y1_ber2)
{
	CSmartPtr<CLDSLineAngle> pDatumJg;
	if(pStart==NULL)
		return 0;	//ʼ��ĩ����Ϣ��ȫ
	if(wing_x0_y1_ber2==0&&extraStartOdd.wing_x.style==0)
		return extraStartOdd.wing_x.odd;
	else if(wing_x0_y1_ber2==1&&extraStartOdd.wing_y.style==0)
		return extraStartOdd.wing_y.odd;
	else if(wing_x0_y1_ber2==2&&extraStartOdd.ber.style==0)
		return extraStartOdd.ber.odd;
	else if(wing_x0_y1_ber2==0&&extraStartOdd.wing_x.style==1)
	{
		//extraStartOdd.wing_x.extraStartPoint.UpdatePos(m_pModel);
		extraStartOdd.wing_x.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=Start();//extraStartOdd.wing_x.extraStartPoint.pos;
		f3dPoint end=extraStartOdd.wing_x.extraDatumPoint.Position();
		f3dPoint direct=End()-Start();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraStartOdd.wing_x.odd=odd_vec*direct;
		return extraStartOdd.wing_x.odd;
	}
	else if(wing_x0_y1_ber2==1&&extraStartOdd.wing_y.style==1)
	{
		//extraStartOdd.wing_y.extraStartPoint.UpdatePos(m_pModel);
		extraStartOdd.wing_y.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=Start();//extraStartOdd.wing_y.extraStartPoint.pos;
		f3dPoint end=extraStartOdd.wing_y.extraDatumPoint.Position();
		f3dPoint direct=End()-Start();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraStartOdd.wing_y.odd=odd_vec*direct;
		return extraStartOdd.wing_y.odd;
	}
	else if(wing_x0_y1_ber2==2&&extraStartOdd.ber.style==1)
	{
		//extraStartOdd.ber.extraStartPoint.UpdatePos(m_pModel);
		extraStartOdd.ber.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=Start();//extraStartOdd.ber.extraStartPoint.pos;
		f3dPoint end=extraStartOdd.ber.extraDatumPoint.Position();
		f3dPoint direct=End()-Start();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraStartOdd.ber.odd=odd_vec*direct;
		return extraStartOdd.ber.odd;
	}
	else if(wing_x0_y1_ber2==2)
	{
		int datum_wing=GetStartWorkWing();
		if(datum_wing==0)
			return -GetStartExtraOdd(0);
		else if(datum_wing==1)
			return -GetStartExtraOdd(1);
		else
		{	//�޷��жϵ�ǰ����֫ʱ��������ͷ����ֵ�ϴ���Ϊ��׼
			double odd_x=-GetStartExtraOdd(0);
			double odd_y=-GetStartExtraOdd(1);
			if(fabs(odd_x)>fabs(odd_y))
				return odd_x;
			else
				return odd_y;
		}
	}
	else
	{
		if(desStartPos.jgber_cal_style==1)
		{	//�Ǹ�ʼ�����ָ��������ʽ
			CLDSLinePart *pDatumLinePart=NULL;
			pDatumLinePart=GetStartDatumPart();
			if(pDatumLinePart&&(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
				pDatumJg=(CLDSLineAngle*)pDatumLinePart;
			if(pDatumJg.IsNULL()||pDatumJg.IsEqualPtr(this))
				return 0;	//�޻�׼�Ǹ�,���׼�Ǹ־��Ǳ���,���׼�˼����ǽǸ�
			
			int datum_start0_end1;
			if(pDatumJg->pStart==pStart)
				datum_start0_end1=0;	//ʼ������
			else if(pDatumJg->pEnd==pStart)
				datum_start0_end1=1;	//�ն�����
			else
				datum_start0_end1=2;	//�м�����
			int father_wing_x0_y1;
			if(wing_x0_y1_ber2==0)
			{
				CLDSLineAngle::IsInsideAngle(pDatumJg,get_norm_x_wing(),&father_wing_x0_y1);
				if(father_wing_x0_y1==0&&fabs(get_norm_x_wing()*pDatumJg->get_norm_x_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
				else if(father_wing_x0_y1==1&&fabs(get_norm_x_wing()*pDatumJg->get_norm_y_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
			}
			else if(wing_x0_y1_ber2==1)
			{
				CLDSLineAngle::IsInsideAngle(pDatumJg,get_norm_y_wing(),&father_wing_x0_y1);
				if(father_wing_x0_y1==0&&fabs(get_norm_y_wing()*pDatumJg->get_norm_x_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
				else if(father_wing_x0_y1==1&&fabs(get_norm_y_wing()*pDatumJg->get_norm_y_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
			}
			f3dLine axis_line,father_axis_line;
			f3dPoint inters,face_norm,face_pick;
			
			JGZJ jgzj,father_jgzj;
			getjgzj(jgzj,GetWidth());
			if(m_bEnableTeG)
			{
				if(wing_x0_y1_ber2==0)
					jgzj=xWingXZhunJu;
				else if(wing_x0_y1_ber2==1)
					jgzj=xWingYZhunJu;
			}
			getjgzj(father_jgzj,pDatumJg->GetWidth());
			if(pDatumJg->m_bEnableTeG)
			{
				if(father_wing_x0_y1==0)
					father_jgzj=pDatumJg->xWingXZhunJu;
				else
					father_jgzj=pDatumJg->xWingYZhunJu;
			}
			if(wing_x0_y1_ber2==0)
			{
				axis_line.startPt=Start()+GetWingVecX()*jgzj.g;
				axis_line.endPt=End()+GetWingVecX()*jgzj.g;
			}
			else if(wing_x0_y1_ber2==1)
			{
				axis_line.startPt=Start()+GetWingVecY()*jgzj.g;
				axis_line.endPt=End()+GetWingVecY()*jgzj.g;
			}
			if(father_wing_x0_y1==0)
			{
				father_axis_line.startPt=pDatumJg->Start()+pDatumJg->GetWingVecX()*father_jgzj.g;
				father_axis_line.endPt=pDatumJg->End()+pDatumJg->GetWingVecX()*father_jgzj.g;
				face_norm=pDatumJg->get_norm_x_wing();
			}
			else
			{
				father_axis_line.startPt=pDatumJg->Start()+pDatumJg->GetWingVecY()*father_jgzj.g;
				father_axis_line.endPt=pDatumJg->End()+pDatumJg->GetWingVecY()*father_jgzj.g;
				face_norm=pDatumJg->get_norm_y_wing();
			}
			normalize(face_norm);
			if(datum_start0_end1==0)
				face_pick=father_axis_line.startPt;
			else
				face_pick=father_axis_line.endPt;
			project_point(axis_line.startPt,face_pick,face_norm);
			project_point(axis_line.endPt,face_pick,face_norm);
			f3dPoint ray_vec,father_vec;
			ray_vec=axis_line.startPt-axis_line.endPt;
			father_vec=father_axis_line.startPt-father_axis_line.endPt;
			normalize(ray_vec);
			normalize(father_vec);
			if(fabs(ray_vec*father_vec)>EPS_COS2)
				return 0;	//���Ǹֻ���ƽ��
			int ret=Int3dpl(axis_line,father_axis_line,inters);
			if(ret!=1)
				return 0;
			else	//�󽻳ɹ�
			{
				double dd=DISTANCE(axis_line.startPt,inters);
				if(axis_line.PtInLine(inters)>0)	//����ֱ�߶���
					return dd;
				else
					return -dd;
			}
		}
		else if(desStartPos.spatialOperationStyle==16||desStartPos.spatialOperationStyle==17)
			return 0;	//����ͶӰ/ƽ�ƶ����̶�����ͷΪ��
		else	//����ƫ��
		{
			JGZJ jgzj;
			getjgzj(jgzj,GetWidth());
			if(m_bEnableTeG)
			{
				if(wing_x0_y1_ber2==0)
					jgzj=xWingXZhunJu;
				else if(wing_x0_y1_ber2==1)
					jgzj=xWingYZhunJu;
			}
			if(desStartPos.spatialOperationStyle==19)
			{	//�˽ڵ��ڸ��Ǹ�ʼ�ˣ��Ҹ��Ǹ�ʼ�˴���ƽ�Ʒ�ʽ
				f3dPoint face_pick,face_norm,inters;
				f3dLine axis_line;
				if(wing_x0_y1_ber2==0)
				{
					axis_line.startPt=Start()+GetWingVecX()*jgzj.g;
					axis_line.endPt=End()+GetWingVecX()*jgzj.g;
				}
				else if(wing_x0_y1_ber2==1)
				{
					axis_line.startPt=Start()+GetWingVecY()*jgzj.g;
					axis_line.endPt=End()+GetWingVecY()*jgzj.g;
				}
				
				if(Int3dlf(inters,axis_line,pStart->Position(true),desStartPos.face_offset_norm)==1)
				{
					double dd;
					dd=DISTANCE(inters,axis_line.startPt);
					if(axis_line.PtInLine(inters)>0)	//�����߶��ڻ�˵���
						return dd;
					else
						return -dd;
				}
				else
					return 0;
			}
			else if(desStartPos.IsInHuoQuLine())	//���ڻ�����
			{
				CLDSLineAngle *pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(desStartPos.huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pHuoQuJg==NULL)
					pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pHuoQuJg==NULL)
					return 0;
				f3dPoint work_norm=((pHuoQuJg->End()-pHuoQuJg->Start())^(End()-Start()));
				normalize(work_norm);
				if(wing_x0_y1_ber2==0&&fabs(work_norm*get_norm_x_wing())<EPS_COS2)
					return 0;
				else if(wing_x0_y1_ber2==1&&fabs(work_norm*get_norm_y_wing())<EPS_COS2)
					return 0;
				else
					return -DISTANCE(GetStartDatumPos(),GetStartDatumPosEx());
			}
			else
				return 0;
		}
	}
}
double CLDSLineAngle::GetEndExtraOdd(int wing_x0_y1_ber2)
{
	int start0_end1=1;
	CSmartPtr<CLDSLineAngle> pDatumJg;
	if(pEnd==NULL)
		return 0;	//ʼ��ĩ����Ϣ��ȫ
	if(wing_x0_y1_ber2==0&&extraEndOdd.wing_x.style==0)
		return extraEndOdd.wing_x.odd;
	else if(wing_x0_y1_ber2==1&&extraEndOdd.wing_y.style==0)
		return extraEndOdd.wing_y.odd;
	else if(wing_x0_y1_ber2==2&&extraEndOdd.ber.style==0)
		return extraEndOdd.ber.odd;
	else if(wing_x0_y1_ber2==0&&extraEndOdd.wing_x.style==1)
	{
		extraEndOdd.wing_x.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=End();//extraEndOdd.wing_x.extraStartPoint.pos;
		f3dPoint end=extraEndOdd.wing_x.extraDatumPoint.Position();
		f3dPoint direct=Start()-End();	//���߷���
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraEndOdd.wing_x.odd=odd_vec*direct;
		return extraEndOdd.wing_x.odd;
	}
	else if(wing_x0_y1_ber2==1&&extraEndOdd.wing_y.style==1)
	{
		//extraEndOdd.wing_y.extraStartPoint.UpdatePos(m_pModel);
		extraEndOdd.wing_y.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=End();//extraEndOdd.wing_y.extraStartPoint.pos;
		f3dPoint end=extraEndOdd.wing_y.extraDatumPoint.Position();
		f3dPoint direct=Start()-End();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraEndOdd.wing_y.odd=odd_vec*direct;
		return extraEndOdd.wing_y.odd;
	}
	else if(wing_x0_y1_ber2==2&&extraEndOdd.ber.style==1)
	{
		//extraEndOdd.ber.extraStartPoint.UpdatePos(m_pModel);
		extraEndOdd.ber.extraDatumPoint.UpdatePos(m_pModel);
		f3dPoint start=End();//extraEndOdd.ber.extraStartPoint.pos;
		f3dPoint end=extraEndOdd.ber.extraDatumPoint.Position();
		f3dPoint direct=Start()-End();
		normalize(direct);
		f3dPoint odd_vec=end-start;
		extraEndOdd.ber.odd=odd_vec*direct;
		return extraEndOdd.ber.odd;
	}
	else if(wing_x0_y1_ber2==2)
	{
		int datum_wing=GetEndWorkWing();
		if(datum_wing==0)
			return -GetEndExtraOdd(0);
		else if(datum_wing==1)
			return -GetEndExtraOdd(1);
		else
		{	//�޷��жϵ�ǰ����֫ʱ��������ͷ����ֵ�ϴ���Ϊ��׼
			double odd_x=-GetEndExtraOdd(0);
			double odd_y=-GetEndExtraOdd(1);
			if(fabs(odd_x)>fabs(odd_y))
				return odd_x;
			else
				return odd_y;
		}
	}
	else
	{
		if(desEndPos.jgber_cal_style==1)
		{	//�Ǹ��ն����ָ��������ʽ
			CLDSLinePart *pDatumLinePart=NULL;
			pDatumLinePart=GetEndDatumPart();
			if(pDatumLinePart&&(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
				pDatumJg=(CLDSLineAngle*)pDatumLinePart;
			if(pDatumJg.IsNULL()||pDatumJg.IsEqualPtr(this))
				return 0;	//�޻�׼�Ǹ�,���׼�Ǹ־��Ǳ���,���׼�˼����ǽǸ�
			
			int datum_start0_end1;
			if(pDatumJg->pStart==pEnd)
				datum_start0_end1=0;	//ʼ������
			else if(pDatumJg->pEnd==pEnd)
				datum_start0_end1=1;	//�ն�����
			else
				datum_start0_end1=2;	//�м�����
			int father_wing_x0_y1;
			if(wing_x0_y1_ber2==0)
			{
				CLDSLineAngle::IsInsideAngle(pDatumJg,get_norm_x_wing(),&father_wing_x0_y1);
				if(father_wing_x0_y1==0&&fabs(get_norm_x_wing()*pDatumJg->get_norm_x_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
				else if(father_wing_x0_y1==1&&fabs(get_norm_x_wing()*pDatumJg->get_norm_y_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
			}
			else if(wing_x0_y1_ber2==1)
			{
				CLDSLineAngle::IsInsideAngle(pDatumJg,get_norm_y_wing(),&father_wing_x0_y1);
				if(father_wing_x0_y1==0&&fabs(get_norm_y_wing()*pDatumJg->get_norm_x_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
				else if(father_wing_x0_y1==1&&fabs(get_norm_y_wing()*pDatumJg->get_norm_y_wing())<EPS_COS2)
					return 0;	//��֫��ƥ��
			}
			f3dLine axis_line,father_axis_line;
			f3dPoint inters,face_norm,face_pick;
			
			JGZJ jgzj,father_jgzj;
			getjgzj(jgzj,GetWidth());
			if(m_bEnableTeG)
			{
				if(wing_x0_y1_ber2==0)
					jgzj=xWingXZhunJu;
				else if(wing_x0_y1_ber2==1)
					jgzj=xWingYZhunJu;
			}
			getjgzj(father_jgzj,pDatumJg->GetWidth());
			if(pDatumJg->m_bEnableTeG)
			{
				if(father_wing_x0_y1==0)
					father_jgzj=pDatumJg->xWingXZhunJu;
				else
					father_jgzj=pDatumJg->xWingYZhunJu;
			}
			if(wing_x0_y1_ber2==0)
			{
				axis_line.startPt=Start()+GetWingVecX()*jgzj.g;
				axis_line.endPt=End()+GetWingVecX()*jgzj.g;
			}
			else if(wing_x0_y1_ber2==1)
			{
				axis_line.startPt=Start()+GetWingVecY()*jgzj.g;
				axis_line.endPt=End()+GetWingVecY()*jgzj.g;
			}
			if(father_wing_x0_y1==0)
			{
				father_axis_line.startPt=pDatumJg->Start()+pDatumJg->GetWingVecX()*father_jgzj.g;
				father_axis_line.endPt=pDatumJg->End()+pDatumJg->GetWingVecX()*father_jgzj.g;
				face_norm=pDatumJg->get_norm_x_wing();
			}
			else
			{
				father_axis_line.startPt=pDatumJg->Start()+pDatumJg->GetWingVecY()*father_jgzj.g;
				father_axis_line.endPt=pDatumJg->End()+pDatumJg->GetWingVecY()*father_jgzj.g;
				face_norm=pDatumJg->get_norm_y_wing();
			}
			normalize(face_norm);
			if(datum_start0_end1==0)
				face_pick=father_axis_line.startPt;
			else
				face_pick=father_axis_line.endPt;
			project_point(axis_line.startPt,face_pick,face_norm);
			project_point(axis_line.endPt,face_pick,face_norm);
			f3dPoint ray_vec,father_vec;
			ray_vec=axis_line.startPt-axis_line.endPt;
			father_vec=father_axis_line.startPt-father_axis_line.endPt;
			normalize(ray_vec);
			normalize(father_vec);
			if(fabs(ray_vec*father_vec)>EPS_COS2)
				return 0;	//���Ǹֻ���ƽ��
			int ret=Int3dpl(axis_line,father_axis_line,inters);
			if(ret!=1)
				return 0;
			else	//�󽻳ɹ�
			{
				double dd=DISTANCE(axis_line.endPt,inters);
				if(axis_line.PtInLine(inters)>0)	//����ֱ�߶���
					return dd;
				else
					return -dd;
			}
		}
		else if(desEndPos.spatialOperationStyle==16||desEndPos.spatialOperationStyle==17)
			return 0;	//����ͶӰ/ƽ�ƶ����̶�����ͷΪ��
		else	//����ƫ��
		{
			JGZJ jgzj;
			getjgzj(jgzj,GetWidth());
			if(m_bEnableTeG)
			{
				if(wing_x0_y1_ber2==0)
					jgzj=xWingXZhunJu;
				else if(wing_x0_y1_ber2==1)
					jgzj=xWingYZhunJu;
			}
			if(desEndPos.spatialOperationStyle==19)
			{	//�˽ڵ��ڸ��Ǹ��նˣ��Ҹ��Ǹ��ն˴���ƽ�Ʒ�ʽ
				f3dPoint inters;
				f3dLine axis_line;
				if(wing_x0_y1_ber2==0)
				{
					axis_line.startPt=Start()+GetWingVecX()*jgzj.g;
					axis_line.endPt=End()+GetWingVecX()*jgzj.g;
				}
				else if(wing_x0_y1_ber2==1)
				{
					axis_line.startPt=Start()+GetWingVecY()*jgzj.g;
					axis_line.endPt=End()+GetWingVecY()*jgzj.g;
				}
				
				if(Int3dlf(inters,axis_line,pEnd->Position(true),desEndPos.face_offset_norm)==1)
				{
					double dd;
					dd=DISTANCE(inters,axis_line.endPt);
					if(axis_line.PtInLine(inters)>0)	//�����߶��ڻ�˵���
						return dd;
					else
						return -dd;
				}
				else
					return 0;
			}
			else if(desEndPos.IsInHuoQuLine())	//���ڻ�����
			{
				CSmartPtr<CLDSLineAngle> pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(desEndPos.huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pHuoQuJg.IsNULL())
					pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pHuoQuJg.IsNULL())
					return 0;
				f3dPoint work_norm=((pHuoQuJg->End()-pHuoQuJg->Start())^(End()-Start()));
				normalize(work_norm);
				if(wing_x0_y1_ber2==0&&fabs(work_norm*get_norm_x_wing())<EPS_COS2)
					return 0;
				else if(wing_x0_y1_ber2==1&&fabs(work_norm*get_norm_y_wing())<EPS_COS2)
					return 0;
				else
					return -DISTANCE(GetEndDatumPos(),GetEndDatumPosEx());
			}
			else
				return 0;
		}
	}
}

f3dPoint CLDSLineAngle::GetDatumPosBerByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick/*=NULL*/,char cSpecDatumWing/*=0*/)
{
	f3dPoint datum_pos;
	f3dLine prjline(Start(),End());
	//���Ǳ���һ��֫�������Ͻ�ʱ������ƫ��Ӱ��
	f3dPoint offset=CalWingOffsetVec(cSpecDatumWing,0);
	prjline.startPt+=offset;
	prjline.endPt+=offset;
	if(face_pick)
		Int3dlf(datum_pos,prjline,*face_pick,face_norm);
	else
		Int3dlf(datum_pos,prjline,pNode->Position(true),face_norm);
	return datum_pos;
}
f3dPoint CLDSLineAngle::GetSelfDatumPosWingXByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick/*=NULL*/)
{
	f3dPoint datum_point;
	f3dLine line;
	double start_offset,end_offset;
	JGZJ jgzj;
	getjgzj(jgzj,size_wide);
	if(m_bEnableTeG)
		jgzj=xWingXZhunJu;
	//�˺���Ŀǰֻ�ڵ���ͶӰ��ƽ�����õ�,����ʱһ�������Ӧ�������Ǹֵ�ƫ����,��Ǹֶ�ͷ��ƫ�����޹�
	start_offset=end_offset=jgzj.g;
	line.startPt = Start()+GetWingVecX()*start_offset;
	line.endPt   = End()+GetWingVecX()*end_offset;
	if(face_pick)
		Int3dlf(datum_point,line,*face_pick,face_norm);
	else
		Int3dlf(datum_point,line,pNode->Position(true),face_norm);
	return datum_point;
}
f3dPoint CLDSLineAngle::GetSelfDatumPosWingYByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick/*=NULL*/)
{
	f3dPoint datum_point;
	f3dLine line;
	double start_offset,end_offset;
	JGZJ jgzj;
	getjgzj(jgzj,size_wide);
	if(m_bEnableTeG)
		jgzj=xWingYZhunJu;
	//�˺���Ŀǰֻ�ڵ���ͶӰ��ƽ�����õ�,����ʱһ�������Ӧ�������Ǹֵ�ƫ����,��Ǹֶ�ͷ��ƫ�����޹�
	start_offset=end_offset=jgzj.g;
	line.startPt = Start()+GetWingVecY()*start_offset;
	line.endPt   = End()+GetWingVecY()*end_offset;
	if(face_pick)
		Int3dlf(datum_point,line,*face_pick,face_norm);
	else
		Int3dlf(datum_point,line,pNode->Position(true),face_norm);
	return datum_point;
}
f3dPoint CLDSLineAngle::GetDatumPosWingXByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick/*=NULL*/,double spec_g/*=0*/)
{
	f3dPoint datum_point;
	f3dLine line;
	double start_offset,end_offset;
	if(spec_g==0)
		start_offset=end_offset=GetNodeWingXG(pNode);
	else
		start_offset=end_offset=spec_g;
	line.startPt = Start()+CalWingOffsetVec('X',start_offset);//GetWingVecX()*start_offset;
	line.endPt   = End()+CalWingOffsetVec('X',end_offset);//GetWingVecX()*end_offset;
	if(face_pick)
		Int3dlf(datum_point,line,*face_pick,face_norm);
	else
		Int3dlf(datum_point,line,pNode->Position(true),face_norm);
	return datum_point;
}
f3dPoint CLDSLineAngle::GetDatumPosWingYByFaceOffset(CLDSNode *pNode,f3dPoint face_norm,f3dPoint *face_pick/*=NULL*/,double spec_g/*=0*/)
{
	f3dPoint datum_point;
	f3dLine line;
	double start_offset,end_offset;
	if(spec_g==0)
		start_offset=end_offset=GetNodeWingYG(pNode);
	else
		start_offset=end_offset=spec_g;
	line.startPt = Start()+CalWingOffsetVec('Y',start_offset);//GetWingVecY()*start_offset;
	line.endPt   = End()+CalWingOffsetVec('Y',end_offset);//GetWingVecY()*end_offset;
	if(face_pick)
		Int3dlf(datum_point,line,*face_pick,face_norm);
	else
		Int3dlf(datum_point,line,pNode->Position(true),face_norm);
	return datum_point;
}
f3dPoint CLDSLineAngle::GetStartWingDatumPosByFaceOffset(char cWing,f3dPoint face_norm)
{
	f3dPoint datum_point;
	f3dLine line;
	JGZJ jgzj;
	getjgzj(jgzj,size_wide);
	//�˴���2014��5��7��(V4.1.1.3�汾)ʱ�Ĵ�,(����׼��Ӧȡ��ǰ֫����׼��)	wht 17-04-20
	if(m_bEnableTeG&&cWing=='X')
		jgzj=xWingXZhunJu;
	else if(m_bEnableTeG&&cWing=='Y')
		jgzj=xWingYZhunJu;
	double wing_offset_dist=jgzj.g;

	WING_OFFSET_STYLE wing_offset=desStartPos.wing_y_offset;
	f3dPoint wing_vec=GetWingVecX();
	if(cWing=='Y')
	{
		wing_offset=desStartPos.wing_x_offset;
		wing_vec=GetWingVecY();
	}
	if(wing_offset.gStyle==0)
		wing_offset_dist=jgzj.g;
	else if(wing_offset.gStyle==1)
		wing_offset_dist=jgzj.g1;
	else if(wing_offset.gStyle==2)
		wing_offset_dist=jgzj.g2;
	else if(wing_offset.gStyle==3)
		wing_offset_dist=jgzj.g3;
	else if(wing_offset.gStyle==4)
		wing_offset_dist=wing_offset.offsetDist;

	line.startPt = Start()+wing_vec*wing_offset_dist;
	line.endPt   = End()+wing_vec*wing_offset_dist;
	Int3dlf(datum_point,line,pStart->Position(),face_norm);
	return datum_point;
}
f3dPoint CLDSLineAngle::GetEndWingDatumPosByFaceOffset(char cWing,f3dPoint face_norm)
{
	f3dPoint datum_point;
	f3dLine line;
	JGZJ jgzj;
	getjgzj(jgzj,size_wide);
	//�˴���2014��5��7��(V4.1.1.3�汾)ʱ�Ĵ�,(����׼��Ӧȡ��ǰ֫����׼��)	wht 17-04-20
	if(m_bEnableTeG&&cWing=='X')
		jgzj=xWingXZhunJu;
	else if(m_bEnableTeG&&cWing=='Y')
		jgzj=xWingYZhunJu;
	double wing_offset_dist=jgzj.g;

	WING_OFFSET_STYLE wing_offset=desEndPos.wing_y_offset;
	f3dPoint wing_vec=GetWingVecX();
	if(cWing=='Y')
	{
		wing_offset=desEndPos.wing_x_offset;
		wing_vec=GetWingVecY();
	}
	if(wing_offset.gStyle==0)
		wing_offset_dist=jgzj.g;
	else if(wing_offset.gStyle==1)
		wing_offset_dist=jgzj.g1;
	else if(wing_offset.gStyle==2)
		wing_offset_dist=jgzj.g2;
	else if(wing_offset.gStyle==3)
		wing_offset_dist=jgzj.g3;
	else if(wing_offset.gStyle==4)
		wing_offset_dist=wing_offset.offsetDist;

	line.startPt = Start()+wing_vec*wing_offset_dist;
	line.endPt   = End()+wing_vec*wing_offset_dist;
	Int3dlf(datum_point,line,pEnd->Position(),face_norm);
	return datum_point;
}
bool CLDSLineAngle::cal_x_wing_norm(bool bIgnoreRepeatFlag/*=FALSE*/)
{
	if(BelongModel()==NULL)
		return false;
	if(IsShadowObject())
	{	//Ӱ��Ǹ�
		CLDSLineAngle *pMotherAngle=(CLDSLineAngle*)MotherObject();
		if(m_iShadowObjType==1)
		{
			pMotherAngle->cal_x_wing_norm();
			CTower *pTower=(CTower*)BelongModel();
			CParaCS *pAcs=pTower->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
			if(pAcs)
			{
				if(pAcs->UpdateACS(m_pModel)==FALSE)
					return false;
				norm_x_wing=pAcs->VectorTrans(pMotherAngle->get_norm_x_wing());
				norm_y_wing=pAcs->VectorTrans(pMotherAngle->get_norm_y_wing());
				if(pAcs->GetParamInfo().m_xMirInfo.axis_flag<0x04)	//X��Y��Գ�
				{
					f3dPoint wing_norm=norm_x_wing;
					norm_x_wing=norm_y_wing;
					norm_y_wing=norm_x_wing;
				}
				return true;
			}
		}
		else if(m_iShadowObjType==2||m_iShadowObjType==3)
		{
			CBlockParameter blockPara;
			norm_x_wing=pMotherAngle->get_norm_x_wing();
			if(GetBlockParameter(blockPara))
			{
				UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
				vector_trans(norm_x_wing,cs,TRUE);
				return true;
			}
		}
		return false;
	}

	CSuperSmartPtr<CLDSLinePart> pVicePart;
	if(bIgnoreRepeatFlag)
		_cnCalNormHits=0;
	CDepthCounter<BYTE> visitor(&_cnCalNormHits);
	if(_cnCalNormHits>2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("0X%X Angle's X and Y leg normal are parallel with each other,leg normal is miscalculation",handle);
#else
		logerr.Log("�Ǹ�0X%X֫����X\\Y֫��������,֫���߼������",handle);
#endif
		return false;
	}
	SetModified();
	f3dLine rodline(Start(),End());
	if(rodline.startPt.IsEqual(rodline.endPt)&&pStart&&pEnd)
	{
		rodline.startPt=pStart->ActivePosition();
		rodline.endPt=pEnd->ActivePosition();
	}
	if(!des_norm_x.IsSpecNormActual())	//��������Զ�����֫����
	{
		if(!des_norm_x.bByOtherWing)
		{
			CLDSPart *pPart = BelongModel()->FromPartHandle(des_norm_x.hViceJg);
			if(des_norm_x.hViceJg==handle)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Angle 0X%X--the angle X leg normal desgin based on is itself��can't calculate leg normal!",handle);
#else
				logerr.Log("�Ǹ�0X%X--X֫����������ݽǸ�Ϊ��ǰ�Ǹ������޷�����֫���߼���!",handle);
				return false;
#endif
			}
			if(pPart&&pPart->IsLinePart())
				pVicePart = (CLDSLinePart*)pPart;
			if(pVicePart.IsHasPtr())
			{
				if(des_norm_x.near_norm.IsZero())	//���Ʒ���Ϊ��ʱȡ��ǰ����ֵΪ���Ʒ���
					des_norm_x.near_norm=norm_x_wing;
				double dl1=DISTANCE(rodline.startPt,rodline.endPt);
				double dl2=DISTANCE(pVicePart->Start(),pVicePart->End());
				double dl=DISTANCE(rodline.endPt,pVicePart->End());
				double minDL12=min(dl1,dl2);
				//if(dl>EPS2&&min(dl1,dl2)/dl<0.05)	//�Ǹֻ���սǸֳ���ԶС�����Ǹּ����,Ϊ��������������Ư�ƽ��������˳�� wjh-2016.12.19
				if(dl>dl1&&minDL12<1500)	//ԭ��ֵ'min(dl1,dl2)/dl<0.05'������,ʵ�����ֻȡ����minDL12�ĳ���,Խ�������Խ�� wjh-2017.1.19
				{
					if(dl2>50)//if(minDL12==dl1)
					{	//�Ե�2��Ϊ�ױ�
						f3dPoint lenBtmStdVec=(pVicePart->End()-pVicePart->Start())/dl2;
						f3dPoint perpLenVecOfStart=rodline.startPt-pVicePart->Start();
						perpLenVecOfStart-=lenBtmStdVec*(perpLenVecOfStart*lenBtmStdVec);
						f3dPoint perpLenVecOfEnd=rodline.endPt-pVicePart->Start();
						perpLenVecOfEnd-=lenBtmStdVec*(perpLenVecOfEnd*lenBtmStdVec);
						if(perpLenVecOfStart.mod2()>perpLenVecOfEnd.mod2())
							norm_x_wing = CalFaceNorm(rodline.startPt,pVicePart->End(),pVicePart->Start(),pVicePart->End());
						else
							norm_x_wing = CalFaceNorm(rodline.endPt,pVicePart->End(),pVicePart->Start(),pVicePart->End());
					}
					else //if(minDL12==dl2)
					{	//�Ե�1��Ϊ�ױ�
						f3dPoint lenBtmStdVec=(rodline.endPt-rodline.startPt)/dl1;
						f3dPoint perpLenVecOfStart=pVicePart->Start()-rodline.startPt;
						perpLenVecOfStart-=lenBtmStdVec*(perpLenVecOfStart*lenBtmStdVec);
						f3dPoint perpLenVecOfEnd=pVicePart->End()-rodline.startPt;
						perpLenVecOfEnd-=lenBtmStdVec*(perpLenVecOfEnd*lenBtmStdVec);
						if(perpLenVecOfStart.mod2()>perpLenVecOfEnd.mod2())
							norm_x_wing = CalFaceNorm(pVicePart->Start(),rodline.endPt,rodline.startPt,rodline.endPt);
						else
							norm_x_wing = CalFaceNorm(pVicePart->End(),rodline.endPt,rodline.startPt,rodline.endPt);
					}
					//GEPOINT vLegacyNorm = CalFaceNorm(rodline.startPt,pVicePart->End(),pVicePart->Start(),rodline.endPt);
					//double cosa=fabs(norm_x_wing*vLegacyNorm);
					//if(cosa<EPS_COS)
					//	Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�Ǹ�X֫ǰ���߼���ƫ��cosa=%f",handle,1-cosa);
				}
				else
					norm_x_wing = CalFaceNorm(rodline.startPt,rodline.endPt,pVicePart->Start(),pVicePart->End());
				normalize(norm_x_wing);
			}
			else
			{
				int iWarningLevel=CLogFile::WARNING_LEVEL3_UNIMPORTANT;
				if(norm_x_wing.IsZero())
				{
					f3dPoint vec = rodline.endPt-rodline.startPt;
					if (pStart&&pEnd&&(vec*(pEnd->xOriginalPos-pStart->xOriginalPos)<0))
						vec*=-1.0;	//�Ǹֶ����λ�ó�������ƫ�� wjh-2020.2.11
					norm_x_wing=norm_y_wing^vec;//.Set();
					normalize(norm_x_wing);
					iWarningLevel=CLogFile::WARNING_LEVEL2_COMMON;
				}
				des_norm_x.hViceJg=0;
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(iWarningLevel,"Angle 0X%X--the angle X leg normal desgin based on is not found��can't calculate leg normal!",handle);
#else
				logerr.LevelLog(iWarningLevel,"�Ǹ�0X%X--X֫����������ݽǸ�Ϊδ�ҵ����޷�����֫���߼���!",handle);
#endif
			}
			double ddx=norm_x_wing*des_norm_x.near_norm;
			if(ddx<-EPS)
				norm_x_wing = -norm_x_wing.normalized();
			if(fabs(ddx)<0.2)	//���Ʒ�����ʵ��֫���߲�ƥ�䣨�ӽ���ֱ����ֱ���õ�ǰʵ�ʷ��ߴ�����Ʒ��� 18-12-24
				des_norm_x.near_norm=norm_x_wing;
		}
		else	//������Y֫����
		{
			f3dPoint vec=rodline.endPt-rodline.startPt;
			normalize(vec);
			cal_y_wing_norm();
			norm_x_wing = vec^norm_y_wing;
		}
	}
	else	//����ָ��֫���߷���
	{
		des_norm_x.spec_norm.UpdateVector(BelongModel());
		if(!des_norm_x.spec_norm.vector.IsZero())	//���Ʒ���Ϊ��ʱȡ��ǰ����ֵΪ���Ʒ���
			norm_x_wing=des_norm_x.spec_norm.vector;
		normalize(norm_x_wing);
	}
	if(norm_x_wing.IsZero())
	{	//�Ǹ�֫����Ϊ0��Ӧ�׳��쳣 wht 11-05-19
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X angle--X leg normal is 0,maybe leg normal is miscalculation!",handle);
#else
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"�Ǹ�0X%X--X֫����Ϊ0,֫���߼�����ܲ�׼!",handle);
#endif
		norm_x_wing=CalAxisYZByX(rodline.endPt-rodline.startPt,1);
		return false;
	}
	set_norm_x_wing(norm_x_wing,des_norm_x.bSpecific);
	return true;
}
bool CLDSLineAngle::SetDirectMasterAngle()	//�������ϽǸ��ӽǸ�ʱ���趨����Ϊ��ϽǸ�֫���ߵļ����׼�Ǹ� wjh-2016.9.19
{
	CLDSGroupLineAngle* pGroupAngle=
		group_father_jg_h>0x20?(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(group_father_jg_h,CLS_GROUPLINEANGLE):NULL;
	for(int i=0;pGroupAngle&&i<4;i++)
	{
		if(pGroupAngle->son_jg_h[i]==handle)
			break;
	}
	if(pGroupAngle&&i<4)
		pGroupAngle->ciMasterDirectIndex=i+1;
	return (pGroupAngle&&i<4);
}
bool CLDSLineAngle::cal_y_wing_norm(bool bIgnoreRepeatFlag/*=FALSE*/)
{
	if(BelongModel()==NULL)
		return false;
	if(IsShadowObject())
	{	//Ӱ��Ǹ�
		CLDSLineAngle *pMotherAngle=(CLDSLineAngle*)MotherObject();
		if(m_iShadowObjType==1)
		{
			pMotherAngle->cal_y_wing_norm();
			CTower *pTower=(CTower*)BelongModel();
			CParaCS *pAcs=pTower->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
			if(pAcs)
			{
				if(pAcs->UpdateACS(m_pModel)==FALSE)
					return false;
				norm_x_wing=pAcs->VectorTrans(pMotherAngle->get_norm_x_wing());
				norm_y_wing=pAcs->VectorTrans(pMotherAngle->get_norm_y_wing());
				if(pAcs->GetParamInfo().m_xMirInfo.axis_flag<0x04)	//X��Y��Գ�
				{
					f3dPoint wing_norm=norm_x_wing;
					norm_x_wing=norm_y_wing;
					norm_y_wing=norm_x_wing;
				}
				return true;
			}
		}
		else if(m_iShadowObjType==2||m_iShadowObjType==3)
		{
			CBlockParameter blockPara;
			norm_y_wing=pMotherAngle->get_norm_y_wing();
			if(GetBlockParameter(blockPara))
			{
				UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
				vector_trans(norm_y_wing,cs,TRUE);
				return true;
			}
		}
		return false;
	}
	CSuperSmartPtr<CLDSLinePart> pVicePart;
	if(bIgnoreRepeatFlag)
		_cnCalNormHits=0;
	CDepthCounter<BYTE> visitor(&_cnCalNormHits);
	if(_cnCalNormHits>2)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("0X%X Angle's X and Y leg normal are parallel with each other,leg normal is miscalculation",handle);
#else
		logerr.Log("�Ǹ�0X%X֫����X\\Y֫��������,֫���߼������",handle);
#endif
		return false;
	}
	SetModified();
	f3dLine rodline(Start(),End());
	if(rodline.startPt.IsEqual(rodline.endPt)&&pStart&&pEnd)
	{
		rodline.startPt=pStart->ActivePosition();
		rodline.endPt=pEnd->ActivePosition();
	}
	if(!des_norm_y.IsSpecNormActual())	//��������Զ�����֫����
	{
		if(pStart&&pEnd&&!des_norm_y.bByOtherWing)
		{
			CLDSPart *pPart = BelongModel()->FromPartHandle(des_norm_y.hViceJg);
			if(des_norm_y.hViceJg==handle)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Angle 0X%X--the angle Y leg normal desgin based on is itself��can't calculate leg normal!",handle);
#else
				logerr.Log("�Ǹ�0X%X--Y֫����������ݽǸ�Ϊ��ǰ�Ǹ������޷�����֫���߼���!",handle);
#endif
				return false;
			}
			if(pPart&&pPart->IsLinePart())
				pVicePart = (CLDSLinePart*)pPart;
			if(pVicePart.IsHasPtr())
			{
				if(des_norm_y.near_norm.IsZero())	//���Ʒ���Ϊ��ʱȡ��ǰ����ֵΪ���Ʒ���
					des_norm_y.near_norm=norm_y_wing;
				double dl1=DISTANCE(rodline.startPt,rodline.endPt);
				double dl2=DISTANCE(pVicePart->Start(),pVicePart->End());
				double dl=DISTANCE(rodline.endPt,pVicePart->End());
				double minDL12=min(dl1,dl2);
				//if(dl>EPS2&&min(dl1,dl2)/dl<0.05)	//�Ǹֻ���սǸֳ���ԶС�����Ǹּ����,Ϊ��������������Ư�ƽ��������˳�� wjh-2016.12.19
				if(dl>dl1&&minDL12<1500)	//ԭ��ֵ'min(dl1,dl2)/dl<0.05'������,ʵ�����ֻȡ����minDL12�ĳ���,Խ�������Խ�� wjh-2017.1.19
				{
					if(dl2>50)//if(minDL12==dl1)
					{	//�Ե�2��Ϊ�ױ�
						f3dPoint lenBtmStdVec=(pVicePart->End()-pVicePart->Start())/dl2;
						f3dPoint perpLenVecOfStart=rodline.startPt-pVicePart->Start();
						perpLenVecOfStart-=lenBtmStdVec*(perpLenVecOfStart*lenBtmStdVec);
						f3dPoint perpLenVecOfEnd=rodline.endPt-pVicePart->Start();
						perpLenVecOfEnd-=lenBtmStdVec*(perpLenVecOfEnd*lenBtmStdVec);
						if(perpLenVecOfStart.mod2()>perpLenVecOfEnd.mod2())
							norm_y_wing = CalFaceNorm(rodline.startPt,pVicePart->End(),pVicePart->Start(),pVicePart->End());
						else
							norm_y_wing = CalFaceNorm(rodline.endPt,pVicePart->End(),pVicePart->Start(),pVicePart->End());
					}
					else //if(minDL12==dl2)
					{	//�Ե�1��Ϊ�ױ�
						f3dPoint lenBtmStdVec=(rodline.endPt-rodline.startPt)/dl1;
						f3dPoint perpLenVecOfStart=pVicePart->Start()-rodline.startPt;
						perpLenVecOfStart-=lenBtmStdVec*(perpLenVecOfStart*lenBtmStdVec);
						f3dPoint perpLenVecOfEnd=pVicePart->End()-rodline.startPt;
						perpLenVecOfEnd-=lenBtmStdVec*(perpLenVecOfEnd*lenBtmStdVec);
						if(perpLenVecOfStart.mod2()>perpLenVecOfEnd.mod2())
							norm_y_wing = CalFaceNorm(pVicePart->Start(),rodline.endPt,rodline.startPt,rodline.endPt);
						else
							norm_y_wing = CalFaceNorm(pVicePart->End(),rodline.endPt,rodline.startPt,rodline.endPt);
					}
					//GEPOINT vLegacyNorm = CalFaceNorm(rodline.startPt,pVicePart->End(),pVicePart->End(),rodline.endPt);
					//double cosa=fabs(norm_y_wing*vLegacyNorm);
					//if(cosa<EPS_COS)
					//	Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X�Ǹ�X֫ǰ���߼���ƫ��cosa=%f",handle,1-cosa);
				}
				else
					norm_y_wing = CalFaceNorm(rodline.startPt,rodline.endPt,pVicePart->Start(),pVicePart->End());
				normalize(norm_y_wing);
			}
			else
			{
				int iWarningLevel=CLogFile::WARNING_LEVEL3_UNIMPORTANT;
				if(norm_y_wing.IsZero())
				{
					f3dPoint vec = rodline.endPt-rodline.startPt;
					norm_y_wing=vec^norm_x_wing;//.Set();
					normalize(norm_y_wing);
					iWarningLevel=CLogFile::WARNING_LEVEL2_COMMON;
				}
				des_norm_y.hViceJg=0;
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.LevelLog(iWarningLevel,"Angle 0X%X--the angle Y leg normal desgin based on is not found��can't calculate leg normal!",handle);
#else
				logerr.LevelLog(iWarningLevel,"�Ǹ�0X%X--Y֫����������ݽǸ�Ϊδ�ҵ����޷�����֫���߼���!",handle);
#endif
			}
			double ddy=norm_y_wing*des_norm_y.near_norm;
			if(ddy<-EPS)
				norm_y_wing = -norm_y_wing.normalized();
			if(fabs(ddy)<0.2)	//���Ʒ�����ʵ��֫���߲�ƥ�䣨�ӽ���ֱ����ֱ���õ�ǰʵ�ʷ��ߴ�����Ʒ��� 18-12-24
				des_norm_y.near_norm=norm_y_wing;
		}
		else	//������X֫����
		{
			f3dPoint vec = rodline.endPt-rodline.startPt;
			if (pStart&&pEnd&&(vec*(pEnd->xOriginalPos-pStart->xOriginalPos)<0))
				vec*=-1.0;	//�Ǹֶ����λ�ó�������ƫ�� wjh-2020.2.11
			normalize(vec);
			cal_x_wing_norm();
			norm_y_wing = norm_x_wing^vec;
		}
	}
	else	//����ָ��֫���߷���
	{
		des_norm_y.spec_norm.UpdateVector(BelongModel());
		if(!des_norm_y.spec_norm.vector.IsZero())	//���Ʒ���Ϊ��ʱȡ��ǰ����ֵΪ���Ʒ���
			norm_y_wing=des_norm_y.spec_norm.vector;
		normalize(norm_y_wing);
	}
	if(norm_y_wing.IsZero())
	{	//�Ǹ�֫����Ϊ0��Ӧ�׳��쳣 wht 11-05-19
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%Xangle--Y leg normal is 0,maybe leg normal is miscalculation!",handle);
#else
		logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"�Ǹ�0X%X--Y֫����Ϊ0,֫���߼�����ܲ�׼!",handle);
#endif
		norm_y_wing=CalAxisYZByX(rodline.endPt-rodline.startPt,0);
		return false;
	}
	set_norm_y_wing(norm_y_wing,des_norm_y.bSpecific);
	return true;
}

bool CLDSLineAngle::IsKaiJiao(double tolerance/*=0*/,double style_cosa0_clearance_1/*=0*/)
{
	double cosa=-norm_x_wing*norm_y_wing;
	if(style_cosa0_clearance_1==0)
	{
		if(cosa<style_cosa0_clearance_1)
			return true;
		else
			return false;
	}
	return false;
}
bool CLDSLineAngle::IsHeJiao(double tolerance/*=0*/,double style_cosa0_clearance_1/*=0*/)
{
	double cosa=-norm_x_wing*norm_y_wing;
	if(style_cosa0_clearance_1==0)
	{
		if(cosa>style_cosa0_clearance_1)
			return true;
		else
			return false;
	}
	return false;
}
bool CLDSLineAngle::IsReservedHeJiao()
{
	if(m_fHeJiaoReservedWidth>0&&IsHeJiao())
		return true;
	else
		return false;
}

double CLDSLineAngle::GetRadWingAngle()
{
	return Pi-cal_angle_of_2vec(norm_x_wing,norm_y_wing);
}

double CLDSLineAngle::GetDecWingAngle()
{
	return 180-cal_angle_of_2vec(norm_x_wing,norm_y_wing)*DEGTORAD_COEF;//180*/Pi;
}
double CLDSLineAngle::ConnectClearance()	//�п��Ͻ�ʱ���������ӵ����߽Ǹ�����϶
{
	double sina=fabs(norm_x_wing*norm_y_wing);
	return sina*(size.wide-size.thick);	//���������Ҫ������϶=��֫��-֫��*sina;
}
UCS_STRU CLDSLineAngle::BuildUCS()
{
	getUCS(ucs);
	return ucs;
}
void CLDSLineAngle::getUCS(UCS_STRU &ucs,int base_x0_y1_mid2/*==0*/)
{
    //--------VVVVVVVVVV----����Ǹ��������ϵ�����������᷽��  Wjh--00.05.15-------
	//�����������������Ϊ�����������������û�������֫���߷����п���
	//��������ʱ������������Z�᷽��ļ���Ϊ��ߣ����ɱ䣩�����ΪX�ᡢY��
	//Z�����Ǹֵ����췽��X��Y��ֱ������֫����չ����
    ucs.origin = Start();
	//����Ǹ�Z�᷽��
    Sub_Pnt( ucs.axis_z, End(), Start());
	if(base_x0_y1_mid2==0)			//X֫Ϊ����ϵ��׼
	{
		ucs.axis_x = ucs.axis_z^norm_x_wing;
		ucs.axis_y = ucs.axis_z^ucs.axis_x;
	}
	else if(base_x0_y1_mid2==1)		//Y֫Ϊ����ϵ��׼
	{
		ucs.axis_y = norm_y_wing^ucs.axis_z;
		ucs.axis_x = ucs.axis_y^ucs.axis_z;
	}
	else// if(base_x0_y1_mid2==2)	//��֫�н�ƽ����Ϊ��׼
	{
		f3dPoint bisec,vec;
		bisec=-get_norm_x_wing()-get_norm_y_wing();
		vec = get_norm_x_wing()-get_norm_y_wing();
		normalize(vec);
		ucs.axis_x = bisec+bisec.mod()*vec;
		ucs.axis_y = bisec-bisec.mod()*vec;
	}

	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	normalize(ucs.axis_z);
}
BOOL CLDSLineAngle::GetStartEndBoltD(int *start_d,int *end_d)
{
	CLsRef *pLsRef=GetFirstLsRef();
	if(pLsRef==NULL)
		return FALSE;
	double start_ls_len,end_ls_len;
	int start_ls_d=pLsRef->GetLsPtr()->get_d();
	int end_ls_d=pLsRef->GetLsPtr()->get_d();
	f3dPoint stdlen_vec=End()-Start();
	normalize(stdlen_vec);
	f3dPoint len_vec=pLsRef->GetLsPtr()->ucs.origin-Start();
	start_ls_len=end_ls_len=len_vec*stdlen_vec;
	for(pLsRef=GetNextLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		len_vec=pLsRef->GetLsPtr()->ucs.origin-Start();
		double len=len_vec*stdlen_vec;
		if(len<start_ls_len)
		{
			start_ls_d=pLsRef->GetLsPtr()->get_d();
			start_ls_len=len;
		}
		else if(len>end_ls_len)
		{
			end_ls_d=pLsRef->GetLsPtr()->get_d();
			end_ls_len=len;
		}
	}
	if(start_d!=NULL)
		*start_d=start_ls_d;
	if(end_d!=NULL)
		*end_d=end_ls_d;
	return TRUE;
}
bool CLDSLineAngle::IsStronglyNoneKaiHeAngle()	//�Ƿ�Ϊǿ�Ʋ����Ͻǵ���������������ģ����������
{
	if(kaihe_base_wing_x0_y1==3)
		return true;
	else if(kaihe_base_wing_x0_y1!=0&&kaihe_base_wing_x0_y1!=1)
	{	//δ��ȷָ�����Ͻ�֫
		if(((Layer(0)=='S'||Layer(0)=='L')&&Layer(1)=='Z'))
			return true;	//�Ȳ���������һ�㲻���Ͻ�
		if( (pStart==NULL&&desStartPos.spatialOperationStyle==0&&desStartPos.datumPoint.datum_pos_style==7)||
			(pEnd==NULL&&desEndPos.spatialOperationStyle==0&&desEndPos.datumPoint.datum_pos_style==7))
			return true;	//�������͵Ķ̽Ǹ�һ��Ҳ����Ҫ���Ͻ�
	}
	return false;
}
int CLDSLineAngle::GetKaiHeWingX0Y1(long hPreventSideRod/*=0*/)
{
	if(IsStronglyNoneKaiHeAngle())
		return -1;	//ǿ��鲻���Ͻǵ���������������ģ����������
	if(kaihe_base_wing_x0_y1==0)
		return 0;	//X֫���п��Ͻ�
	else if(kaihe_base_wing_x0_y1==1)
		return 1;	//Y֫���п��Ͻ�
	else if(kaihe_base_wing_x0_y1==3)
		return -1;
	int work_wing=GetStartWorkWing();
	if(work_wing>=2)
		work_wing=GetEndWorkWing();
	if(work_wing>=2&&pStart!=NULL&&pEnd!=NULL&&m_pModel!=NULL)
	{	//����Ƿ�Ϊ���߸˼�����Ϊ���߸˼����迼�����ӵĹ��߸˼����Ƿ�Ϊ��ӽǸ�
		GEPOINT lenJointVec,lenStdVec = xPosEnd - xPosStart;
		normalize(lenStdVec);
		long hColinearJointRod = desStartPos.datum_jg_h;
		if(pStart!=NULL&&desStartPos.datum_jg_h==0)
			hColinearJointRod=pStart->hFatherPart;
		if (hColinearJointRod == handle&&(desStartPos.spatialOperationStyle==1||
			desStartPos.spatialOperationStyle==16||desStartPos.spatialOperationStyle==17))
		{
			hColinearJointRod=0;
			CLdsPartListStack stack(m_pModel);
			for (CLDSLinePart* pRod = m_pModel->EnumRodFirst(); pRod; pRod = m_pModel->EnumRodNext())
			{
				if (pRod==this||!ModuleTogetherWith(pRod))
					continue;
				if (pRod->pStart == pStart)
					lenJointVec = pRod->xPosStart - pRod->xPosEnd;
				else if (pRod->pEnd == pStart)
					lenJointVec = pRod->xPosEnd - pRod->xPosStart;
				else
					continue;
				normalize(lenJointVec);
				if ((lenJointVec*lenStdVec) > EPS_COS2)
				{
					hColinearJointRod = pRod->handle;
					break;
				}
			}
		}
		if (hColinearJointRod!=handle && hColinearJointRod!=hPreventSideRod)
		{	//ʼ�����Ӹ˼�������
			CLDSLineAngle* pParentAngle = (CLDSLineAngle*)m_pModel->FromPartHandle(hColinearJointRod,CLS_LINEANGLE);
			if(pParentAngle)
			{
				GEPOINT lenJointVec = pParentAngle->xPosEnd - pParentAngle->xPosStart;
				normalize(lenJointVec);
				if (fabs(lenStdVec*lenJointVec) > EPS_COS2)
					work_wing = pParentAngle->GetKaiHeWingX0Y1(handle);	//�Խӹ���
			}
		}
		hColinearJointRod = desEndPos.datum_jg_h;
		if(pEnd!=NULL&&desEndPos.datum_jg_h==0)
			hColinearJointRod=pEnd->hFatherPart;
		if (hColinearJointRod == handle&& (desEndPos.spatialOperationStyle == 1 ||
			desEndPos.spatialOperationStyle == 16 || desEndPos.spatialOperationStyle == 17))
		{
			hColinearJointRod=0;
			CLdsPartListStack stack(m_pModel);
			for (CLDSLinePart* pRod = m_pModel->EnumRodFirst(); pRod; pRod = m_pModel->EnumRodNext())
			{
				if (pRod==this||!ModuleTogetherWith(pRod))
					continue;
				if (pRod->pStart == pEnd)
					lenJointVec = pRod->xPosStart - pRod->xPosEnd;
				else if (pRod->pEnd == pEnd)
					lenJointVec = pRod->xPosEnd - pRod->xPosStart;
				else
					continue;
				normalize(lenJointVec);
				if ((lenJointVec*lenStdVec) <-EPS_COS2)
				{
					hColinearJointRod = pRod->handle;
					break;
				}
			}
		}
		if (hColinearJointRod!=handle && hColinearJointRod!=hPreventSideRod&&work_wing>=2)
		{	//�ն����Ӹ˼�������
			CLDSLineAngle* pParentAngle = (CLDSLineAngle*)m_pModel->FromPartHandle(hColinearJointRod,CLS_LINEANGLE);
			if(pParentAngle)
			{
				GEPOINT lenJointVec = pParentAngle->xPosEnd - pParentAngle->xPosStart;
				normalize(lenJointVec);
				if (fabs(lenStdVec*lenJointVec) > EPS_COS2)
					work_wing = pParentAngle->GetKaiHeWingX0Y1(handle);	//�Խӹ���
			}
		}
		if(work_wing<2)	//��ҵϰ��һ���ԷǶ�λ����֫(������֫)Ϊ����֫
			work_wing=1-work_wing;
	}
	int kaihe_wing=-1;
	if(work_wing<2)	//��ҵϰ��һ���ԷǶ�λ����֫(������֫)Ϊ����֫
		kaihe_wing=1-work_wing;
	//else //if(kaihe_base_wing_x0_y1==2)
	{
		int wing_x_ls_n=0,wing_y_ls_n=0;
		f3dPoint normX=get_norm_x_wing();
		f3dPoint normY=get_norm_y_wing();
		BOOL pushed=pLsRefList->push_stack();
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(fabs((*pLsRef)->get_norm()*normX)>fabs((*pLsRef)->get_norm()*normY))
				wing_x_ls_n++;
			else
				wing_y_ls_n++;
		}
		pLsRefList->pop_stack(pushed);
		if(kaihe_wing==0||kaihe_wing==1)
		{
			if((kaihe_wing==0&&wing_x_ls_n==0)||(kaihe_wing==1&&wing_y_ls_n==0))
				return -1;	//����֫����˨,����Ҫ���Ͻ� wht 17-08-30
			else
				return kaihe_wing;
		}
		else if(wing_x_ls_n<=wing_y_ls_n)
		{
			if(wing_x_ls_n==0)
				return -1;	//����Ҫ���п��ϽǴ���
			else
				return 0;	//Y֫��˨�϶��ѡX֫Ϊ���Ͻǹ��մ���֫
		}
		else
		{
			if(wing_y_ls_n==0)
				return -1;	//����Ҫ���п��ϽǴ���
			else
				return 1;	//X֫��˨�϶��ѡY֫Ϊ���Ͻǹ��մ���֫
		}
	}
}
//��ȡ��λ��ʽΪ���˼������淨�ߵ�֫
int CLDSLineAngle::GetWorkWingX0Y1()
{
	if(!des_norm_x.IsByViceRodActual()&&des_norm_y.IsByViceRodActual())
		return 1;	//Y֫Ϊ�˼������淨�ߣ�X֫���Ǹ˼������淨��
	else if(!des_norm_y.IsByViceRodActual()&&des_norm_x.IsByViceRodActual())
		return 0;	//X֫Ϊ�˼������淨�ߣ�Y֫���Ǹ˼������淨��
	else
		return -1;
}
#include "FuCaiCheck.h"
bool CLDSLineAngle::IsEqual(CLDSLineAngle *pJg,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	char sZhengError[200]="";
	UCS_STRU ucs,tem_ucs,vice_ucs,tem_vice_ucs;
	if(Layer(0)=='S'&&Layer(1)=='F'&&pJg->Layer(0)=='S'&&pJg->Layer(1)=='F'
		&&pStart&&pEnd&&pJg->pStart&&pJg->pEnd)	//���Ĳ���Ϊ�̽Ǹ�
	{
		CTransFuCai fucai;
		int rslt=fucai.IsEqual(&CTransFuCai(pJg),epsilon,error_info,pErrorInfo,pChkStyleFlag,true);
		if(rslt==0)	//ĿǰCTransFuCai���޷������򸨲ķ���ƫ�������µ��ľ�ƫ��
			return false;
		else if(rslt==1)
			return true;
		//else �����ϸ��ıȽϹ��� wjh-2016.10.20
	}
	/*if(sub_type!=pJg->sub_type)
	{		//�Ǹ����Ͳ�ͬ(��������һ�����������⴦��,�����)
		if(error_info)
			strcpy(error_info,"�Ǹ����Ͳ�ͬ");
		return false;
	}*/
	//try{
		getUCS(ucs,2);
		pJg->getUCS(vice_ucs,2);
		tem_ucs = ucs;
		tem_vice_ucs = vice_ucs;
		Sub_Pnt(tem_ucs.origin,ucs.origin,ucs.axis_z*start_oddment);
		Sub_Pnt(tem_vice_ucs.origin,vice_ucs.origin,vice_ucs.axis_z*pJg->start_oddment);
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_RODINFO)
		{	//��鹹��������Ϣ
			bool bRet=true;
			if(fabs(GetLength()-pJg->GetLength())>epsilon)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} length is different,lack %.1f",(char*)sPartNo,fabs(GetLength()-pJg->GetLength()));
#else
					sprintf(error_info,"{%s}���Ȳ�ͬ,��%.1f",(char*)sPartNo,fabs(GetLength()-pJg->GetLength()));
#endif
				bRet=false;
			}
			else if(GetWidth()!=pJg->GetWidth()!=0)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} wide is different",(char*)sPartNo);
#else
					sprintf(error_info,"{%s}֫��ͬ",(char*)sPartNo);
#endif
				bRet=false;
			}
			else if(GetThick()!=pJg->GetThick()!=0)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} thickness is different",(char*)sPartNo);
#else
					sprintf(error_info,"{%s}֫��ͬ",(char*)sPartNo);
#endif
				bRet=false;
			}
			else if(cMaterial!=pJg->cMaterial)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} material is different",(char*)sPartNo);
#else
					sprintf(error_info,"{%s}���ʲ�ͬ",(char*)sPartNo);
#endif
				bRet=false;
			}
			else if(fabs(GetDecWingAngle()-pJg->GetDecWingAngle())>epsilon)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} wing angle is different",(char*)sPartNo);
#else
					sprintf(error_info,"{%s}���Ͻǲ�ͬ",(char*)sPartNo);
#endif
				bRet=false;
			}
			else if(GetHeJiaoReservedWidth()!=pJg->GetHeJiaoReservedWidth()!=0)
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s}reserve width is different",(char*)sPartNo);
#else
					sprintf(error_info,"{%s}�ϽǱ������Ȳ�ͬ",(char*)sPartNo);
#endif
				bRet=false;
			}
			if(!bRet)
			{
				if(pErrorInfo!=NULL)
					*pErrorInfo=CHKPARTNO_RODINFO;
				return false;
			}
		}
		//����Ӧ
		int i,j;
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_ANGLECUT)
		{	//���ѹ���н���Ϣ�Ƿ���ͬ
			if( start_push_flat_x1_y2!=pJg->start_push_flat_x1_y2||
				start_push_flat_x1_y2>0&&fabs(start_push_flat_length-pJg->start_push_flat_length)>EPS)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sZhengError,"{%s} start push flat data is different",(char*)sPartNo); 
#else
				sprintf(sZhengError,"{%s}ʼ��ѹ�����ݲ�ͬ",(char*)sPartNo); 
#endif
				goto REVERSE_ORDER;
			}
			if( end_push_flat_x1_y2!=pJg->end_push_flat_x1_y2||
				end_push_flat_x1_y2>0&&fabs(end_push_flat_length-pJg->end_push_flat_length)>EPS)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sZhengError,"{%s} end push flat data is different",(char*)sPartNo); 
#else
				sprintf(sZhengError,"{%s}�ն�ѹ�����ݲ�ͬ",(char*)sPartNo); 
#endif
				goto REVERSE_ORDER;
			}
			for(i=0;i<2;i++)
			{
				if(cut_wing[i]!=pJg->cut_wing[i])
				{	//��¼������Ϣ������׼ȷ��� wht 11-05-26
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sZhengError,"{%s} cut leg data is different",(char*)sPartNo); 
#else
					sprintf(sZhengError,"{%s}��֫���ݲ�ͬ",(char*)sPartNo); 
#endif
					goto REVERSE_ORDER;
				}
				if(cut_wing[i]>0)
				{
					for(j=0;j<3;j++)
					{
						if(fabs(cut_wing_para[i][j]-pJg->cut_wing_para[i][j])>epsilon)
						{	//��¼������Ϣ������׼ȷ��� wht 11-05-26
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(sZhengError,"{%s} cut leg data is different",(char*)sPartNo);
#else
							sprintf(sZhengError,"{%s}��֫���ݲ�ͬ",(char*)sPartNo);
#endif
							goto REVERSE_ORDER;
						}
					}
				}
				for(j=0;j<4;j++)
				{
					if(fabs(cut_angle[j][i]-pJg->cut_angle[j][i])>epsilon)
					{	//��¼������Ϣ������׼ȷ��� wht 11-05-26
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(sZhengError,"{%s} cut leg data is different",(char*)sPartNo);
#else
						sprintf(sZhengError,"{%s}�н����ݲ�ͬ",(char*)sPartNo);
#endif
						goto REVERSE_ORDER;
					}
				}
			}
		}
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_RODBOLTINFO)
		{	//�����˨��Ϣ�Ƿ���ͬ
			if(pLsRefList->GetNodeNum()!=pJg->pLsRefList->GetNodeNum())
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} bolt number is different",(char*)sPartNo);
#else
					sprintf(error_info,"{%s}��˨������ͬ",(char*)sPartNo);
#endif
				if(pErrorInfo!=NULL)
					*pErrorInfo=CHKPARTNO_RODBOLTINFO;
				return false;
			}
			if(!IsLsRefListEqual(pLsRefList,tem_ucs,
				pJg->pLsRefList,tem_vice_ucs,epsilon,sZhengError,NULL))
				goto REVERSE_ORDER;
		}
		return true;
		//����Ӧ
	REVERSE_ORDER:
		ucs.origin = End();
		f3dPoint vec=ucs.axis_x;
		ucs.axis_x = ucs.axis_y;
		ucs.axis_y = vec;
		ucs.axis_z = -1.0*ucs.axis_z;
		tem_ucs = ucs;
		Sub_Pnt(tem_ucs.origin,ucs.origin,ucs.axis_z*end_oddment);
		//����ӦʱӦ�ȱȽ���˨��Ϣ�Ƿ�һ�£���������Ϊ������˨��Ϣ��һ�²������˴���
		//���������˨��Ϣһ�²ſ����Ƿ���Ӧ���ټ����Ƚ��н���֫��Ϣ�Ƿ�һ�� wht 11-01-18
		char sFanError[200]="";
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_RODBOLTINFO)
		{	//���ȼ����˨��Ϣ�Ƿ���ͬ
			if(!IsLsRefListEqual( pLsRefList,tem_ucs,
				pJg->pLsRefList,tem_vice_ucs,epsilon,sFanError,NULL))
			{
				if(error_info)
				{
					if(strlen(sZhengError)>0&&strcmp(sZhengError,sFanError)==0) //��������Ϣһ��
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(error_info,"{%s} proportional&inversely proportional:%s",(char*)sPartNo,sFanError);
					else if(strlen(sZhengError)>0)	//�������ȴ�����Ϣ����������Ϣ��һ��
						sprintf(error_info,"{%s} proportional:%s,inversely proportional:%s",(char*)sPartNo,sZhengError,sFanError);
					else 
						sprintf(error_info,"{%s} inversely proportional:%s",(char*)sPartNo,sFanError);
#else
						sprintf(error_info,"{%s}������:%s",(char*)sPartNo,sFanError);
					else if(strlen(sZhengError)>0)	//�������ȴ�����Ϣ����������Ϣ��һ��
						sprintf(error_info,"{%s}����:%s,����:%s",(char*)sPartNo,sZhengError,sFanError);
					else 
						sprintf(error_info,"{%s}����:%s",(char*)sPartNo,sFanError);
#endif
				}
				if(pErrorInfo)
					*pErrorInfo=CHKPARTNO_RODBOLTINFO;
				return false;
			}
		}
		if(pChkStyleFlag==NULL||*pChkStyleFlag&CHKPARTNO_ANGLECUT)
		{	//����н���Ϣ�Ƿ���ͬ
			for(i=0;i<2;i++)
			{
				bool bRetCode=true;
				if(cut_wing[i]+pJg->cut_wing[1-i]!=0 && cut_wing[i]+pJg->cut_wing[1-i]!=3)
				{	//������һ����ͷ����֫����X/Y֫����� wjh-2014.4.8
					if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
						sprintf(error_info,"{%s} cut leg data is different",(char*)sPartNo);
#else
						sprintf(error_info,"{%s}��֫���ݲ�ͬ",(char*)sPartNo);
#endif
					bRetCode=false;	//��֫״����ͬ
				}
				else if(cut_wing[i]+pJg->cut_wing[1-i]==3)
				{	//��ͷ������֫����X/Y֫��� wjh-2014.4.8
					for(j=0;j<3;j++)
					{
						if(fabs(cut_wing_para[i][j]-pJg->cut_wing_para[1-i][j])>2)
						{
							if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
								sprintf(error_info,"{%s} cut leg data is different",(char*)sPartNo);
#else
								sprintf(error_info,"{%s}��֫���ݲ�ͬ",(char*)sPartNo);
#endif
							bRetCode=false;
						}
					}
				}
				//else if(cut_wing[i]+pJg->cut_wing[1-i]==0)	//�����Ǹֶ�����֫�����迼����֫��Ϣ�ȶ�
				for(j=0;j<4;j++)
				{
					if(fabs(cut_angle[j][i]-pJg->cut_angle[3-j][i])>2)
					{
						if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
							sprintf(error_info,"{%s} cut leg data is different",(char*)sPartNo);
#else
							sprintf(error_info,"{%s}�н����ݲ�ͬ",(char*)sPartNo);
#endif
						bRetCode=false;
					}
				}
				if(!bRetCode)
				{
					if(pErrorInfo)
						*pErrorInfo=CHKPARTNO_ANGLECUT;
					return false;
				}
			}
			if( (start_push_flat_x1_y2>0||pJg->end_push_flat_x1_y2>0)&&
				(start_push_flat_x1_y2==pJg->end_push_flat_x1_y2||fabs(start_push_flat_length-pJg->end_push_flat_length)>EPS))
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} push flat data is different",(char*)sPartNo);
#else
					sprintf(error_info,"{%s}ѹ�����ݲ�ͬ",(char*)sPartNo);
#endif
				if(pErrorInfo!=NULL)
					*pErrorInfo=CHKPARTNO_ANGLECUT;
				return false;
			}
			if( (end_push_flat_x1_y2>0||pJg->start_push_flat_x1_y2>0)&&
				(end_push_flat_x1_y2==pJg->start_push_flat_x1_y2||fabs(end_push_flat_length-pJg->start_push_flat_length)>EPS))
			{
				if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(error_info,"{%s} push flat data is different",(char*)sPartNo);
#else
					sprintf(error_info,"{%s}ѹ�����ݲ�ͬ",(char*)sPartNo);
#endif
				if(pErrorInfo!=NULL)
					*pErrorInfo=CHKPARTNO_ANGLECUT;
				return false;
			}
		}
		return true;
	/*}
	catch(CPtrException &e)
	{
		AfxMessageBox(e.sError);
		return false;
	}*/
}
static double CompareBoltHoleList(ARRAY_LIST<BOLTHOLE_INFO> &datumBoltList,ARRAY_LIST<BOLTHOLE_INFO> &peerBoltList,
								  CLDSPart::PEER_PART *pPeerInfo,double fAngleLen=0)
{
	const double MAX_DOUBLE=1.7976931348623158e+308;	//��󸡵���
	if(datumBoltList.GetSize()!=peerBoltList.GetSize())
		return MAX_DOUBLE;	//������󸡵���
	double fError=0;
	double HOLE_POS_CMP_EPS=1;
	ARRAY_LIST<BOLTHOLE_INFO> reversePeerBoltList;
	ARRAY_LIST<BOLTHOLE_INFO> *pPeerBoltList=&peerBoltList;
	if(fAngleLen>0)
	{
		for(BOLTHOLE_INFO *pPeerBoltInfo=peerBoltList.GetFirst();pPeerBoltInfo;pPeerBoltInfo=peerBoltList.GetNext())
		{
			BOLTHOLE_INFO *pNewBoltInfo=reversePeerBoltList.append(*pPeerBoltInfo);
			pNewBoltInfo->centre.x=fAngleLen-pPeerBoltInfo->centre.x;
			pNewBoltInfo->centre.y=-pPeerBoltInfo->centre.y;
		}
		pPeerBoltList=&reversePeerBoltList;
	}
	for(BOLTHOLE_INFO *pBoltInfo=datumBoltList.GetFirst();pBoltInfo;pBoltInfo=datumBoltList.GetNext())
	{
		BOLTHOLE_INFO *pPeerBoltInfo=NULL;
		for(pPeerBoltInfo=pPeerBoltList->GetFirst();pPeerBoltInfo;pPeerBoltInfo=pPeerBoltList->GetNext())
		{
			if(pBoltInfo->centre.IsEqual(pPeerBoltInfo->centre,HOLE_POS_CMP_EPS))
				break;
		}
		double differ=MAX_DOUBLE;
		if(pPeerBoltInfo)
			differ=DISTANCE(pBoltInfo->centre,pPeerBoltInfo->centre);
		fError+=differ;
		if(pPeerInfo)
		{
			CLDSPart::PEER_BOLT *pPeerBolt=pPeerInfo->hashBoltsByBase.Add(pBoltInfo->BoltHandle());	
			if(pPeerBoltInfo==NULL)
				pPeerBolt->hPeerBolt=0;
			else
			{
				pPeerBolt->hPeerBolt=pPeerBoltInfo->BoltHandle();
				pPeerBolt->difference=DISTANCE(pBoltInfo->centre,pPeerBoltInfo->centre);
				//
				CLDSPart::PEER_BOLT *pPeerBolt2=pPeerInfo->hashBoltsByPeer.Add(pPeerBoltInfo->BoltHandle());
				pPeerBolt2->hPeerBolt=pBoltInfo->BoltHandle();
				pPeerBolt2->difference=pPeerBolt->difference;
			}
		}
	}
	if(pPeerInfo)
	{
		for(BOLTHOLE_INFO *pPeerBoltInfo=pPeerBoltList->GetFirst();pPeerBoltInfo;pPeerBoltInfo=pPeerBoltList->GetNext())
		{
			if(pPeerInfo->hashBoltsByPeer.GetValue(pPeerBoltInfo->BoltHandle()))
				continue;
			CLDSPart::PEER_BOLT *pPeerBolt2=pPeerInfo->hashBoltsByPeer.Add(pPeerBoltInfo->BoltHandle());
			pPeerBolt2->hPeerBolt=0;
		}
	}
	if(fError>0&&datumBoltList.GetSize()>0)
		fError/=datumBoltList.GetSize();		//��������������˨����
	return fError;
}
//
bool CLDSLineAngle::CompareLabelPart(CLDSPart *pPeerPart,MIRMSG* pMirmsg/*=NULL*/,BOOL bCmpls/*=TRUE*/,
										PEER_PART *pPeerInfo/*=NULL*/,double epsilon/*=1.0*/)
{
	if(pPeerPart->GetClassTypeId()!=CLS_LINEANGLE)
		return false;
	if(pPeerPart->m_hBlock!=m_hBlock)
		return false;	//����������һ��,����false
	CLDSLineAngle* pPeerAngle=(CLDSLineAngle*)pPeerPart;
	if(pPeerInfo)
	{
		pPeerInfo->hPeerPart=pPeerAngle->handle;
		pPeerInfo->m_hBlock=m_hBlock;
		pPeerInfo->csBase=GetUnifiedACS();
		pPeerInfo->csPeer=pPeerPart->GetUnifiedACS();
	}
	if(handle==pPeerAngle->handle)
		return true;
	//1���ȽϹ���������Ϣ(���ʡ����ȡ���ȡ���ȡ���˨��)
	if(cMaterial!=pPeerAngle->cMaterial)
		return false;
	if(fabs(GetLength()-pPeerAngle->GetLength())>epsilon)	//�Ǹֳ���
		return false;
	if(GetWidth()!=pPeerAngle->GetWidth())
		return false;
	if(GetThick()!=pPeerAngle->GetThick())
		return false;
	if(pLsRefList->GetNodeNum()!=pPeerAngle->pLsRefList->GetNodeNum())
		return false;
	//2���ȽϽǸ���˨����Ϣ
	UCS_STRU datumUcs,temUcs1,temUcs2;
	datumUcs=GetUnifiedACS();
	temUcs1=pPeerAngle->GetUnifiedACS();
	temUcs2.origin=temUcs1.origin+temUcs1.axis_z*pPeerAngle->GetLength();
	//�Ǹ�����ϵĬ����X֫Ϊ��׼�����巴������ϵʱӦ���ǿ��Ͻ����� wht 15-12-30
	temUcs2.axis_x=pPeerAngle->GetWingVecY();
	temUcs2.axis_z=-1.0*temUcs1.axis_z;
	temUcs2.axis_y=temUcs2.axis_z^temUcs2.axis_x;
	normalize(temUcs2.axis_y);
	bool bRet=false;
	//[0]:��׼�Ǹ���˨ [1]:��ǰ�Ǹ���˨
	f3dCircle *pCircle=NULL;
	ARRAY_LIST<BOLTHOLE_INFO> boltHoleList[2];	
	for(int i=0;i<2;i++)
	{
		UCS_STRU ucsArr[2]={datumUcs,temUcs1};
		CLDSLineAngle *pAngle=(i==0)?this:pPeerAngle;
		for(CLsRef *pLsRef=pAngle->GetFirstLsRef();pLsRef;pLsRef=pAngle->GetNextLsRef())
		{
			f3dPoint bolt_pos;
			pAngle->GetBoltLocalPos(pLsRef->GetLsPtr(),bolt_pos,true);
			boltHoleList[i].append(BOLTHOLE_INFO(bolt_pos,pLsRef));
		}
	}
	PEER_PART peerPart[2];
	if(pPeerInfo)
	{
		peerPart[0].CopyFrom(*pPeerInfo);
		peerPart[1].CopyFrom(*pPeerInfo);
		peerPart[1].csPeer=temUcs2;
	}
	double differ1=CompareBoltHoleList(boltHoleList[0],boltHoleList[1],&peerPart[0]);
	double differ2=CompareBoltHoleList(boltHoleList[0],boltHoleList[1],&peerPart[1],GetLength());
	if(differ1>epsilon&&differ2>epsilon)
		return false;
	else
	{
		BOOL bReverse=differ2<differ1;
		if(pPeerInfo)
			pPeerInfo->CopyFrom(bReverse?peerPart[1]:peerPart[0]);
	}
	//3���ȽϽǸֹ�����Ϣ(����+����)
	if(start_push_flat_x1_y2!=pPeerAngle->start_push_flat_x1_y2||
		(start_push_flat_x1_y2>0&&fabs(start_push_flat_length-pPeerAngle->start_push_flat_length)>EPS))
		goto REVERSE_ORDER;
	if( end_push_flat_x1_y2!=pPeerAngle->end_push_flat_x1_y2||
		(end_push_flat_x1_y2>0&&fabs(end_push_flat_length-pPeerAngle->end_push_flat_length)>EPS))
		goto REVERSE_ORDER;
	for(int i=0;i<2;i++)
	{
		if(cut_wing[i]!=pPeerAngle->cut_wing[i])
			goto REVERSE_ORDER;
		if(cut_wing[i]>0)
		{
			for(int j=0;j<3;j++)
			{
				if(fabs(cut_wing_para[i][j]-pPeerAngle->cut_wing_para[i][j])>epsilon)
					goto REVERSE_ORDER;
			}
		}
		for(int j=0;j<4;j++)
		{
			if(fabs(cut_angle[j][i]-pPeerAngle->cut_angle[j][i])>epsilon)
				goto REVERSE_ORDER;
		}
	}
	return true;
REVERSE_ORDER:
	if(start_push_flat_x1_y2!=pPeerAngle->end_push_flat_x1_y2>0||
		(start_push_flat_x1_y2>0&&fabs(start_push_flat_length-pPeerAngle->end_push_flat_length)>EPS))
		return false;
	if(end_push_flat_x1_y2!=pPeerAngle->start_push_flat_x1_y2>0||
		(end_push_flat_x1_y2>0&&fabs(end_push_flat_length-pPeerAngle->start_push_flat_length)>EPS))
		return false;
	for(int i=0;i<2;i++)
	{
		if(cut_wing[i]+pPeerAngle->cut_wing[1-i]!=0 && cut_wing[i]+pPeerAngle->cut_wing[1-i]!=3)
			return false;	//��֫״����ͬ
		else if(cut_wing[i]+pPeerAngle->cut_wing[1-i]==3)
		{	//��ͷ������֫����X/Y֫��� wjh-2014.4.8
			for(int j=0;j<3;j++)
			{
				if(fabs(cut_wing_para[i][j]-pPeerAngle->cut_wing_para[1-i][j])>2)
					return false;
			}
		}
		for(int j=0;j<4;j++)
		{
			if(fabs(cut_angle[j][i]-pPeerAngle->cut_angle[3-j][i])>2)
				return false;
		}
	}
	return true;
}
f3dPoint CalOffsetAngleBer(f3dPoint datumPoint,f3dPoint offset_x,f3dPoint offset_y)
{
	double a=offset_y.mod();
	double b=offset_x.mod();
	f3dPoint vec_z=offset_x^offset_y;
	f3dPoint norm_y=vec_z^offset_y;
	normalize(norm_y);
	if(fabs(a)<EPS)
		return datumPoint-offset_x;
	else if(fabs(b)<EPS)
		return datumPoint-offset_y;
	else
	{
		double cosa=(offset_x*offset_y)/(a*b);
		double sina=sqrt(1-cosa*cosa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		double c = (b-a*cosa)/sina;
		f3dPoint angle_ber_pos=datumPoint+norm_y*c-offset_y;
		return angle_ber_pos;
	}
}
CLDSLineAngle *CLDSLineAngle::GetStartNodeOffsetDatumAngle()
{
	if(pStart==NULL||pEnd==NULL||pStart->m_cPosCalType!=7)
		return NULL;
	CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(pStart->arrRelationNode[0]);
	CLDSNode *pAttachStartNode=BelongModel()->FromNodeHandle(pStart->arrRelationNode[1]);
	CLDSNode *pAttachEndNode=BelongModel()->FromNodeHandle(pStart->arrRelationNode[2]);
	if(pDatumNode==NULL)
		return NULL;
	CLDSLineAngle *pOffsetDatumAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(pDatumNode->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	//if(pDatumNode->hFatherPart==handle)
	//	return pOffsetDatumAngle;
	//else
	{
		//����ʵ������Ľṹ����ᵼ�¹��߼������±ߵĴ�������õ���������У���Ƿ�����
		f3dPoint line_vec,line_vec2;
		if(pAttachStartNode&&pAttachEndNode)
			line_vec=pAttachEndNode->Position(true)-pAttachStartNode->Position(true);
		else
			line_vec=pEnd->Position(true)-pStart->Position(true);
		normalize(line_vec);
		if(pOffsetDatumAngle)
		{
			line_vec2=pOffsetDatumAngle->pEnd->Position(true)-pOffsetDatumAngle->pStart->Position(true);
			normalize(line_vec2);
			if(fabs(line_vec*line_vec2)>EPS_COS)
				return pOffsetDatumAngle;
		}
		if(pDatumNode->m_cPosCalType==4)	//�����
		{
			//�жϵ�һ������˼��Ƿ�Ϊƫ�ƻ�׼�˼�
			pOffsetDatumAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(pDatumNode->arrRelationPole[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pOffsetDatumAngle==this)
				return pOffsetDatumAngle;
			line_vec2=pOffsetDatumAngle->pEnd->Position(true)-pOffsetDatumAngle->pStart->Position(true);
			normalize(line_vec2);
			if(fabs(line_vec*line_vec2)>EPS_COS)
				return pOffsetDatumAngle;
			//�жϵڶ�������˼��Ƿ�Ϊƫ�ƻ�׼�˼�
			pOffsetDatumAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(pDatumNode->arrRelationPole[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pOffsetDatumAngle==this)
				return pOffsetDatumAngle;
			line_vec2=pOffsetDatumAngle->pEnd->Position(true)-pOffsetDatumAngle->pStart->Position(true);
			normalize(line_vec2);
			if(fabs(line_vec*line_vec2)>EPS_COS)
				return pOffsetDatumAngle;
		}
		//���������б�ǰӦ��ѹջ wht 12-03-05
		BOOL pushed=m_pModel->PushPartStack();
		for(CLDSPart *pPart=m_pModel->EnumPartFirst(CLS_LINEANGLE);pPart;pPart=m_pModel->EnumPartNext())
		{
			if(!pPart->IsAngle())
				continue;
			pOffsetDatumAngle=(CLDSLineAngle*)pPart;
			if(pOffsetDatumAngle->pStart==pDatumNode||pOffsetDatumAngle->pEnd==pDatumNode)
			{
				line_vec2=pOffsetDatumAngle->pEnd->Position(true)-pOffsetDatumAngle->pStart->Position(true);
				normalize(line_vec2);
				if(fabs(line_vec*line_vec2)>EPS_COS)
				{
					m_pModel->PopPartStack(pushed);
					return pOffsetDatumAngle;
				}
			}
		}
		m_pModel->PopPartStack(pushed);
	}
	return NULL;
}
CLDSLineAngle *CLDSLineAngle::GetEndNodeOffsetDatumAngle()
{
	if(pStart==NULL||pEnd==NULL||pEnd->m_cPosCalType!=7)
		return NULL;
	CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[0]);
	CLDSNode *pAttachStartNode=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[1]);
	CLDSNode *pAttachEndNode=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[2]);
	if(pDatumNode==NULL)
		return NULL;
	CLDSLineAngle *pOffsetDatumAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(pDatumNode->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	//if(pDatumNode->hFatherPart==handle)
	//	return pOffsetDatumAngle;
	//else
	{
		f3dPoint line_vec,line_vec2;
		if(pAttachStartNode&&pAttachEndNode)
			line_vec=pAttachEndNode->Position(true)-pAttachStartNode->Position(true);
		else
			line_vec=pStart->Position(true)-pEnd->Position(true);
		normalize(line_vec);
		if(pOffsetDatumAngle)
		{
			line_vec2=pOffsetDatumAngle->pEnd->Position(true)-pOffsetDatumAngle->pStart->Position(true);
			normalize(line_vec2);
			if(fabs(line_vec*line_vec2)>EPS_COS)
				return pOffsetDatumAngle;
		}
		if(pDatumNode->m_cPosCalType==4)	//�����
		{
			//�жϵ�һ������˼��Ƿ�Ϊƫ�ƻ�׼�˼�
			pOffsetDatumAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(pDatumNode->arrRelationPole[0],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pOffsetDatumAngle==this)
				return pOffsetDatumAngle;
			line_vec2=pOffsetDatumAngle->pEnd->Position(true)-pOffsetDatumAngle->pStart->Position(true);
			normalize(line_vec2);
			if(fabs(line_vec*line_vec2)>EPS_COS)
				return pOffsetDatumAngle;
			//�жϵڶ�������˼��Ƿ�Ϊƫ�ƻ�׼�˼�
			pOffsetDatumAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(pDatumNode->arrRelationPole[1],CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pOffsetDatumAngle==this)
				return pOffsetDatumAngle;
			line_vec2=pOffsetDatumAngle->pEnd->Position(true)-pOffsetDatumAngle->pStart->Position(true);
			normalize(line_vec2);
			if(fabs(line_vec*line_vec2)>EPS_COS)
				return pOffsetDatumAngle;
		}
		//���������б�ǰӦ��ѹջ wht 12-03-05
		BOOL pushed=m_pModel->PushPartStack();
		for(CLDSPart *pPart=m_pModel->EnumPartFirst(CLS_LINEANGLE);pPart;pPart=m_pModel->EnumPartNext())
		{
			if(!pPart->IsAngle())
				continue;
			pOffsetDatumAngle=(CLDSLineAngle*)pPart;
			if(pOffsetDatumAngle->pStart==pDatumNode||pOffsetDatumAngle->pEnd==pDatumNode)
			{
				line_vec2=pOffsetDatumAngle->pEnd->Position(true)-pOffsetDatumAngle->pStart->Position(true);
				normalize(line_vec2);
				if(fabs(line_vec*line_vec2)>EPS_COS)
				{
					m_pModel->PopPartStack(pushed);
					return pOffsetDatumAngle;
				}
			}
		}
		m_pModel->PopPartStack(pushed);
	}
	return NULL;
}
static char IntelliJustifyRodArmDirection(CLDSLinePart* pRod,BOOL bCurrIsStart)
{	//��Ҫ�������ַɻ���������
	CLDSNode *pCurNode=bCurrIsStart?pRod->pStart:pRod->pEnd;
	CLDSNode *pOthNode=bCurrIsStart?pRod->pEnd:pRod->pStart;
	if(pCurNode==NULL||pOthNode==NULL)
		return 0;
	char cArmAxis='X';
	if(pOthNode->Layer(0)=='S'&&pOthNode->Layer(2)>'0'&&pOthNode->Layer(2)<='4')
	{	//��һ�����������������Ͻڵ�	
		if(fabs(pCurNode->Position('X'))<fabs(pCurNode->Position('Y')))
			cArmAxis='Y';	//Y����ᵣ
	}
	else
	{	//
		if(fabs(pCurNode->Position('X')-pOthNode->Position('X'))<fabs(pCurNode->Position('Y')-pOthNode->Position('Y')))
			cArmAxis='Y';	//�˼�X������С��Y������ʱ,�ж�ΪY����ᵣ
	}
	return cArmAxis;

}
//���ܸ��¼򵥶�λʱ�Ǹ�ʼ�˵�λ����Ʋ�����wjh-2016.3.11
#ifdef __ALFA_TEST_VERSION_
//gbLegacyMode���Դ���ΪDebug�汾�����¾ɰ汾���Ĵ���һ���Է��� wjh-2016.4.28
extern BOOL gbLegacyMode/*=FALSE*/;
//gbLegacyAngleCtrlWingModeForTMAV5_1_2������2016.4.29�汾��TMA V5.1.3.0�汾����֫��������㷨�����仯�����ӵĲ��Դ��롡wjh-2016.4.28
extern BOOL gbLegacyAngleCtrlWingModeForTMAV5_1_2/*=FALSE*/;
#endif
bool CLDSLineAngle::IntelliUpdateSimplePosParam(BOOL bStart,CLDSLinePart* pFatherRod/*=NULL*/)
{
	if(pStart==NULL||pEnd==NULL)
		return false;
	CMultiOffsetPos *pCurDesPos=bStart?&desStartPos:&desEndPos;
	CLDSNode *pCurNode=bStart?pStart:pEnd;
	BOOL isInHuoQuLine=pCurDesPos->IsInHuoQuLine()||pCurDesPos->IsInOffsetZhunLine();
	if(pCurDesPos->spatialOperationStyle==0&&(!pCurDesPos->bUdfDatumPointMode||pCurDesPos->m_iSimpleDatumPoint==0))
	{
		char cSelfWorkWing=GetSelfWorkWing(bStart,NULL);
		if(cSelfWorkWing=='X')
		{
			if(pCurDesPos->wing_x_offset.gStyle<4)
				pCurDesPos->wing_x_offset.offsetDist=0;
			pCurDesPos->wing_x_offset.gStyle=4;
			pCurDesPos->wing_y_offset.gStyle=0;
		}
		else if(cSelfWorkWing=='Y')
		{
			if(pCurDesPos->wing_y_offset.gStyle<4)
				pCurDesPos->wing_y_offset.offsetDist=0;
			pCurDesPos->wing_y_offset.gStyle=4;
			pCurDesPos->wing_x_offset.gStyle=0;
		}
		return true;	//ԭ����ָ�����㣬��ǰΪ�����жϻ������ͻ���ָ������(0)
	}
	if(pFatherRod==this)
		pFatherRod=NULL;	//����Ϊ����ʱ���,�Ա������и˼���λ�����б�
	else if(pFatherRod==NULL)
	{
		if(pCurDesPos->datum_jg_h>0x20&&pCurDesPos->datum_jg_h!=handle)
			pFatherRod=BelongModel()->FromRodHandle(pCurDesPos->datum_jg_h);
		else if(pCurNode->hFatherPart!=handle)
			pFatherRod=BelongModel()->FromRodHandle(pCurNode->hFatherPart);
	}
	CLDSGroupLineAngle *pParentGroupAngle=NULL;
	CLDSLineAngle *pFatherJg=(pFatherRod!=NULL&&pFatherRod->IsAngle()) ? (CLDSLineAngle*)pFatherRod : NULL;
	if (pFatherJg!=NULL&&pFatherJg->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pParentGroupAngle=(CLDSGroupLineAngle*)pFatherJg;
	GEPOINT vWorkNorm,vFatherLenVec,lenVec=End()-Start();
	GEPOINT lenStdVec=lenVec;
	normalize(lenStdVec);	//��Ҫ���ü���֫����
	if( pFatherRod!=NULL)
	{
		vFatherLenVec=pFatherRod->End()-pFatherRod->Start();
		normalize(vFatherLenVec);
		vWorkNorm=lenStdVec^vFatherLenVec;	//����ƽ��ķ���
		normalize(vWorkNorm);
	}
	double bendTolerance=max(50,size.wide/2);	//���ݹ���ʵ�������ݲ��������ֵ���ж�ʧ�󣬹ʸ�Ϊ��Ǹ�֫����� wjh-2016.3.16
	if(pFatherJg!=NULL&&(!pCurDesPos->bUdfDatumPointMode||pCurDesPos->m_iSimpleDatumPoint==2))
	{	//�����������ڵ�ĽǸ�һ��ѡ����
		bool bending=false;
		char cDatumAngleWing;
		double justifyx=1.0,justifyy=1.0;
		if(pCurDesPos->bUdfLappedDatumWing&&pCurDesPos->cSimpleDatumAngleWing=='X')
		{
			cDatumAngleWing='X';
			vWorkNorm=pFatherJg->get_norm_x_wing();
			if(pCurDesPos->ciDestLineMode==0&&fabs(vWorkNorm*lenVec)>bendTolerance)	//���Ƕ��빤��ʵ���Բ��簴�˼��������
				bending=true;
		}
		else if(pCurDesPos->bUdfLappedDatumWing&&pCurDesPos->cSimpleDatumAngleWing=='Y')
		{
			cDatumAngleWing='Y';
			vWorkNorm=pFatherJg->get_norm_y_wing();
			if(pCurDesPos->ciDestLineMode==0&&fabs(vWorkNorm*lenVec)>bendTolerance)	//���Ƕ��빤��ʵ���Բ��簴�˼��������
				bending=true; 
		}
		else 	//�Զ��ж�
		{
			double justify=lenStdVec*vFatherLenVec;
			if(((Layer(0)==pFatherRod->Layer(0)&&Layer(0)=='S'&&Layer(1)==pFatherRod->Layer(1)&&Layer(1)=='Z')&&fabs(justify)>0.866)||
				fabs(justify)>0.985)//��������ϴ����ʱjustifyȡ̫����������(0.866=cos(30��),�������ȡ0.985=cos(10��), wjh-2016.3.16
			{
				if(pCurDesPos->spatialOperationStyle==1||pCurDesPos->spatialOperationStyle==4)
				{
					if(pCurDesPos->wing_x_offset.gStyle!=4&&pCurDesPos->wing_y_offset.gStyle!=4)
						pCurDesPos->wing_x_offset.offsetDist=pCurDesPos->wing_y_offset.offsetDist=0;
					pCurDesPos->wing_x_offset.gStyle=pCurDesPos->wing_y_offset.gStyle=4;
					return true;	//�ӽ�����
				}
				else if(Layer(1)=='Z')
				{
					if((bStart&&pFatherJg->pEnd==pStart)||(!bStart&&pFatherJg->pStart==pEnd))
						justify*=-1.0;
					if( justify<0||(bStart&&pStart!=pFatherJg->pStart&&pStart!=pFatherJg->pEnd)||
						(!bStart&&pEnd!=pFatherJg->pStart&&pEnd!=pFatherJg->pEnd))
					{	//���߷������(�н�С��90��),��ǰ�ڵ��Ϊ���˼��˽ڵ�
						if(lenStdVec.z>0.95)
						{
							pCurDesPos->spatialOperationStyle=4;
							pCurDesPos->wing_x_offset.gStyle=pCurDesPos->wing_y_offset.gStyle=4;
							pCurDesPos->wing_x_offset.offsetDist=pCurDesPos->wing_y_offset.offsetDist=0;
							pCurDesPos->face_offset_norm.Set(0,0,1);
						}
						else
							pCurDesPos->spatialOperationStyle=1;
						return true;
					}
				}
			}
			justifyx=fabs(vWorkNorm*pFatherJg->vxWingNorm);
			justifyy=fabs(vWorkNorm*pFatherJg->vyWingNorm);
			if(justifyx>justifyy)	//��׼�Ǹ�X֫Ϊ��ǰ����֫
			{
				cDatumAngleWing='X';
				vWorkNorm=pFatherJg->get_norm_x_wing();
			}
			else					//��׼�Ǹ�Y֫Ϊ��ǰ����֫
			{
				cDatumAngleWing='Y';
				vWorkNorm=pFatherJg->get_norm_y_wing();
			}
			if(pCurDesPos->ciDestLineMode==0&&fabs(vWorkNorm*lenVec)>bendTolerance)	//���Ƕ��빤��ʵ���Բ��簴�˼��������
				bending=true;
		}
		char cFaceOffset=0;
		if(pCurDesPos->IsFaceOffset())
		{
			if(fabs(pCurDesPos->face_offset_norm.x)+fabs(pCurDesPos->face_offset_norm.y)<EPS)
				cFaceOffset='Z';
			else if(fabs(pCurDesPos->face_offset_norm.x)+fabs(pCurDesPos->face_offset_norm.z)<EPS)
				cFaceOffset='Y';
			else if(fabs(pCurDesPos->face_offset_norm.y)+fabs(pCurDesPos->face_offset_norm.z)<EPS)
				cFaceOffset='X';
		}
		if(pFatherJg&&pFatherJg->GetClassTypeId()==CLS_GROUPLINEANGLE&&(
			((CLDSGroupLineAngle*)pFatherJg)->size_cSubClassType=='D'||((CLDSGroupLineAngle*)pFatherJg)->size_cSubClassType=='X'))
			pCurDesPos->spatialOperationStyle=1;
		else if(pParentGroupAngle&&pParentGroupAngle->size_cSubClassType=='T'&&
			(pParentGroupAngle->ciMasterIndex+'X')!=cDatumAngleWing)
			pCurDesPos->spatialOperationStyle=1;
		else if(pCurDesPos->ciDestLineMode!=CMultiOffsetPos::TO_BERLINE)
		{	//ָ����������ʱ�������жϽ�����һ֫����(���������Ľ��ڹ������ĵ����)��wjh-2016.11.29
			if(cDatumAngleWing=='X')	//2:X֫����ͶӰ
				pCurDesPos->spatialOperationStyle=2;
			else //if(cDatumAngleWing=='Y')	//3:Y֫����ͶӰ
				pCurDesPos->spatialOperationStyle=3;
		}
		if((pCurDesPos->ciDestLineMode==0&&bending)||					//�����ж�
			pCurDesPos->ciDestLineMode==CMultiOffsetPos::TO_BENDLINE||	//ָ�����ڻ�����
			pCurDesPos->ciDestLineMode==CMultiOffsetPos::TO_OFFSETZHUN)	//ָ������ƫ��׼��
		{
			pCurDesPos->spatialOperationStyle+=6;
			if(!isInHuoQuLine)
				pCurDesPos->huoqu_dist=sysPara!=NULL ? sysPara->GetBendClearance() : 10;
		}
		if(pCurNode->m_cPosCalType>0&&pCurNode->m_cPosCalType<=3)	//1~3.�˼���X/Y/Z����ֵ�����
			cFaceOffset='X'+pCurNode->m_cPosCalType-1;
		else if(pCurNode->m_cPosCalType==6)	//6.�ظ˼���ĳ�ڵ����������Ľڵ�
			cFaceOffset=pCurNode->m_cPosCalViceType;	//�˼���X/Y/Z����ֵ�ȸߵ�
		//else if(pCurNode->m_cPosCalType
		else if((pFatherJg->Layer(0)=='S'||pFatherJg->Layer(0)=='L')&&pFatherJg->Layer(1)=='Z'&&fabs(vFatherLenVec.z)>0.5)
			cFaceOffset='Z';
		/*
		1.���ڴ�������Ǹ������Ӷ�λ��׼,��ʱ�򵥶�λ�ٽ�������Ϊƽ��,��û������,���ҷ������ܵ��¾Ʊ��������ۡ�ͷ���ᵣ����������,����б�Ĵ�Ӵ���
		2.���ڴ��ڴ�Ӷ�λ��׼���,���������е�ָ����������ڵ����Ķ�Ӧ��Ϊƽ�� wjh-2016.4.26
		3.Ѽ��ᵣ������������֫������Ƽ�ƽ�ƣ�����ǰ�����Ӻ�ļ�����б���������Ƶ�һ��Ӧͬʱ����һ�����趨(OK)
		*/
		bool priorFaceOffsetProcessed=false;
#ifndef __ALFA_TEST_VERSION_
		BOOL gbLegacyMode=FALSE;
#endif
		if(pFatherJg&&!gbLegacyMode)
		{	//����ʶ��Ѽ��ᵣ������������֫������Ƽ�ƽ�ƣ�����ǰ�����Ӻ�ļ�����б���������Ƶ�һ��Ӧͬʱ����һ�����趨
			CMultiOffsetPos* pDatumDesPos=NULL;
			if((bStart&&pStart==pFatherJg->pStart)||(!bStart&&pEnd==pFatherJg->pStart))
				pDatumDesPos=&pFatherJg->desStartPos;
			else if((bStart&&pStart==pFatherJg->pEnd)||(!bStart&&pEnd==pFatherJg->pEnd))
				pDatumDesPos=&pFatherJg->desEndPos;
			WING_OFFSET_STYLE* pWorkOffsetStyle=NULL;
			if(pDatumDesPos)
			{
				if(pDatumDesPos->spatialOperationStyle==15&&pDatumDesPos->cMainCoordCtrlWing==cDatumAngleWing)
					pWorkOffsetStyle=&pDatumDesPos->ctrlWing;
				else if(pDatumDesPos->spatialOperationStyle==15)
					pWorkOffsetStyle=&pDatumDesPos->otherWing;
			}
			if(pWorkOffsetStyle&&pWorkOffsetStyle->operStyle>0&&pWorkOffsetStyle->operStyle<4)
			{
				priorFaceOffsetProcessed=true;
				if(pWorkOffsetStyle->operStyle==1)
					cFaceOffset='Z';
				else if(pWorkOffsetStyle->operStyle==2)
					cFaceOffset='X';
				else if(pWorkOffsetStyle->operStyle==3)
					cFaceOffset='Y';
				else
					priorFaceOffsetProcessed=false;
				if(priorFaceOffsetProcessed)
					pCurDesPos->spatialOperationStyle+=3;
			}
			else if(pWorkOffsetStyle&&pWorkOffsetStyle->operStyle==0)
				priorFaceOffsetProcessed=true;
		}
		if( cFaceOffset>='X'&&cFaceOffset<='Z'&&pFatherJg&&!priorFaceOffsetProcessed&&	//����ͶӰ->ƽ��
			pFatherJg->m_cPosCtrlDatumLineType!='X'&&pFatherJg->m_cPosCtrlDatumLineType!='Y')
			pCurDesPos->spatialOperationStyle+=3;
		if(cFaceOffset>='X'&&cFaceOffset<='Z')
		{	//ֻ����ƽ����Чʱ�Ÿ���pCurDesPos->face_offset_norm���������ڱȶԲ��� wjh-2016.3.16
			pCurDesPos->face_offset_norm.x=(cFaceOffset=='X') ? 1.0 : 0.0;
			pCurDesPos->face_offset_norm.y=(cFaceOffset=='Y') ? 1.0 : 0.0;
			pCurDesPos->face_offset_norm.z=(cFaceOffset=='Z') ? 1.0 : 0.0;
		}
		if(pCurDesPos->IsFaceOffset()&&fabs(pCurDesPos->face_offset_norm*vFatherLenVec)<0.25)
			pCurDesPos->spatialOperationStyle-=3;	//ƽ������˼��н�̫С(15��)����ʹ��ƽ�� wjh-2016.3.16
		if( pCurDesPos->ciDestLineMode==CMultiOffsetPos::TO_OFFSETZHUN||(pCurDesPos->ciDestLineMode==0&&
			pFatherJg->Layer(0)=='L'&&pFatherJg->Layer(1)=='X'&&pCurDesPos->IsInHuoQuLine()))
		{	//V�渨��,Ĭ�ϸ���Ϊƫ��׼��
			if(pCurDesPos->IsInDatumJgWingX())
				pCurDesPos->spatialOperationStyle=13;	//X֫ƫ��׼��
			else
				pCurDesPos->spatialOperationStyle=14;	//Y֫ƫ��׼��
		}
	}
	else if(!pCurDesPos->bUdfDatumPointMode&&pFatherJg==NULL&&pFatherRod!=NULL)
		pCurDesPos->spatialOperationStyle=1;	//��׼�˼����ǽǸ�,��ֹܵ�,Ĭ�ϴ���ڻ�׼����
	else if(!pCurDesPos->bUdfDatumPointMode||pCurDesPos->m_iSimpleDatumPoint==1)
	{	//�ڵ�Ϊ���ƽڵ�(���ٿ��������������)
		char cFaceOffset=0;
		if(pCurDesPos->bUdfCtrlCoordsMode&&pCurDesPos->m_iSimpleDatumPoint==1)
		{	//ָ�����Ƶ����͵ĸ�֫��������ʱ,������Ҫ��spatialOperationStyle��cMainCoordCtrlWing
			if(pCurNode->m_cPosCalType>=8)
			{	//��׼ƽ���ϵĽڵ�����һ�㶼�����ڱ�֤�Ǹ�ĳһ֫λ��һ����ƽ���ϵ�,��Ӧ��Ϊָ����׼�㣬���ౣ�ֲ��� wjh-2016.3.29
				pCurDesPos->spatialOperationStyle=0;
				pCurDesPos->datumPoint.datum_pos_style=8;	//��ڵ�ͬλ
				pCurDesPos->datumPoint.des_para.hNode=pCurNode->handle;
			}
		}
		else if((Layer(0)=='S'||Layer(0)=='L')&&Layer(1)=='Z'&&fabs(lenStdVec.z)>0.5)
		{
			if(pCurDesPos->spatialOperationStyle==16||pCurDesPos->spatialOperationStyle==17)
				pCurDesPos->spatialOperationStyle=17;	//ԭ�����߶�λ�ļ�������
			else
				pCurDesPos->spatialOperationStyle=19;	//�����Ϊ������Ƶ�ƽ��
			pCurDesPos->face_offset_norm.Set(0,0,1.0);	//ƽ����Ϊˮƽ��
			if(fabs(lenStdVec.x)+EPS>fabs(lenStdVec.y))
			{	//�����¶ȴ��������¶�,ȡ����ΪZ������׼��
				if(fabs(norm_x_wing.y)>fabs(norm_y_wing.y))
					pCurDesPos->cFaceOffsetDatumLine='X';
				else
					pCurDesPos->cFaceOffsetDatumLine='Y';
			}
			else if(fabs(lenStdVec.x)<fabs(lenStdVec.y))
			{	//�����¶�С�������¶�,ȡ����ΪZ������׼��
				if(fabs(norm_x_wing.x)>fabs(norm_y_wing.x))
					pCurDesPos->cFaceOffsetDatumLine='X';
				else
					pCurDesPos->cFaceOffsetDatumLine='Y';
			}
			cFaceOffset='Z';
		}
		else
		{
			if(pCurNode->m_cPosCalType==0)	//�ڵ�Ϊ�������κ������ڵ�ʱ�ڵ㸸�˼�ӦĬ����Ϊ��֫�������
			{
				pCurDesPos->spatialOperationStyle=15;	//��֫�������,��֫�������ʱ�Ŀ���֫Ҳ��Ҫ�����ж� wjh-2016.3.29
				pCurDesPos->ctrlWing.gStyle=pCurDesPos->otherWing.gStyle=0;
				char cArmAxis=IntelliJustifyRodArmDirection(this,bStart);//'X';
				if(cArmAxis=='Y')//fabs(pCurNode->Position('X'))<fabs(pCurNode->Position('Y')))
				{	//Y����ᵣ
					if(fabs(norm_x_wing.x)>fabs(norm_y_wing.x))
						pCurDesPos->cMainCoordCtrlWing='X';
					else
						pCurDesPos->cMainCoordCtrlWing='Y';
					pCurDesPos->coordCtrlType=1;
					if(fabs(lenStdVec.y)>=fabs(lenStdVec.z))
						pCurDesPos->otherWing.operStyle=pCurDesPos->ctrlWing.operStyle=3;	//X-Zƽ��Ϊƽ����
					else
						pCurDesPos->otherWing.operStyle=pCurDesPos->ctrlWing.operStyle=1;	//X-Yƽ��Ϊƽ����
				}
				else
				{	//X����ᵣ
					if(fabs(norm_x_wing.y)>fabs(norm_y_wing.y))
						pCurDesPos->cMainCoordCtrlWing='X';
					else
						pCurDesPos->cMainCoordCtrlWing='Y';
					pCurDesPos->coordCtrlType=0;
					if(fabs(lenStdVec.x)>=fabs(lenStdVec.z))
						pCurDesPos->otherWing.operStyle=pCurDesPos->ctrlWing.operStyle=2;	//Y-Zƽ��Ϊƽ����
					else
						pCurDesPos->otherWing.operStyle=pCurDesPos->ctrlWing.operStyle=1;	//X-Yƽ��Ϊƽ����
				}
			}
			else if(pCurNode->m_cPosCalType==4)
			{
				if(pCurDesPos->spatialOperationStyle==16||pCurDesPos->spatialOperationStyle==17||group_father_jg_h>0x20)
					return true;	//����ڵ���Ϊ�˼��˽ڵ�ʱ��Ӧ����ԭ�е���ͶӰ��ƽ��ģʽ

				int jointAtStart0End1=2;
				if(bStart)
				{
					GEPOINT lineLenStdVec=pEnd->Position(false)-pStart->Position(false);
					normalize(lineLenStdVec);
					BOOL pushed=BelongModel()->PushPartStack();
					for(CLDSLinePart* pRod=BelongModel()->EnumRodFirst();pRod;pRod=BelongModel()->EnumRodNext())
					{
						if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod==this||pRod->GetClassTypeId()!=GetClassTypeId())
							continue;
						GEPOINT buddyLenVec;
						if(pRod->pStart==pStart)
							buddyLenVec=pRod->pEnd->Position(false)-pRod->pStart->Position(false);
						else if(pRod->pEnd==pStart)
							buddyLenVec=pRod->pStart->Position(false)-pRod->pEnd->Position(false);
						else
							continue;
						if(lineLenStdVec*buddyLenVec<-EPS_COS)	//����
							break;
					}
					BelongModel()->PopPartStack(pushed);
					if(pRod&&pRod->pStart==pStart)
						jointAtStart0End1=0;
					else if(pRod&&pRod->pEnd==pStart)
						jointAtStart0End1=1;
					if(jointAtStart0End1<2)
					{
						pCurDesPos->spatialOperationStyle=16;	//������ͶӰ
						pCurDesPos->datumLineStart.datum_pos_style=1;
						pCurDesPos->datumLineStart.des_para.RODEND.hRod=pRod->handle;
						pCurDesPos->datumLineStart.des_para.RODEND.direction=1-jointAtStart0End1;
						pCurDesPos->datumLineStart.des_para.RODEND.SetWingOffset(0);
						pCurDesPos->datumLineEnd.datum_pos_style=1;
						pCurDesPos->datumLineEnd.des_para.RODEND.hRod=handle;
						pCurDesPos->datumLineEnd.des_para.RODEND.direction=1;
						pCurDesPos->datumLineEnd.des_para.RODEND.SetWingOffset(0);
					}
				}
				else
				{
					GEPOINT lineLenStdVec=pStart->Position(false)-pEnd->Position(false);
					normalize(lineLenStdVec);
					CLdsPartListStack stack(BelongModel());
					for(CLDSLinePart* pRod=BelongModel()->EnumRodFirst();pRod;pRod=BelongModel()->EnumRodNext())
					{
						if(pRod->pStart==NULL||pRod->pEnd==NULL||pRod==this||pRod->GetClassTypeId()!=GetClassTypeId())
							continue;
						GEPOINT buddyLenVec=pRod->pStart->Position(false)-pRod->pEnd->Position(false);
						if(pRod->pStart==pEnd)
							buddyLenVec*=-1;
						if(lineLenStdVec*buddyLenVec<-EPS_COS)	//����
							break;
					}
					if(pRod&&pRod->pStart==pEnd)
						jointAtStart0End1=0;
					else if(pRod&&pRod->pEnd==pEnd)
						jointAtStart0End1=1;
					if(jointAtStart0End1<2)
					{
						pCurDesPos->spatialOperationStyle=16;	//������ͶӰ
						pCurDesPos->datumLineStart.datum_pos_style=1;
						pCurDesPos->datumLineStart.des_para.RODEND.hRod=pRod->handle;
						pCurDesPos->datumLineStart.des_para.RODEND.direction=1-jointAtStart0End1;
						pCurDesPos->datumLineStart.des_para.RODEND.SetWingOffset(0);
						pCurDesPos->datumLineEnd.datum_pos_style=1;
						pCurDesPos->datumLineEnd.des_para.RODEND.hRod=handle;
						pCurDesPos->datumLineEnd.des_para.RODEND.direction=0;
						pCurDesPos->datumLineEnd.des_para.RODEND.SetWingOffset(0);
					}
				}
			}
			else if(pCurNode->m_cPosCalType>0&&pCurNode->m_cPosCalType<8)
			{	//���������ڵ���Ϊ��׼�Ǹֶ˽ڵ����,�Զ��ж�ʱֻ����ʱȡ�������Ŀ��Ƶ�ͶӰ
				if( pCurDesPos->spatialOperationStyle!=15&&	//���ܸı�֮ǰ��õĶ�λ��ʽ wjh-2018.4.30
					pCurDesPos->spatialOperationStyle!=18&&pCurDesPos->spatialOperationStyle!=19)
					pCurDesPos->spatialOperationStyle=18;
			}
			else	//����ڵ�����һ�㶼�����ڱ�֤�Ǹ�ĳһ֫λ��һ����ƽ���ϵ�,��Ӧ��Ϊָ����׼��
			{
				pCurDesPos->spatialOperationStyle=0;
				pCurDesPos->datumPoint.datum_pos_style=8;	//��ڵ�ͬλ
				pCurDesPos->datumPoint.des_para.hNode=pCurNode->handle;
			}
		}
	}
	if(!vWorkNorm.IsZero()&&pCurDesPos->spatialOperationStyle<=14&&pCurDesPos->ciDestLineMode!=CMultiOffsetPos::TO_BERLINE)
	{
		char cSelfWorkWing=0;
		if(pCurDesPos->cWorkWingStyle==1)	//X֫Ϊ��ǰ����֫
			cSelfWorkWing='X';
		else if(pCurDesPos->cWorkWingStyle==2)	//Y֫Ϊ��ǰ����֫
			cSelfWorkWing='Y';
		else if(pCurDesPos->cWorkWingStyle==0)	//�Զ��жϹ���֫
		{
			if(fabs(norm_x_wing*vWorkNorm)>fabs(norm_y_wing*vWorkNorm))
				cSelfWorkWing='X';
			else
				cSelfWorkWing='Y';
		}
		double norm_offset=(pCurDesPos->wing_x_offset.gStyle==4)?pCurDesPos->wing_x_offset.offsetDist:0;
		if(norm_offset==0&&pCurDesPos->wing_y_offset.gStyle==4)
			norm_offset=pCurDesPos->wing_y_offset.offsetDist;
		if(cSelfWorkWing=='X')
		{
			if(GetClassTypeId()!=CLS_GROUPLINEANGLE)
				pCurDesPos->wing_y_offset.gStyle=0;
			else if(size.cSubClassType!='T'||((CLDSGroupLineAngle*)this)->ciMasterIndex==1)
			{
				pCurDesPos->wing_y_offset.gStyle=4;
				pCurDesPos->wing_y_offset.offsetDist=0;
			}
			else
				pCurDesPos->wing_y_offset.gStyle=0;
			pCurDesPos->wing_x_offset.gStyle=4;
			pCurDesPos->wing_x_offset.offsetDist=norm_offset;
		}
		else if(cSelfWorkWing=='Y')
		{
			if(GetClassTypeId()!=CLS_GROUPLINEANGLE)
				pCurDesPos->wing_x_offset.gStyle=0;
			else if(size.cSubClassType!='T'||((CLDSGroupLineAngle*)this)->ciMasterIndex!=1)
			{
				pCurDesPos->wing_x_offset.gStyle=4;
				pCurDesPos->wing_x_offset.offsetDist=0;
			}
			else
				pCurDesPos->wing_x_offset.gStyle=0;
			pCurDesPos->wing_y_offset.gStyle=4;
			pCurDesPos->wing_y_offset.offsetDist=norm_offset;
		}
		else
			pCurDesPos->wing_x_offset.gStyle=pCurDesPos->wing_y_offset.gStyle=0;
	}
	return true;
}

char CLDSLineAngle::GetSelfWorkWing(BOOL bStart,const double* worknorm)	//��ǰ�Ǹ�ʼ��������֫
{
	GEPOINT vWorkNorm(worknorm);
	CMultiOffsetPos* pDesPos=bStart ? &desStartPos : &desEndPos;
#ifndef __ALFA_TEST_VERSION_
	BOOL gbLegacyMode=FALSE;
#endif
	if(vWorkNorm.IsZero()&&pDesPos->cWorkWingStyle==0)
	{
		CLDSLinePart* pDependRod=NULL;
		if(pDesPos->spatialOperationStyle==0&&pDesPos->datumPoint.datum_pos_style>0&&pDesPos->datumPoint.datum_pos_style<=3)
			pDependRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pDesPos->datumPoint.des_para.hPart,CLS_LINEPART);
		else
			pDependRod=bStart?GetStartDatumPart():GetEndDatumPart();
		if(pDependRod==NULL||pDependRod==this)
			return 0;	//ȱ�ٸ��˼�ʱ�Զ��ж��޷��жϵ�ǰ����֫
		CLDSLineAngle* pDependAngle=pDependRod->IsLinePart()&&pDependRod->IsAngle() ? (CLDSLineAngle*)pDependRod : NULL;
		if(!gbLegacyMode&&pDependAngle&&((norm_x_wing*pDependAngle->vxWingNorm>EPS_COS2)&&(norm_y_wing*pDependAngle->vyWingNorm>EPS_COS2)||
			(norm_x_wing*pDependAngle->vyWingNorm>EPS_COS2)&&(norm_y_wing*pDependAngle->vxWingNorm>EPS_COS2)))
			return 3;	//�Զ��ж�Ϊ˫֫��λ wjh-2016.4.28
		GEPOINT base_vec=pDependRod->End()-pDependRod->Start();
		GEPOINT curr_vec=End()-Start();
		vWorkNorm=base_vec^curr_vec;
		if(!normalize(vWorkNorm))
			return 0;
	}
	if(pDesPos->cWorkWingStyle==3)
		return 3;
	else if(pDesPos->cWorkWingStyle==1)
		return 'X';
	else if(pDesPos->cWorkWingStyle==2)
		return 'Y';
	else if(fabs(norm_x_wing*vWorkNorm)>fabs(norm_y_wing*vWorkNorm))
		return 'X';
	else //if(fabs(norm_x_wing*vWorkNorm)<fabs(norm_y_wing*vWorkNorm))
		return 'Y';
}
char CLDSLineAngle::GetLappedWorkWing(BOOL bStart,const double* worknorm)//ʼ�˻�׼�ǸֵĴ��֫
{
	CMultiOffsetPos* pDesPos=bStart?&desStartPos:&desEndPos;
	if(pDesPos->bUdfLappedDatumWing)
	{
		if(pDesPos->cDatumAngleWing=='X')
			return 'X';
		else if(pDesPos->cDatumAngleWing=='Y')
			return 'Y';
		else if(pDesPos->IsInDatumJgWingX())
			return 'X';
		else if(pDesPos->IsInDatumJgWingY())
			return 'Y';
		else
			return 0;
	}
	GEPOINT vWorkNorm(worknorm);
	CLDSLinePart* pDependRod=bStart?GetStartDatumPart():GetEndDatumPart();
	if(pDependRod==NULL || !pDependRod->IsAngle())
		return 0;	//�ǽǸֻ�׼�˼�
	if(vWorkNorm.IsZero())
	{
		GEPOINT base_vec=pDependRod->End()-pDependRod->Start();
		GEPOINT curr_vec=End()-Start();
		vWorkNorm=base_vec^curr_vec;
		normalize(vWorkNorm);
	}
	CLDSLineAngle* pDependAngle=(CLDSLineAngle*)pDependRod;
	if(vWorkNorm.IsZero())
		return 0;
	else if(fabs(pDependAngle->vxWingNorm*vWorkNorm)>fabs(pDependAngle->vyWingNorm*vWorkNorm))
		return 'X';
	else //if(fabs(pDependAngle->vxWingNorm*vWorkNorm)<fabs(pDependAngle->vyWingNorm*vWorkNorm))
		return 'Y';
}
//���ض̽Ǹ�(ʼĩ�˽ڵ�Ϊ�յĶ̽Ǹ�)�ĵ�ǰ��λ����֫
int GetShortAngleWorkWing(CLDSLineAngle* pShortAngle)
{
	bool datumWingX=true,datumWingY=true;
	if(pShortAngle->des_norm_x.bSpecific||pShortAngle->des_norm_x.bByOtherWing||pShortAngle->des_norm_x.hViceJg<0x20)
		datumWingX=false;
	if(pShortAngle->des_norm_y.bSpecific||pShortAngle->des_norm_y.bByOtherWing||pShortAngle->des_norm_y.hViceJg<0x20)
		datumWingY=false;
	if( datumWingX&&pShortAngle->desStartPos.datumPoint.des_para.hPart==pShortAngle->des_norm_x.hViceJg&&
		pShortAngle->desEndPos.datumPoint.des_para.hPart==pShortAngle->des_norm_x.hViceJg)
	{
		if(datumWingY&&pShortAngle->des_norm_y.hViceJg==pShortAngle->des_norm_x.hViceJg)
			return 2;	//˫֫��λ����
		else
			return 0;
	}
	else if(datumWingY&&pShortAngle->desStartPos.datumPoint.des_para.hPart==pShortAngle->des_norm_y.hViceJg&&
		pShortAngle->desEndPos.datumPoint.des_para.hPart==pShortAngle->des_norm_y.hViceJg)
		return 1;	//Y֫Ϊ��λ����֫
	return 2;
}
BYTE CLDSLineAngle::get_ciStartAnchorWing()
{
	if(desStartPos.cWorkWingStyle>=1&&desStartPos.cWorkWingStyle<=3)
		return desStartPos.cWorkWingStyle;	//�û���ָ����λ����֫
	//
	if( (desStartPos.spatialOperationStyle==17||desStartPos.spatialOperationStyle==19)&&
		(desStartPos.cFaceOffsetDatumLine=='X'||desStartPos.cFaceOffsetDatumLine=='Y'))
		return desStartPos.cFaceOffsetDatumLine-'W';//��ʽ�ӿ��ƽڵ�ͶӰ/ƽ�Ƽ���Ǹ���㣬ָ����Ӷ�λ��׼֫
	else if(desStartPos.spatialOperationStyle>=16&&(m_cPosCtrlDatumLineType=='X'||m_cPosCtrlDatumLineType=='Y'))
		return m_cPosCtrlDatumLineType-'W';
	else if(desStartPos.spatialOperationStyle<15)
	{	//��ʽ�ӿ��ƽڵ�ͶӰ����Ǹ�������ͨ��Ӽ���Ǹ����
		CLDSLinePart* pDependRod=NULL;
		if(desStartPos.spatialOperationStyle==0&&desStartPos.datumPoint.datum_pos_style>0&&desStartPos.datumPoint.datum_pos_style<=3)
			pDependRod=(CLDSLinePart*)BelongModel()->FromPartHandle(desStartPos.datumPoint.des_para.hPart,CLS_LINEPART);
		else
			pDependRod=GetStartDatumPart();
		if(pDependRod==NULL||pDependRod==this)
			return 3;	//ȱ�ٸ��˼�ʱ�Զ��ж��޷��жϵ�ǰ����֫,����Ϊ˫֫��λ
		CLDSLineAngle* pDependAngle=pDependRod->IsLinePart()&&pDependRod->IsAngle() ? (CLDSLineAngle*)pDependRod : NULL;
		if(pDependAngle)
		{
			if(desStartPos.IsInDatumJgWingX())
				return fabs(norm_x_wing*pDependAngle->vxWingNorm)>=fabs(norm_y_wing*pDependAngle->vxWingNorm)?1:2;
			else if(desStartPos.IsInDatumJgWingY())
				return fabs(norm_x_wing*pDependAngle->vyWingNorm)>=fabs(norm_y_wing*pDependAngle->vyWingNorm)?1:2;
			else if(((norm_x_wing*pDependAngle->vxWingNorm>EPS_COS2)&&(norm_y_wing*pDependAngle->vyWingNorm>EPS_COS2)||
				(norm_x_wing*pDependAngle->vyWingNorm>EPS_COS2)&&(norm_y_wing*pDependAngle->vxWingNorm>EPS_COS2)))
				return 3;	//�Զ��ж�Ϊ˫֫��λ wjh-2016.4.28
		}
		f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
		BYTE work_wing_style=desStartPos.cWorkWingStyle;
		//��ʽ�ӿ��ƽڵ�ͶӰ����Ǹ����
		if(pDependRod->handle==handle)
		{	//��ʼ�ڵ㸸�Ǹ�Ϊ��ǰ�Ǹ֣���ʱƫ�Ʒ����ؽǸ���֫����
			if( desStartPos.wing_x_offset.gStyle==4&&desStartPos.wing_x_offset.offsetDist==0&&(
				desStartPos.wing_y_offset.gStyle!=4||desStartPos.wing_y_offset.offsetDist!=0))
				return 1;	//X֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��X֫Ϊ��ǰ����֫
			else if(desStartPos.wing_y_offset.gStyle==4&&desStartPos.wing_y_offset.offsetDist==0&&(
					desStartPos.wing_x_offset.gStyle!=4||desStartPos.wing_x_offset.offsetDist!=0))
				return 2;	//Y֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��Y֫Ϊ��ǰ����֫
			else
				return 3;
		}
		else
		{	//��ӽǸ��ص�ǰ����֫�ķ��߼�֫�������ƫ��
			f3dPoint father_vec,cur_vec,work_norm;
			cur_vec = End()-Start();
			father_vec = pDependRod->End()-pDependRod->Start();
			normalize(cur_vec);
			normalize(father_vec);
			if(pDependRod->GetClassTypeId()==CLS_LINEANGLE||pDependRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				if(desStartPos.IsInDatumJgWingX())
					work_norm=pDependAngle->vxWingNorm;
				else if(desStartPos.IsInDatumJgWingY())
					work_norm=pDependAngle->vyWingNorm;
				else if((norm_x_wing*pDependAngle->vxWingNorm>EPS_COS2&&norm_y_wing*pDependAngle->vyWingNorm>EPS_COS2)||
						(norm_x_wing*pDependAngle->vyWingNorm>EPS_COS2&&norm_y_wing*pDependAngle->vxWingNorm>EPS_COS2))
					work_wing_style=3;	//�Զ��ж�Ϊ˫֫��λ
				else
					work_norm=father_vec^cur_vec;
			}
			else if(fabs(father_vec*cur_vec)<EPS_COS2)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
				work_norm = father_vec^cur_vec;
			if(work_wing_style!=3&&normalize(work_norm)!=0)
			{
				if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
					work_wing_style=1;
				else
					work_wing_style=2;
			}
			else
				work_wing_style=3;
			return work_wing_style;
		}
	}
	else if(desStartPos.spatialOperationStyle==18)
	{
		if( desStartPos.wing_x_offset.gStyle==4&&desStartPos.wing_x_offset.offsetDist==0&&(
			desStartPos.wing_y_offset.gStyle!=4||desStartPos.wing_y_offset.offsetDist!=0))
			return 1;	//X֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��X֫Ϊ��ǰ����֫
		else if(desStartPos.wing_y_offset.gStyle==4&&desStartPos.wing_y_offset.offsetDist==0&&(
				desStartPos.wing_x_offset.gStyle!=4||desStartPos.wing_x_offset.offsetDist!=0))
			return 2;	//Y֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��Y֫Ϊ��ǰ����֫
		else
			return 3;
	}
	else
		return 3;
	return 0;
}
BYTE CLDSLineAngle::get_ciEndAnchorWing()
{
	if(desEndPos.cWorkWingStyle>=1&&desEndPos.cWorkWingStyle<=3)
		return desEndPos.cWorkWingStyle;	//�û���ָ����λ����֫
	//
	if( (desEndPos.spatialOperationStyle==17||desEndPos.spatialOperationStyle==19)&&
		(desEndPos.cFaceOffsetDatumLine=='X'||desEndPos.cFaceOffsetDatumLine=='Y'))
		return desEndPos.cFaceOffsetDatumLine-'W';//��ʽ�ӿ��ƽڵ�ͶӰ/ƽ�Ƽ���Ǹ���㣬ָ����Ӷ�λ��׼֫
	else if(desEndPos.spatialOperationStyle>=16&&(m_cPosCtrlDatumLineType=='X'||m_cPosCtrlDatumLineType=='Y'))
		return m_cPosCtrlDatumLineType-'W';
	else if(desEndPos.spatialOperationStyle<15)
	{	//��ʽ�ӿ��ƽڵ�ͶӰ����Ǹ�������ͨ��Ӽ���Ǹ����
		CLDSLinePart* pDependRod=NULL;
		if(desEndPos.spatialOperationStyle==0&&desEndPos.datumPoint.datum_pos_style>0&&desEndPos.datumPoint.datum_pos_style<=3)
			pDependRod=(CLDSLinePart*)BelongModel()->FromPartHandle(desEndPos.datumPoint.des_para.hPart,CLS_LINEPART);
		else
			pDependRod=GetEndDatumPart();
		if(pDependRod==NULL||pDependRod==this)
			return 3;	//ȱ�ٸ��˼�ʱ�Զ��ж��޷��жϵ�ǰ����֫,����Ϊ˫֫��λ
		CLDSLineAngle* pDependAngle=pDependRod->IsLinePart()&&pDependRod->IsAngle() ? (CLDSLineAngle*)pDependRod : NULL;
		if(pDependAngle)
		{
			if(desEndPos.IsInDatumJgWingX())
				return fabs(norm_x_wing*pDependAngle->vxWingNorm)>=fabs(norm_y_wing*pDependAngle->vxWingNorm)?1:2;
			else if(desEndPos.IsInDatumJgWingY())
				return fabs(norm_x_wing*pDependAngle->vyWingNorm)>=fabs(norm_y_wing*pDependAngle->vyWingNorm)?1:2;
			else if(((norm_x_wing*pDependAngle->vxWingNorm>EPS_COS2)&&(norm_y_wing*pDependAngle->vyWingNorm>EPS_COS2)||
				(norm_x_wing*pDependAngle->vyWingNorm>EPS_COS2)&&(norm_y_wing*pDependAngle->vxWingNorm>EPS_COS2)))
				return 3;	//�Զ��ж�Ϊ˫֫��λ wjh-2016.4.28
		}
		f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
		BYTE work_wing_style=desEndPos.cWorkWingStyle;
		//��ʽ�ӿ��ƽڵ�ͶӰ����Ǹ����
		if(pDependRod->handle==handle)
		{	//��ʼ�ڵ㸸�Ǹ�Ϊ��ǰ�Ǹ֣���ʱƫ�Ʒ����ؽǸ���֫����
			if( desEndPos.wing_x_offset.gStyle==4&&desEndPos.wing_x_offset.offsetDist==0&&(
				desEndPos.wing_y_offset.gStyle!=4||desEndPos.wing_y_offset.offsetDist!=0))
				return 1;	//X֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��X֫Ϊ��ǰ����֫
			else if(desEndPos.wing_y_offset.gStyle==4&&desEndPos.wing_y_offset.offsetDist==0&&(
					desEndPos.wing_x_offset.gStyle!=4||desEndPos.wing_x_offset.offsetDist!=0))
				return 2;	//Y֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��Y֫Ϊ��ǰ����֫
			else
				return 3;
		}
		else
		{	//��ӽǸ��ص�ǰ����֫�ķ��߼�֫�������ƫ��
			f3dPoint father_vec,cur_vec,work_norm;
			cur_vec = End()-Start();
			father_vec = pDependRod->End()-pDependRod->Start();
			normalize(cur_vec);
			normalize(father_vec);
			if(pDependRod->GetClassTypeId()==CLS_LINEANGLE||pDependRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				if(desEndPos.IsInDatumJgWingX())
					work_norm=pDependAngle->vxWingNorm;
				else if(desEndPos.IsInDatumJgWingY())
					work_norm=pDependAngle->vyWingNorm;
				else if((norm_x_wing*pDependAngle->vxWingNorm>EPS_COS2&&norm_y_wing*pDependAngle->vyWingNorm>EPS_COS2)||
						(norm_x_wing*pDependAngle->vyWingNorm>EPS_COS2&&norm_y_wing*pDependAngle->vxWingNorm>EPS_COS2))
					work_wing_style=3;	//�Զ��ж�Ϊ˫֫��λ
				else
					work_norm=father_vec^cur_vec;
			}
			else if(fabs(father_vec*cur_vec)<EPS_COS2)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
				work_norm = father_vec^cur_vec;
			if(work_wing_style!=3&&normalize(work_norm)!=0)
			{
				if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
					work_wing_style=1;
				else
					work_wing_style=2;
			}
			else
				work_wing_style=3;
			return work_wing_style;
		}
	}
	else if(desEndPos.spatialOperationStyle==18)
	{
		if( desEndPos.wing_x_offset.gStyle==4&&desEndPos.wing_x_offset.offsetDist==0&&(
			desEndPos.wing_y_offset.gStyle!=4||desEndPos.wing_y_offset.offsetDist!=0))
			return 1;	//X֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��X֫Ϊ��ǰ����֫
		else if(desEndPos.wing_y_offset.gStyle==4&&desEndPos.wing_y_offset.offsetDist==0&&(
				desEndPos.wing_x_offset.gStyle!=4||desEndPos.wing_x_offset.offsetDist!=0))
			return 2;	//Y֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��Y֫Ϊ��ǰ����֫
		else
			return 3;
	}
	else
		return 3;
	return 0;
}
//�¸��ĽǸ����λ�ò������ԭ����ʼ��λ�ü��㺯�� wjh-2012-4-28
void CLDSLineAngle::CalStartPosition(CLDSArcLift *pLift/*=NULL*/)
{
	if(BelongModel()==NULL)
		return;
	if(!LockStart())
		return;	//�ѵ�λ
#ifdef __COMMON_PART_INC_
	if(pLift==NULL&&m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#else
	pLift=NULL;
#endif
	if(IsStartPosLocked())
		return;			//ʼ��λ���ѱ������������ǻ����㣩
	JGZJ jgzj;
	double sina;
	CSuperSmartPtr<CLDSLinePart> pDatumLinePart;
	if(desStartPos.jgber_cal_style==1)	//ֱ��ָ��������꣬����Ҫ����������
	{
		if(pLift)
			baseline.SetStart(pLift->LiftPosTrans(m_xStartBeforeLift));
		return;
	}
	else if(desStartPos.jgber_cal_style==2)
	{	//����ֱ�Ӵ���CSuperSmartPtr���͵�pDatumLinePart,����Ϊ��ʱ�ᷢ����Чָ���쳣 wjh-2016.3.26
		CLDSLinePart* pDatumRod=NULL;
		pDatumLinePart=pDatumRod=GetStartDatumPart();
		IntelliUpdateSimplePosParam(TRUE,pDatumRod);
	}
	if(pStart==NULL)	//�Ҳ���ʼ�ڵ�
	{
		if(desStartPos.spatialOperationStyle==0)
		{	//��ָ����׼��ʱ���ļ��в��洢datumPoint wjh-2012.7.24
			desStartPos.datumPoint.UpdatePos(BelongModel());
			m_xStartBeforeLift=desStartPos.datumPoint.Position();
			if( pLift&&(desStartPos.datumPoint.datum_pos_style==0||	//ֱ��ָ������
				desStartPos.datumPoint.datum_pos_style==5||			//��׼�����������겻���
				desStartPos.datumPoint.datum_pos_style==6))			//��׼�����������겻���
				desStartPos.datumPoint.UpdatePos(BelongModel(),pLift);
			//���ݻ���->����ʸ��ƫ�ƣ������Ǹ�ʼ������
			double a,b;
			if(desStartPos.wing_x_offset.gStyle==0)
				a = xWingYZhunJu.g;
			else if(desStartPos.wing_x_offset.gStyle==1)
				a = xWingYZhunJu.g1;
			else if(desStartPos.wing_x_offset.gStyle==2)
				a = xWingYZhunJu.g2;
			else if(desStartPos.wing_x_offset.gStyle==3)
				a = xWingYZhunJu.g3;
			else //if(desStartPos.wing_x_offset.gStyle==4)
				a = desStartPos.wing_x_offset.offsetDist;
			if(desStartPos.wing_y_offset.gStyle==0)
				b = xWingXZhunJu.g;
			else if(desStartPos.wing_y_offset.gStyle==1)
				b = xWingXZhunJu.g1;
			else if(desStartPos.wing_y_offset.gStyle==2)
				b = xWingXZhunJu.g2;
			else if(desStartPos.wing_y_offset.gStyle==3)
				b = xWingXZhunJu.g3;
			else //if(desStartPos.wing_y_offset.gStyle==4)
				b = desStartPos.wing_y_offset.offsetDist;
			GECS cs;
			cs.origin=desStartPos.datumPoint.Position();
			cs.axis_z=End()-Start();
			normalize(cs.axis_z);
			double b1=0,b2=b;
			int iWorkWingX0Y1Both2=GetShortAngleWorkWing(this);
			if(iWorkWingX0Y1Both2==1)
			{	//Y֫Ϊ��λ��׼֫
				cs.axis_x=-norm_y_wing;
				cs.axis_y=cs.axis_z^cs.axis_x;
			}
			else
			{	//X֫Ϊ��λ��׼֫��˫֫��ͬ��λ
				cs.axis_y=-norm_x_wing;
				cs.axis_x=cs.axis_y^cs.axis_z;
				if(iWorkWingX0Y1Both2!=0)
				{
					GEPOINT vLocalWingNormY=cs.TransVToCS(norm_y_wing);
					GEPOINT vLocalWingVecY=GEPOINT(vLocalWingNormY.y,-vLocalWingNormY.x);
					if(fabs(vLocalWingVecY.y)>EPS)
					{
						b1=a*vLocalWingVecY.x/vLocalWingVecY.y;
						b2=b/vLocalWingVecY.y;
					}
				}
			}
			GEPOINT datum_pos=cs.origin-cs.axis_y*a-cs.axis_x*(b1+b2);
			SetStart(datum_pos);
		}
		return;
	}
	f3dPoint pos,init_pos,datum_pos,final_pos;
	//��һ�� ��ʼ��λ�ü���
	if(!GetStartInitPos(init_pos))
		return;
	if(!getjgzj(jgzj,size_wide))
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "The specifications %.0fx%.0f of angle(0X%X) can't find in stadia library,failed calculate position!",size_wide,size_thick,handle);
#else
			log2file->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"%.0fx%.0f���ĽǸ�(0X%X)��׼������Ҳ���,λ�ü�����ܲ�׼ȷ!",size_wide,size_thick,handle);
#endif
		//return;//Ŀǰgetjgzj�϶��з���ֵ���ʲ�Ӧ����׼������޸�׼����˳�λ�ü��� wjh-2016.3.17
	}
	SetModified();
	
	//�ڶ��� ʵʼ��->����Ŀռ����㶨λ
	if(pDatumLinePart.IsNULL())
		pDatumLinePart=GetStartDatumPart();
	f3dPoint lenStdVec=End()-Start();
	if(pDatumLinePart.IsHasPtr()&&desStartPos.spatialOperationStyle<=14)
		lenStdVec=pDatumLinePart->End()-pDatumLinePart->Start();	//��Ӷ�λʱӦ�Դ�������Ǹֵ�����Ϊƽ����Ϸ����жϻ�׼
	if(desStartPos.IsFaceOffset())
		normalize(lenStdVec);
	if(desStartPos.spatialOperationStyle==0)
	{	//ָ����׼��
		desStartPos.datumPoint.UpdatePos(BelongModel(),pLift);
		datum_pos=desStartPos.datumPoint.Position();
	}
	else if(desStartPos.spatialOperationStyle<15)
	{
		if(pDatumLinePart.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0X%X angle Can't find the datum angle which lap with start!",handle);
#else
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0X%X�Ǹ��Ҳ���ʼ�˴��λ�û�׼�Ǹ�!",handle);
#endif
			return;
		}
		else if(pDatumLinePart.IsEqualPtr(this))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"Datum point positions datum angle,angle itself,the design parameter of position is wrong,please redesign 0X%X angle's start position!",handle);
#else
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"���㶨λ��׼�Ǹּ��Ǹֱ���,λ����Ʋ�������,���������0X%X�Ǹ�ʼ��λ��!",handle);
#endif
			return;
		}
		//�����������������ѭ������֪�������ں��ֿ��ǲ�������˼�������ʶ wjh 2010-10-18
		//pFatherLinePart->ClearFlag();
		pDatumLinePart->CalPosition();	//���Ǹֵ�λ
		if(desStartPos.spatialOperationStyle<7)
		{	//Ԥ��׼��ͶӰ��ƽ��
			if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
				SnapPerp(&datum_pos,pDatumLinePart->Start(),pDatumLinePart->End(),pStart->Position(true));
			else if(pDatumLinePart->IsAngle())
			{
				if(desStartPos.spatialOperationStyle==1)	//�Ը��Ǹ�����Ϊ��׼
				{
					//������´������Ľ��������Ķ˵�����ͶӰʱ������ֱ��
					//����GetDatumPosBer����Ϊ��ʱ���迼�ǹ̶�����ͷӰ��
					if(pStart==pDatumLinePart->pStart)
						datum_pos=pDatumLinePart->Start();
					else if(pStart==pDatumLinePart->pEnd)
						datum_pos=pDatumLinePart->End();
					else
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBer(pStart);
					if(pDatumLinePart.LineAnglePointer()->IsReservedHeJiao())
					{	//�����б���֫��ȵ�����Ͻ�
						f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
						f3dPoint vec=End()-Start();
						f3dPoint work_norm=vec^len_offset_vec;
						normalize(work_norm);
						if( fabs(work_norm*pDatumLinePart.LineAnglePointer()->get_norm_x_wing())>
							fabs(work_norm*pDatumLinePart.LineAnglePointer()->get_norm_y_wing()))
							datum_pos+=pDatumLinePart.LineAnglePointer()->CalWingOffsetVec('X',0);
						else
							datum_pos+=pDatumLinePart.LineAnglePointer()->CalWingOffsetVec('Y',0);
					}
				}
				else if (desStartPos.spatialOperationStyle==2)	//�Ը��Ǹ�X֧Ϊ��׼
				{
					double dfSpecG=desStartPos.datumWingOffset.gStyle!=4?0:desStartPos.datumWingOffset.offsetDist;
					datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingX(pStart,dfSpecG);
				}
				else if (desStartPos.spatialOperationStyle==3)	//�Ը��Ǹ�Y֧Ϊ��׼
				{
					double dfSpecG=desStartPos.datumWingOffset.gStyle!=4?0:desStartPos.datumWingOffset.offsetDist;
					datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingY(pStart,dfSpecG);
				}
				else if(desStartPos.spatialOperationStyle==4)	//�Ը��Ǹ�����ƽ��Ϊ��׼
				{
					char cDatumWing=0;
					if(pDatumLinePart.LineAnglePointer()->IsReservedHeJiao())
					{	//�����б���֫��ȵ�����Ͻ�
						f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
						f3dPoint vec=End()-Start();
						f3dPoint work_norm=vec^len_offset_vec;
						normalize(work_norm);
						if( fabs(work_norm*pDatumLinePart.LineAnglePointer()->get_norm_x_wing())>
							fabs(work_norm*pDatumLinePart.LineAnglePointer()->get_norm_y_wing()))
							cDatumWing='X';
						else
							cDatumWing='Y';
					}
					f3dPoint face_norm=desStartPos.face_offset_norm;
					if(pLift&&BelongModel()->RepositionAlgor()<=2)
					{	//ʵ����������ʱ����Բ��Ԥ����ƽ���淨�ߵ�Ӱ������Ϲ�����֪ʵ����������ܵ������߽��㲻�Կ� wjh-2017.10.11
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					
					if(fabs(lenStdVec*desStartPos.face_offset_norm)<0.25)	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
					{
						if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
							log2file->Log("The offset defination of angle 0X%X start is error or rod's extend direction is too close,planar loacation's calculation failed!",handle);
#else 
							log2file->Log("0X%X�Ǹ�ʼ��ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
#endif
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBer(pStart);
					}
					else	//��һ�д����������С�wjh-2012.4.28
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,face_norm,&init_pos,cDatumWing);
					/*if(desStartPos.bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,desStartPos.face_offset_norm,&pt,cDatumWing);
					}
					else
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,desStartPos.face_offset_norm,NULL,cDatumWing);*/
				}
				else if(desStartPos.spatialOperationStyle==5)	//�Ը��Ǹ�X֫Ϊ��׼
				{
					f3dPoint face_norm=desStartPos.face_offset_norm;
					if(pLift&&BelongModel()->RepositionAlgor()<=2)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					double dfSpecG=desStartPos.datumWingOffset.gStyle!=4?0:desStartPos.datumWingOffset.offsetDist;
					if(fabs(lenStdVec*desStartPos.face_offset_norm)<0.25)	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
					{
						if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
							log2file->Log("The offset defination of angle 0X%X start is error or rod's extend direction is too close,planar loacation's calculation failed!",handle);
#else 
							log2file->Log("0X%X�Ǹ�ʼ��ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
#endif
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingX(pStart,dfSpecG);
					}
					else	//��һ�д����������С�wjh-2012.4.28
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,face_norm,&init_pos,dfSpecG);
					/*if(desStartPos.bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm,&pt);
					}
					else
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm);*/
				}
				else //if(desStartPos.spatialOperationStyle==6)	//�Ը��Ǹ�Y֫Ϊ��׼
				{
					f3dPoint face_norm=desStartPos.face_offset_norm;
					if(pLift&&BelongModel()->RepositionAlgor()<=2)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					double dfSpecG=desStartPos.datumWingOffset.gStyle!=4?0:desStartPos.datumWingOffset.offsetDist;
					if(fabs(lenStdVec*desStartPos.face_offset_norm)<0.25)	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
					{
						if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
							log2file->Log("The offset defination of angle 0X%X start is error or rod's extend direction is too close,planar loacation's calculation failed!",handle);
#else 
							log2file->Log("0X%X�Ǹ�ʼ��ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
#endif
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingY(pStart,dfSpecG);
					}
					else	//��һ�д����������С�wjh-2012.4.28
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,face_norm,&init_pos,dfSpecG);
					/*if(desStartPos.bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm,&pt);
					}
					else
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm);*/
				}
			}
			//����Ǹ�ʼ�˴�ӻ�׼�˼�(�Ǹֻ�ֹ�)�ĳ��ȷ��򣬿��ǻ������췽��ƫ���� wht 11-05-19
			f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
			normalize(len_offset_vec);
			double actual_len_offset=desStartPos.len_offset_dist;
			if(desStartPos.len_offset_type==1)//����ƽ����,�ص�ǰ�˼���ֱ����ƽ��ƫ��
			{
				f3dPoint curvec=End()-Start();
				f3dPoint norm=curvec^len_offset_vec;
				f3dPoint offsetvec=norm^curvec;
				if(normalize(offsetvec))
					actual_len_offset/=fabs(offsetvec*len_offset_vec);
				else if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("The datum point on 0X%X angle's start position failed calculates parallel offsetting, automatic change to offset by datum rod's length extending direction",handle);
#else
					log2file->Log("0X%X�Ǹֵ�ʼ��λ�û���ƽ��ƫ�Ƽ���ʧ��,�Զ�����Ϊ���ݻ�׼�˼��������췽�����ƫ��",handle);
#endif
			}
			else if(desStartPos.len_offset_type=='X')//�ش�ӻ�׼�˼�,ƫ��ָ��X�������
			{
				if(fabs(len_offset_vec.x)>0.01)
					actual_len_offset/=len_offset_vec.x;
				else if(log2file){
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Extending direction of joint rod on 0X%X angle's start vertical with X axis,can't specify X component along rod offset",handle);
#else
					log2file->Log("0X%X�Ǹֵ�ʼ�˴�Ӹ˼����췽����X�ᴹֱ�޷��ظ˼�ƫ��ָ��X�������",handle);
#endif
					actual_len_offset=0;
				}
			}
			else if(desStartPos.len_offset_type=='Y')//�ش�ӻ�׼�˼�,ƫ��ָ��Y�������
			{
				if(fabs(len_offset_vec.y)>0.01)
					actual_len_offset/=len_offset_vec.y;
				else if(log2file){
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Extending direction of joint rod on 0X%X angle's start vertical with Y axis,can't specify Y component along rod offset",handle);
#else
					log2file->Log("0X%X�Ǹֵ�ʼ�˴�Ӹ˼����췽����Y�ᴹֱ�޷��ظ˼�ƫ��ָ��Y�������",handle);
#endif
					actual_len_offset=0;
				}
			}
			else if(desStartPos.len_offset_type=='Z')//�ش�ӻ�׼�˼�,ƫ��ָ��Z�������
			{
				if(fabs(len_offset_vec.z)>0.01)
					actual_len_offset/=len_offset_vec.z;
				else if(log2file){
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Extending direction of joint rod on 0X%X angle's start vertical with Z axis,can't specify Z component along rod offset",handle);
#else
					log2file->Log("0X%X�Ǹֵ�ʼ�˴�Ӹ˼����췽����Z�ᴹֱ�޷��ظ˼�ƫ��ָ��Z�������",handle);
#endif
					actual_len_offset=0;
				}
			}
			//�ش�ӻ�׼�˼����ȷ���ƫ�Ƶ�Ч���ƫ����
			datum_pos+=len_offset_vec*actual_len_offset;
		}
		else //if(desStartPos.datum_pos_style<15)
		{	//���ǻ�����1������/2ƫ��׼�ߣ�
			f3dPoint norm;
			CSmartPtr<CLDSLineAngle>pHuoQuJg;
			if(desStartPos.huoqu_jg_h>0x20)
				pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(desStartPos.huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pHuoQuJg.IsNULL()&&pDatumLinePart->IsAngle())
				pHuoQuJg=pDatumLinePart.LineAnglePointer();
			int huoqu_x_wing0_y_wing1;
			if(desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2==1)
				huoqu_x_wing0_y_wing1 = 0;
			else if(desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2==2)
				huoqu_x_wing0_y_wing1 = 1;
			else if(pDatumLinePart==pHuoQuJg&&desStartPos.IsInDatumJgWingX())
				huoqu_x_wing0_y_wing1 = 0;
			else if(pDatumLinePart==pHuoQuJg&&desStartPos.IsInDatumJgWingY())
				huoqu_x_wing0_y_wing1 = 1;
			else if(desStartPos.IsInDatumJgWingX())
			{
				norm = pDatumLinePart.LineAnglePointer()->get_norm_x_wing();
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			else if(desStartPos.IsInDatumJgWingY())
			{
				norm = pDatumLinePart.LineAnglePointer()->get_norm_y_wing();
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			else
			{
				f3dPoint huoqu_vec,jg_vec;
				huoqu_vec = pHuoQuJg->pEnd->GetInverseNodePos(pHuoQuJg)-pHuoQuJg->pStart->GetInverseNodePos(pHuoQuJg);
				jg_vec = pEnd->GetInverseNodePos(this)-pStart->GetInverseNodePos(this);
				norm = huoqu_vec^jg_vec;
				normalize(norm);
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}

			BOOL bBerHuoQu = IsBerHuoQuJg(pHuoQuJg,End()-Start(),huoqu_x_wing0_y_wing1);
			f3dPoint huoqu_wing_vec,datum_face_norm,spread_face_norm;
			f3dLine huoqu_line,ray_line;
			ray_line.startPt = GetStartDatumPos();
			//���¼��и�Ϊֱ���ɽǸ����߷���ȷ�����߷��򣬵�û�о���ʵ�����飬��֪�Բ���
			//��Ҫ���� WJH-2005.03.07
			//Ϊ�˸������߲�����һ������ WJH-2004.05.31
			//if(pEnd->hFatherPart==handle)//&&pOldDesEndPos->offset_style!=1)
			//{	//�Ǹֶ˴��ڻ�׼λ��,��֪���ڻ����÷� WJH-2005.03.03
				ray_line.endPt=End()-Start()+ray_line.startPt;
			//}
			//else
			//	ray_line.endPt = GetEndDatumPos();
			if(huoqu_x_wing0_y_wing1==0)
			{
				huoqu_wing_vec = pHuoQuJg->GetWingVecX();
				//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
				ray_line.startPt+= pHuoQuJg->get_norm_x_wing()*desStartPos.datum_offset_dist;
			}
			else
			{
				huoqu_wing_vec = pHuoQuJg->GetWingVecY();
				//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
				ray_line.startPt+= pHuoQuJg->get_norm_y_wing()*desStartPos.datum_offset_dist;
			}

			huoqu_line.startPt = pHuoQuJg->Start();
			huoqu_line.endPt   = pHuoQuJg->End();
			if(bBerHuoQu)
			{
				huoqu_line.startPt = huoqu_line.startPt-huoqu_wing_vec*desStartPos.huoqu_dist;
				huoqu_line.endPt = huoqu_line.endPt-huoqu_wing_vec*desStartPos.huoqu_dist;
			}
			else
			{
				huoqu_line.startPt = huoqu_line.startPt+
					huoqu_wing_vec*(pHuoQuJg->GetWidth()+desStartPos.huoqu_dist);
				huoqu_line.endPt = huoqu_line.endPt+
					huoqu_wing_vec*(pHuoQuJg->GetWidth()+desStartPos.huoqu_dist);
			}
			//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
			if(huoqu_x_wing0_y_wing1==0)
			{
				huoqu_line.startPt+= pHuoQuJg->get_norm_x_wing()*desStartPos.datum_offset_dist;
				huoqu_line.endPt+= pHuoQuJg->get_norm_x_wing()*desStartPos.datum_offset_dist;
			}
			else
			{
				huoqu_line.startPt+= pHuoQuJg->get_norm_y_wing()*desStartPos.datum_offset_dist;
				huoqu_line.endPt+= pHuoQuJg->get_norm_y_wing()*desStartPos.datum_offset_dist;
			}
			f3dPoint huoqu_vec = huoqu_line.endPt-huoqu_line.startPt;
			f3dPoint ray_vec = ray_line.endPt-ray_line.startPt;
			spread_face_norm = huoqu_vec^ray_vec;
			if(desStartPos.IsInHuoQuLine())	//���ڻ�����׼�Ǹֻ�����
			{
				if(huoqu_x_wing0_y_wing1==0)	//������׼�Ǹ�X֫Ϊ��׼��
					datum_face_norm = pHuoQuJg->get_norm_x_wing();
				else
					datum_face_norm = pHuoQuJg->get_norm_y_wing();

				ray_line.endPt = SpreadFacePntToDatumFace(datum_face_norm,huoqu_line,
					spread_face_norm,ray_line.endPt);
				UCS_STRU inters_ucs;
				inters_ucs.axis_z = datum_face_norm.normalized();
				inters_ucs.axis_y = huoqu_vec.normalized();
				inters_ucs.axis_x = inters_ucs.axis_y^inters_ucs.axis_z;
				normalize(inters_ucs.axis_x);
				inters_ucs.axis_z = inters_ucs.axis_x^inters_ucs.axis_y;
				inters_ucs.origin = pDatumLinePart.LineAnglePointer()->GetDatumPosBer(pStart);
				coord_trans(huoqu_line.startPt,inters_ucs,FALSE);
				coord_trans(huoqu_line.endPt,inters_ucs,FALSE);
				coord_trans(ray_line.startPt,inters_ucs,FALSE);
				coord_trans(ray_line.endPt,inters_ucs,FALSE);
				//�޸�Ϊ�Ի�����Ϊ׼������Z����Ϊ��(�Դ����׼֫����ƫ��) WJH-2003.12.23
				ray_line.startPt.z = ray_line.endPt.z = huoqu_line.startPt.z = huoqu_line.endPt.z;
				Int3dpl(huoqu_line,ray_line,datum_pos);
				coord_trans(datum_pos,inters_ucs,TRUE);
			}
			else if(desStartPos.IsInOffsetZhunLine())	//���ڻ�����׼�Ǹ�ƫ��׼��
			{
				f3dPoint spread_vec = spread_face_norm^huoqu_vec;
				if(spread_vec*ray_vec<0)	//����
					spread_vec*=-1;
				normalize(spread_vec);
				if(huoqu_x_wing0_y_wing1==0)	//������׼�Ǹ�X֫Ϊ��׼��
					datum_face_norm = pHuoQuJg->get_norm_x_wing();
				else
					datum_face_norm = pHuoQuJg->get_norm_y_wing();
				
				SnapPerp(&datum_pos,huoqu_line,pHuoQuJg->GetDatumPosBer(pStart));//pStart->GetInverseNodePos()); //modified by wjh 04.11.11
				datum_pos +=spread_vec*desStartPos.offset_zhun;
				//����ڵ�������ӻ�׼�Ǹֳ��ȷ���ƫ�Ƶ�ƫ�Ʒ��� wht 10-12-09
				f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
				normalize(len_offset_vec);
				datum_pos+=len_offset_vec*desStartPos.len_offset_dist;
			}
		}
	}
	else if(desStartPos.spatialOperationStyle==15&&pStart)
	{	//��֫�������
		if(desStartPos.cMainCoordCtrlWing=='X'||desStartPos.cMainCoordCtrlWing=='Y')
		{
			f3dPoint kp_main,kp_other;
			f3dPoint line_vec,main_wing_offset_vec,face_norm,other_wing_offset_vec;
			if(desStartPos.cMainCoordCtrlWing=='X')
			{
				if(desStartPos.ctrlWing.gStyle==4)
					main_wing_offset_vec=desStartPos.ctrlWing.offsetDist*GetWingVecX();
				else //if(desStartPos.ctrlWing.gStyle<4)
					main_wing_offset_vec=GetWingXG(desStartPos.ctrlWing.gStyle)*GetWingVecX();
				if(desStartPos.otherWing.gStyle==4)
					other_wing_offset_vec=desStartPos.otherWing.offsetDist*GetWingVecY();
				else //if(desStartPos.otherWing.gStyle<4)
					other_wing_offset_vec=GetWingYG(desStartPos.otherWing.gStyle)*GetWingVecY();
			}
			else //if(desStartPos.cMainCoordCtrlWing=='Y')
			{
				if(desStartPos.ctrlWing.gStyle==4)
					main_wing_offset_vec=desStartPos.ctrlWing.offsetDist*GetWingVecY();
				else //if(desStartPos.ctrlWing.gStyle<4)
					main_wing_offset_vec=GetWingYG(desStartPos.ctrlWing.gStyle)*GetWingVecY();
				if(desStartPos.otherWing.gStyle==4)
					other_wing_offset_vec=desStartPos.otherWing.offsetDist*GetWingVecX();
				else //if(desStartPos.otherWing.gStyle<4)
					other_wing_offset_vec=GetWingXG(desStartPos.otherWing.gStyle)*GetWingVecX();
			}
			//if(desStartPos.ctrlWing.operStyle!=0||desStartPos.otherWing.operStyle!=0)
			{	//��ƽ�ƹؼ���ʱ�����˼���(��)�ߵ����췽��
				line_vec=End()-Start();
				normalize(line_vec);
			}
			kp_main=Start()+main_wing_offset_vec;
			f3dPoint kp_ber=Start();
#ifndef __ALFA_TEST_VERSION_
			BOOL gbLegacyMode=FALSE;
			BOOL gbLegacyAngleCtrlWingModeForTMAV5_1_2=FALSE;
#endif
			//���㷨ģʽĬ������֫Ϊ����ͶӰ����ͨ����ϸУ�˳��ж����λֵ���������֫��ʽ��Ӱ�������) wjh-2016.6.04
			if(gbLegacyMode&&desStartPos.ctrlWing.operStyle!=0)
			{
				if(desStartPos.ctrlWing.operStyle==1)
					face_norm.Set(0,0,1);	//X-Yƽ����
				else if(desStartPos.ctrlWing.operStyle==2)
					face_norm.Set(1,0,0);	//Y-Zƽ����
				else if(desStartPos.ctrlWing.operStyle==3)
					face_norm.Set(0,1,0);	//X-Zƽ����
				if(pLift)
					face_norm=pLift->LiftVecTrans(face_norm,init_pos);
				Int3dlf(kp_main,kp_main,line_vec,init_pos,face_norm);
				//���ص�Ϊƽ�Ƶ�ʱ,Ӧ��ƽ�Ƶ㷴�����������,�������������ʼֵ����ʱ,���޷����� wjh-2012.5.1
				SnapPerp(&kp_ber,Start(),End(),kp_main);
			}
			kp_other=Start()+other_wing_offset_vec;
			//���㷨ģʽ����֫ѡ��'����ͶӰ'ʱ����Ҫ���Ƕ����λֵ����������Ҫ�������ص� wjh-2016.6.04
			if(!gbLegacyMode||desStartPos.otherWing.operStyle!=0||desStartPos.ctrlWing.operStyle!=0)
			{	//ֻҪ����֫�򸨿�֫��һ֫Ϊƽ��,Ϊ��֤��������λ��ͬһ�Ǹֽ���ԭ��,�������ƽ�Ƽ���
				if(desStartPos.otherWing.operStyle==0)
					face_norm=line_vec;
				else if(desStartPos.otherWing.operStyle==1)
					face_norm.Set(0,0,1);	//X-Yƽ����
				else if(desStartPos.otherWing.operStyle==2)
					face_norm.Set(1,0,0);	//Y-Zƽ����
				else if(desStartPos.otherWing.operStyle==3)
					face_norm.Set(0,1,0);	//X-Zƽ����
				if(pLift)
					face_norm=pLift->LiftVecTrans(face_norm,init_pos);
				GEPOINT face_pick=init_pos;
				if(gbLegacyMode)
					Int3dlf(kp_other,kp_other,line_vec,init_pos,face_norm);
				else if(gbLegacyAngleCtrlWingModeForTMAV5_1_2)
				{
					if(desStartPos.ctrlWing.operStyle!=0&&desStartPos.otherWing.operStyle==0&&!gbLegacyMode)
						face_pick=kp_main-line_vec*desStartPos.fLenOffsetOfCoordWing;
					Int3dlf(kp_other,kp_other,line_vec,face_pick,face_norm);
					if(desStartPos.ctrlWing.operStyle==0&&desStartPos.otherWing.operStyle!=0&&!gbLegacyMode)
					{
						face_pick=kp_other-line_vec*desStartPos.fLenOffsetOfCoordWing;
						Int3dlf(kp_main,kp_main,line_vec,face_pick,face_norm);
					}
				}
				else
				{
					if(desStartPos.otherWing.operStyle==0)
						face_pick=kp_main-line_vec*desStartPos.fLenOffsetOfCoordWing;
					Int3dlf(kp_other,kp_other,line_vec,face_pick,face_norm);
				}
			}
			f3dPoint other_main=kp_main-kp_other;
			f3dPoint restore_vec=kp_ber-kp_main;	//�˴�������������Ķ����,�����޷���������ʼֵ��������
			if(desStartPos.coordCtrlType==1)		//Y-Z����
				kp_main=init_pos+f3dPoint(other_main.x,0,0);
			else if(desStartPos.coordCtrlType==2)	//X-Y����
				kp_main=init_pos+f3dPoint(0,0,other_main.z);
			else //if(desStartPos.coordCtrlType==0)	//X-Z����
				kp_main=init_pos+f3dPoint(0,other_main.y,0);
			final_pos=kp_main+restore_vec;
		}
		else	//����ʼ�����
			final_pos=datum_pos=init_pos;
	}
	else if(desStartPos.spatialOperationStyle==16||desStartPos.spatialOperationStyle==17)
	{	//��ָ����ͶӰ��ƽ��
		CLDSPoint* pDatumPoint=NULL;
		CLDSLine* pDatumLine=desStartPos.hDatumLine>0X20 ? BelongModel()->FromLineHandle(desStartPos.hDatumLine) : NULL;
		if(pDatumLine)
		{
			if (pDatumLine->hPointArr[0].bSimpleNodePoint)
			{	//��ʱ�趨��׼�ߵ���ʼ�㶨�����
				desStartPos.datumLineStart.datum_pos_style=8;
				desStartPos.datumLineStart.des_para.hNode=pDatumLine->hPointArr[0];
				desStartPos.datumLineStart.UpdatePos(BelongModel(),pLift);
			}
			else if (pDatumLine->hPointArr[0].bRodTipStart||pDatumLine->hPointArr[0].bRodTipEnd)
			{	//�˼�ʼĩ�˵�Ϊ��׼����ʼ��
				desStartPos.datumLineStart.datum_pos_style=1;
				desStartPos.datumLineStart.des_para.RODEND.hRod=pDatumLine->hPointArr[0];
				desStartPos.datumLineStart.des_para.RODEND.direction=pDatumLine->hPointArr[0].bRodTipStart?0:1;
				desStartPos.datumLineStart.des_para.RODEND.SetWingOffset(0);
				desStartPos.datumLineStart.des_para.RODEND.norm_offset[0]=0;
				desStartPos.datumLineStart.UpdatePos(BelongModel(),pLift);
			}
			else //if (pDatumLine->hPointArr[0].ciHidType==0)
			{
				pDatumPoint=BelongModel()->FromPointHandle(pDatumLine->hPointArr[0]);
				if (pDatumPoint!=NULL)
				{
					pDatumPoint->UpdatePos(BelongModel(),pLift);
					desStartPos.datumLineStart=*pDatumPoint;
				}
				else
					logerr.Log("0x%X�Ǹ�ʼ��ͶӰ(ƽ��)ָ���ߵ�ʼ�˵㶨������!",handle);
			}
			if (pDatumLine->hPointArr[1].bSimpleNodePoint)
			{	//��ʱ�趨��׼�ߵ���ʼ�㶨�����
				desStartPos.datumLineEnd.datum_pos_style=8;
				desStartPos.datumLineEnd.des_para.hNode=pDatumLine->hPointArr[1];
				desStartPos.datumLineEnd.UpdatePos(BelongModel(),pLift);
			}
			else if (pDatumLine->hPointArr[1].bRodTipStart||pDatumLine->hPointArr[1].bRodTipEnd)
			{	//�˼�ʼĩ�˵�Ϊ��׼����ʼ��
				desStartPos.datumLineEnd.datum_pos_style=1;
				desStartPos.datumLineEnd.des_para.RODEND.hRod=pDatumLine->hPointArr[1];
				desStartPos.datumLineEnd.des_para.RODEND.direction=pDatumLine->hPointArr[1].bRodTipStart?0:1;
				desStartPos.datumLineEnd.des_para.RODEND.SetWingOffset(0);
				desStartPos.datumLineEnd.des_para.RODEND.norm_offset[0]=0;
				desStartPos.datumLineEnd.UpdatePos(BelongModel(),pLift);
			}
			else //if (pDatumLine->hPointArr[1].ciHidType==0)
			{
				pDatumPoint=BelongModel()->FromPointHandle(pDatumLine->hPointArr[1]);
				if (pDatumPoint!=NULL)
				{
					pDatumPoint->UpdatePos(BelongModel(),pLift);
					desStartPos.datumLineEnd=*pDatumPoint;
				}
				else
					logerr.Log("0x%X�Ǹ�ʼ��ͶӰ(ƽ��)ָ���ߵ��ն˵㶨������!",handle);
			}
		}
		else
		{
			desStartPos.datumLineStart.UpdatePos(BelongModel(),pLift);
			desStartPos.datumLineEnd.UpdatePos(BelongModel(),pLift);
		}
		f3dPoint start=desStartPos.datumLineStart.Position();
		f3dPoint end=desStartPos.datumLineEnd.Position();
		if(desStartPos.datumLineStart.datum_pos_style==0&&pLift)
			start=pLift->LiftPosTrans(start);
		if(desStartPos.datumLineEnd.datum_pos_style==0&&pLift)
			end=pLift->LiftPosTrans(end);
		//Ϊ�˱������ɴ�����άʵ��ģ��ʱ����趨��������λ�ã����ӻ�׼�߲��նԳơ�wjh-2015.12.04
		if(desStartPos.hDatumLine>0X20&&toupper(desStartPos.cMirTransType)=='X')
		{
			start.y*=-1.0;
			end.y*=-1.0;
		}
		else if(desStartPos.hDatumLine>0X20&&toupper(desStartPos.cMirTransType)=='Y')
		{
			start.x*=-1.0;
			end.x*=-1.0;
		}
		else if(desStartPos.hDatumLine>0X20&&toupper(desStartPos.cMirTransType)=='Z')
		{
			start.x*=-1.0;
			start.y*=-1.0;
			end.x*=-1.0;
			end.y*=-1.0;
		}
		
		/*double alfa=Pi-GetRadWingAngle();
		sina=sin(alfa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		double w_d=(desStartPos.fStartJgLengOffsetY-desStartPos.fStartJgLengOffsetX*cos(alfa))/sina;
		start+=get_norm_x_wing()*desStartPos.fStartJgLengOffsetX-GetWingVecX()*w_d;

		alfa=Pi-GetRadWingAngle();
		sina=sin(alfa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		w_d=(desStartPos.fEndJgLengOffsetY-desStartPos.fEndJgLengOffsetX*cos(alfa))/sina;
		end+=get_norm_x_wing()*desStartPos.fEndJgLengOffsetX-GetWingVecX()*w_d;*/

		final_pos=Start();
		if(pStart&&pStart->m_cPosCalType==4)	//���Ǹֽ��潻��
		{
			pStart->GetIntersZhun(&init_pos);
			SnapPerp(&final_pos,start,end,init_pos);
			SetStart(final_pos);
			return;
		}
		else if(pStart&&pStart->m_cPosCalType==7&&	//��ͨ�ظ˼����ȷ���ƫ�ƵĽڵ�
			(pStart->m_cPosCalViceType!='X'&&pStart->m_cPosCalViceType!='Y'&&pStart->m_cPosCalViceType!='Z'))
		{	//GetDatumPosBer(pDatumNode)�õ��ĵ��λ�ã������ɽڵ���˼�ͶӰ����õ��ģ�����׼�ڵ�ĸ��˼���λ��ʽΪƽ�ƣ�
			//�����׼�ڵ㸸�˼������˼���(����ƫ�ƽڵ��)һ�ˣ���λ��ʽ�޸�Ϊƽ�Ʋ�������  wht 09-07-17	
			CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(pStart->arrRelationNode[0]);
			//ƫ�ƽڵ��׼�ڵ㸸�˼�Ϊ��ǰ�˼�ʱ��(pDatumNode->hFatherPart==handle)
			//���ݻ�׼�ڵ��Լ�ƫ�ƾ��������ƫ�ƽڵ�Ϊ�˵�ĸ˼���ʼ��λ�� wht 10-10-19
			CLDSLineAngle *pDatumFatherJg=GetStartNodeOffsetDatumAngle();
			if(pDatumFatherJg)
			{	//�����׼�ڵ㸸�˼���λ��ʽΪƽ��ʱ����GetDatumPosBerByFaceOffset() wht 10-07-30
				f3dPoint pos=pDatumFatherJg->GetDatumPosBer(pDatumNode);
				f3dPoint face_norm;
				if(pDatumFatherJg->desStartPos.IsFaceOffset())//||
					face_norm=pDatumFatherJg->desStartPos.face_offset_norm;
				else if(pDatumFatherJg->desEndPos.IsFaceOffset())
					face_norm=pDatumFatherJg->desEndPos.face_offset_norm;
				if(fabs(lenStdVec*face_norm)>0.25)	//��׼�ڵ㶨λ��ʽΪƽ��
				{	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
					if(pLift)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					pos=pDatumFatherJg->GetDatumPosBerByFaceOffset(pDatumNode,face_norm,&pos);	
				}
				else if(desStartPos.IsFaceOffset())
				{
		#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("The offset defination of angle 0X%X start is error or rod's extend direction is too close,planar loacation's calculation failed!",handle);
		#else 
					Log2File()->Log("0X%X�Ǹ�ʼ��ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
		#endif
				}
				f3dPoint line_vec=pDatumFatherJg->End()-pDatumFatherJg->Start();
				normalize(line_vec);
				CLDSNode* pAttachNode1=BelongModel()->FromNodeHandle(pStart->arrRelationNode[1]);
				CLDSNode* pAttachNode2=BelongModel()->FromNodeHandle(pStart->arrRelationNode[2]);
				if(pAttachNode1&&pAttachNode2&&(line_vec*(pAttachNode2->Position()-pAttachNode1->Position())<0))
					line_vec*=-1.0;
				//������ƫ����ͶӰ�������޷�ͬʱ��֤ƫ������ͶӰֵ
				final_pos=pos+line_vec*pStart->attach_offset;
				SnapPerp(&final_pos,start,end,final_pos);
				SetStart(final_pos);
				return;
			}
		}
		else
		{
			if(desStartPos.spatialOperationStyle==16)	//ͶӰ
				SnapPerp(&final_pos,start,end,init_pos);
			else //if(desStartPos.spatialOperationStyle=17)	//ƽ��
			{
				f3dPoint face_norm=desStartPos.face_offset_norm;
				if(pLift)
				{
					bool bInRightSide=true;
					if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
						bInRightSide=false;
					else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
						bInRightSide=false;
					face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
				}
				Int3dlf(final_pos,f3dLine(start,end),init_pos,face_norm);
				if(desStartPos.cFaceOffsetDatumLine=='X'||desStartPos.cFaceOffsetDatumLine=='Y')
				{	//ƽ�ƻ�׼��Ϊ֫����
					SetStart(final_pos);
					f3dPoint wing_prj_vec,wing_pos;
					f3dPoint line_vec=final_pos-End();
					normalize(line_vec);
					if(desStartPos.cFaceOffsetDatumLine=='X')	//ƽ�ƻ�׼��ΪX֫����
						wing_pos=GetSelfDatumPosWingXByFaceOffset(pStart,face_norm,&init_pos);
					else
						wing_pos=GetSelfDatumPosWingYByFaceOffset(pStart,face_norm,&init_pos);
					SnapPerp(&final_pos,Start(),End(),wing_pos);
				}
			}
			datum_pos=init_pos;
			SetStart(final_pos);
			return;
		}
	}
	else //if(desStartPos.spatialOperationStyle==18||desStartPos.spatialOperationStyle==19)
		datum_pos=init_pos;	//������Ƶ�ͶӰ��ƽ��
	if(pDatumLinePart.IsHasPtr()&&pDatumLinePart->GetClassTypeId()==CLS_LINETUBE&&
		(desStartPos.spatialOperationStyle==1||desStartPos.spatialOperationStyle==4))
	{	//���Ǹִ���ڸֹ���ʱʹ��ƫ�ľ�������Ǹֲ����ڸֹ�������(��������1/4������)����� wht 09-10-17
		f3dLine datumLine(pDatumLinePart->Start(),pDatumLinePart->End());
		f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
		f3dPoint cur_vec=End()-Start();
		f3dPoint plane_norm=datum_vec^cur_vec;
		f3dPoint eccentricOffsetVec=datum_vec^plane_norm;
		normalize(eccentricOffsetVec);
		if(eccentricOffsetVec*cur_vec<EPS)
			eccentricOffsetVec*=-1.0;
		if(fabs(desStartPos.fEccentricDist)>EPS)
		{	//ƫ�ľ�
			if(desStartPos.spatialOperationStyle==1||desStartPos.face_offset_norm.IsZero()) //ͶӰ
				datum_pos+=eccentricOffsetVec*desStartPos.fEccentricDist;
			else //if(desStartPos.spatialOperationStyle==4) //ƽ��
			{	
				datumLine.startPt+=eccentricOffsetVec*desStartPos.fEccentricDist;
				datumLine.endPt+=eccentricOffsetVec*desStartPos.fEccentricDist;
				f3dPoint face_pick=datum_pos;
				f3dPoint face_norm=desStartPos.face_offset_norm;
				if(pLift)
				{
					bool bInRightSide=true;
					if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
						bInRightSide=false;
					else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
						bInRightSide=false;
					face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
				}
				Int3dlf(datum_pos,datumLine,face_pick,face_norm);
			}
		}
	}

	//������ ����->����ʸ��ƫ�ƣ��������ڴ�Ӹ˼���(��)���λ�ü���
	if((desStartPos.spatialOperationStyle>0&&desStartPos.spatialOperationStyle<15)&&//���޴�����������֫�������ʱ��λ�ÿ��ܻ����㡡wjh-2015.3.2
		desStartPos.wing_x_offset.gStyle==4&&fabs(desStartPos.wing_x_offset.offsetDist)<EPS &&
		desStartPos.wing_y_offset.gStyle==4&&fabs(desStartPos.wing_y_offset.offsetDist)<EPS)
		final_pos=datum_pos;
	if(desStartPos.spatialOperationStyle<15||desStartPos.spatialOperationStyle==18)
	{	//��ʽ�ӿ��ƽڵ�ͶӰ����Ǹ�������ͨ��Ӽ���Ǹ����
		f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
		short work_wing_style=desStartPos.cWorkWingStyle;
#ifndef __ALFA_TEST_VERSION_
		BOOL gbLegacyMode=FALSE;
#endif
		if(desStartPos.cWorkWingStyle==0&&pDatumLinePart.IsHasPtr())
		{	//��ʽ�ӿ��ƽڵ�ͶӰ����Ǹ����
			if(pDatumLinePart->handle==handle||desStartPos.spatialOperationStyle==18)
			{	//��ʼ�ڵ㸸�Ǹ�Ϊ��ǰ�Ǹ֣���ʱƫ�Ʒ����ؽǸ���֫����
				if( desStartPos.wing_x_offset.gStyle==4&&desStartPos.wing_x_offset.offsetDist==0&&(
					desStartPos.wing_y_offset.gStyle!=4||desStartPos.wing_y_offset.offsetDist!=0))
					work_wing_style=1;	//X֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��X֫Ϊ��ǰ����֫
				else if(desStartPos.wing_y_offset.gStyle==4&&desStartPos.wing_y_offset.offsetDist==0&&(
						desStartPos.wing_x_offset.gStyle!=4||desStartPos.wing_x_offset.offsetDist!=0))
					work_wing_style=2;	//Y֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��Y֫Ϊ��ǰ����֫
				else
					work_wing_style=3;
			}
			else
			{	//��ӽǸ��ص�ǰ����֫�ķ��߼�֫�������ƫ��
				f3dPoint father_vec,cur_vec,work_norm;
				if(pDatumLinePart.IsHasPtr())
				{
					cur_vec = End()-Start();
					father_vec = pDatumLinePart->End()-pDatumLinePart->Start();
					normalize(cur_vec);
					normalize(father_vec);
					if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
					{
						if(desStartPos.IsInDatumJgWingX())
							work_norm=pDatumLinePart.LineAnglePointer()->get_norm_x_wing();
						else if(desStartPos.IsInDatumJgWingY())
							work_norm=pDatumLinePart.LineAnglePointer()->get_norm_y_wing();
						else if(!gbLegacyMode&&
							(norm_x_wing*pDatumLinePart.LineAnglePointer()->vxWingNorm>EPS_COS2&&norm_y_wing*pDatumLinePart.LineAnglePointer()->vyWingNorm>EPS_COS2)||
							(norm_x_wing*pDatumLinePart.LineAnglePointer()->vyWingNorm>EPS_COS2&&norm_y_wing*pDatumLinePart.LineAnglePointer()->vxWingNorm>EPS_COS2))
							work_wing_style=3;	//�Զ��ж�Ϊ˫֫��λ
						else
							work_norm=father_vec^cur_vec;
					}
					else if(fabs(father_vec*cur_vec)<EPS_COS2)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
						work_norm = father_vec^cur_vec;
				}
				if(work_wing_style!=3&&normalize(work_norm)!=0)
				{
					if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
						work_wing_style=1;
					else
						work_wing_style=2;
				}
				else
					work_wing_style=3;
			}
		}
		else if(desStartPos.cWorkWingStyle==0)
			work_wing_style=3;
		if(work_wing_style<3)
		{	//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
			f3dPoint* pSelfWorkNorm=NULL;
			if(work_wing_style==1)
			{
				offset_vec_x = get_norm_x_wing();
				offset_vec_y = -GetWingVecX();
				pSelfWorkNorm=&offset_vec_x;
			}
			else
			{
				offset_vec_x = -GetWingVecY();
				offset_vec_y = get_norm_y_wing();
				pSelfWorkNorm=&offset_vec_y;
			}
			if( pSelfWorkNorm&&pDatumLinePart.IsHasPtr()&&!gbLegacyMode&&BelongModel()->RepositionAlgor()!=1&&
				!desStartPos.IsInHuoQuLine()&&!desStartPos.IsInOffsetZhunLine()&&
				(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
			{
				f3dPoint vWorkNorm;
				if(desStartPos.IsInDatumJgWingX())
					vWorkNorm=pDatumLinePart.LineAnglePointer()->vxWingNorm;
				else if(desStartPos.IsInDatumJgWingY())
					vWorkNorm=pDatumLinePart.LineAnglePointer()->vyWingNorm;
				double justify=(*pSelfWorkNorm)*vWorkNorm;
				if(justify>EPS_COS2)
					(*pSelfWorkNorm)=vWorkNorm;
				else if(justify<-EPS_COS2)
					(*pSelfWorkNorm)=-vWorkNorm;
			}
			if(desStartPos.wing_x_offset.gStyle==0)
				final_pos=datum_pos+offset_vec_x*xWingYZhunJu.g;
			else if(desStartPos.wing_x_offset.gStyle==1)
				final_pos=datum_pos+offset_vec_x*xWingYZhunJu.g1;
			else if(desStartPos.wing_x_offset.gStyle==2)
				final_pos=datum_pos+offset_vec_x*xWingYZhunJu.g2;
			else if(desStartPos.wing_x_offset.gStyle==3)
				final_pos=datum_pos+offset_vec_x*xWingYZhunJu.g3;
			else //if(desStartPos.wing_x_offset.gStyle==4)
				final_pos=datum_pos+offset_vec_x*desStartPos.wing_x_offset.offsetDist;
			if(desStartPos.wing_y_offset.gStyle==0)
				final_pos+=offset_vec_y*xWingXZhunJu.g;
			else if(desStartPos.wing_y_offset.gStyle==1)
				final_pos+=offset_vec_y*xWingXZhunJu.g1;
			else if(desStartPos.wing_y_offset.gStyle==2)
				final_pos+=offset_vec_y*xWingXZhunJu.g2;
			else if(desStartPos.wing_y_offset.gStyle==3)
				final_pos+=offset_vec_y*xWingXZhunJu.g3;
			else //if(desStartPos.wing_y_offset.gStyle==4)
				final_pos+=offset_vec_y*desStartPos.wing_y_offset.offsetDist;
		}
		else
		{
			double a,b,c,alfa;
			if(desStartPos.wing_x_offset.gStyle==0)
				a = xWingYZhunJu.g;
			else if(desStartPos.wing_x_offset.gStyle==1)
				a = xWingYZhunJu.g1;
			else if(desStartPos.wing_x_offset.gStyle==2)
				a = xWingYZhunJu.g2;
			else if(desStartPos.wing_x_offset.gStyle==3)
				a = xWingYZhunJu.g3;
			else //if(desStartPos.wing_x_offset.gStyle==4)
				a = desStartPos.wing_x_offset.offsetDist;
			if(desStartPos.wing_y_offset.gStyle==0)
				b = xWingXZhunJu.g;
			else if(desStartPos.wing_y_offset.gStyle==1)
				b = xWingXZhunJu.g1;
			else if(desStartPos.wing_y_offset.gStyle==2)
				b = xWingXZhunJu.g2;
			else if(desStartPos.wing_y_offset.gStyle==3)
				b = xWingXZhunJu.g3;
			else //if(desStartPos.wing_y_offset.gStyle==4)
				b = desStartPos.wing_y_offset.offsetDist;
			alfa = GetRadWingAngle();
			/*c = sqrt((b*b-2*a*b*cos(alfa)+a*a*cos(alfa)*cos(alfa))/(sin(alfa)*sin(alfa))+a*a-b*b);
			//if(a>0)
			//start_xyz = start_pos+norm_y_wing*b-GetWingVecY()*c;
			//else
			//start_xyz = start_pos+norm_y_wing*b+GetWingVecY()*c;*/
			sina=sin(alfa);
			if(fabs(sina)<EPS)
				sina=1;		//�Է����ֳ�0
			c = (b-a*cos(alfa))/sina;
			final_pos=datum_pos-GetWingVecY()*a+get_norm_y_wing()*c;
		}
	}
	else if(desStartPos.spatialOperationStyle==19)
	{	//��ʽ�ӿ��ƽڵ�ƽ�Ƽ���Ǹ����
		if(fabs(lenStdVec*desStartPos.face_offset_norm)<0.25)	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("The offset defination of angle 0X%X start is error or rod's extend direction is too close,planar loacation's calculation failed!",handle);
#else 
				log2file->Log("0X%X�Ǹ�ʼ��ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
#endif
			return;
		}
		f3dPoint wing_x_pos=GetStartWingDatumPosByFaceOffset('X',desStartPos.face_offset_norm);
		//f3dPoint wing_x_pos=GetSelfDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm);
		f3dPoint wing_y_pos=GetStartWingDatumPosByFaceOffset('Y',desStartPos.face_offset_norm);
		//f3dPoint wing_y_pos=GetSelfDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm);
		f3dPoint start_pos=GetDatumPosBerByFaceOffset(pStart,desStartPos.face_offset_norm);
		f3dPoint wing_x_prj_vec=wing_x_pos-start_pos;
		f3dPoint wing_y_prj_vec=wing_y_pos-start_pos;
		final_pos=CalOffsetAngleBer(pStart->Position(true),wing_x_prj_vec,wing_y_prj_vec);
		SetStart(final_pos);
		if(desStartPos.cFaceOffsetDatumLine=='X'||desStartPos.cFaceOffsetDatumLine=='Y')
		{	//ƽ�ƻ�׼��Ϊ֫����
			f3dPoint wing_prj_vec,wing_pos;
			f3dPoint line_vec=Start()-End();
			normalize(line_vec);
			if(desStartPos.cFaceOffsetDatumLine=='X')	//ƽ�ƻ�׼��ΪX֫����
				wing_pos=GetSelfDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm);
			else //if(desStartPos.cFaceOffsetDatumLine=='Y')
				wing_pos=GetSelfDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm);
			SnapPerp(&final_pos,Start(),End(),wing_pos);
		}
	}
	SetStart(final_pos);
}
//�¸��ĽǸ����λ�ò������ԭ�����ն�λ�ü��㺯�� wjh-2012-4-28
void CLDSLineAngle::CalEndPosition(CLDSArcLift *pLift/*=NULL*/)
{
	if(BelongModel()==NULL)
		return;
	if(!LockEnd())
		return;	//�ѵ�λ
#ifdef __COMMON_PART_INC_
	if(pLift==NULL&&m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#else
	pLift=NULL;
#endif
	JGZJ jgzj;
	double sina;
	if(IsEndPosLocked())
		return;			//ʼ��λ���ѱ������������ǻ����㣩
	CSuperSmartPtr<CLDSLinePart> pDatumLinePart;
	if(desEndPos.jgber_cal_style==1)	//ֱ��ָ��������꣬����Ҫ����������
	{
		if(pLift)
			baseline.SetEnd(pLift->LiftPosTrans(m_xEndBeforeLift));
		return;
	}
	else if(desEndPos.jgber_cal_style==2)
	{
		CLDSLinePart* pDatumRod=NULL;
		pDatumLinePart=pDatumRod=GetEndDatumPart();
		IntelliUpdateSimplePosParam(FALSE,pDatumRod);
	}
	if(pEnd==NULL)	//�Ҳ����սڵ�
	{
		if(desEndPos.spatialOperationStyle==0)
		{	//��ָ����׼��ʱ���ļ��в��洢datumPoint wjh-2012.7.24
			desEndPos.datumPoint.UpdatePos(BelongModel());
			m_xEndBeforeLift=desEndPos.datumPoint.Position();
			if( pLift&&(desEndPos.datumPoint.datum_pos_style==0||	//ֱ��ָ������
				desEndPos.datumPoint.datum_pos_style==5||			//��׼�����������겻���
				desEndPos.datumPoint.datum_pos_style==6))			//��׼�����������겻���
				desEndPos.datumPoint.UpdatePos(BelongModel(),pLift);
			//���ݻ���->����ʸ��ƫ�ƣ������Ǹ�ʼ������
			double a,b;
			if(desEndPos.wing_x_offset.gStyle==0)
				a = xWingYZhunJu.g;
			else if(desEndPos.wing_x_offset.gStyle==1)
				a = xWingYZhunJu.g1;
			else if(desEndPos.wing_x_offset.gStyle==2)
				a = xWingYZhunJu.g2;
			else if(desEndPos.wing_x_offset.gStyle==3)
				a = xWingYZhunJu.g3;
			else
				a = desEndPos.wing_x_offset.offsetDist;
			if(desEndPos.wing_y_offset.gStyle==0)
				b = xWingXZhunJu.g;
			else if(desEndPos.wing_y_offset.gStyle==1)
				b = xWingXZhunJu.g1;
			else if(desEndPos.wing_y_offset.gStyle==2)
				b = xWingXZhunJu.g2;
			else if(desEndPos.wing_y_offset.gStyle==3)
				b = xWingXZhunJu.g3;
			else
				b = desEndPos.wing_y_offset.offsetDist;
			GECS cs;
			cs.origin=desEndPos.datumPoint.Position();
			cs.axis_z=End()-Start();
			normalize(cs.axis_z);
			double b1=0,b2=b;
			int iWorkWingX0Y1Both2=GetShortAngleWorkWing(this);
			if(iWorkWingX0Y1Both2==1)
			{	//Y֫Ϊ��λ��׼֫
				cs.axis_x=-norm_y_wing;
				cs.axis_y=cs.axis_z^cs.axis_x;
			}
			else
			{	//X֫Ϊ��λ��׼֫��˫֫��ͬ��λ
				cs.axis_y=-norm_x_wing;
				cs.axis_x=cs.axis_y^cs.axis_z;
				if(iWorkWingX0Y1Both2!=0)
				{
					GEPOINT vLocalWingNormY=cs.TransVToCS(norm_y_wing);
					GEPOINT vLocalWingVecY=GEPOINT(vLocalWingNormY.y,-vLocalWingNormY.x);
					if(fabs(vLocalWingVecY.y)>EPS)
					{
						b1=a*vLocalWingVecY.x/vLocalWingVecY.y;
						b2=b/vLocalWingVecY.y;
					}
				}
			}
			GEPOINT datum_pos=cs.origin-cs.axis_y*a-cs.axis_x*(b1+b2);
			SetEnd(datum_pos);
		}
		return;
	}
	f3dPoint pos,init_pos,datum_pos,final_pos;
	//��һ�� ��ʼ��λ�ü���
	if(!GetEndInitPos(init_pos))
		return;
	if(!getjgzj(jgzj,size_wide))
	{
		if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
			log2file->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "The specifications %.0fx%.0f of angle(0X%X) can't find in stadia library,failed calculate position!", size_wide, size_thick, handle);
#else
			log2file->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "%.0fx%.0f���ĽǸ�(0X%X)��׼������Ҳ���,λ�ü�����ܲ�׼ȷ!", size_wide, size_thick, handle);
#endif
		//return;//Ŀǰgetjgzj�϶��з���ֵ���ʲ�Ӧ����׼������޸�׼����˳�λ�ü��� wjh-2016.3.17
	}
	SetModified();
	
	//�ڶ��� ʵʼ��->����Ŀռ����㶨λ
	if(pDatumLinePart.IsNULL())
		pDatumLinePart=GetEndDatumPart();
	f3dPoint lenStdVec=End()-Start();
	if(pDatumLinePart.IsHasPtr()&&desEndPos.spatialOperationStyle<=14)
		lenStdVec=pDatumLinePart->End()-pDatumLinePart->Start();	//��Ӷ�λʱӦ�Դ�������Ǹֵ�����Ϊƽ����Ϸ����жϻ�׼
	if(desEndPos.IsFaceOffset())
		normalize(lenStdVec);
	if(desEndPos.spatialOperationStyle==0)
	{
		desEndPos.datumPoint.UpdatePos(BelongModel(),pLift);
		datum_pos=desEndPos.datumPoint.Position();
	}
	else if(desEndPos.spatialOperationStyle<15)
	{
		if(pDatumLinePart.IsNULL())
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("0X%X Angle can't find the datum angle which lap with end!",handle);
#else
				log2file->Log("0X%X�Ǹ��Ҳ����ն˴��λ�û�׼�Ǹ�!",handle);
#endif
			return;
		}
		else if(pDatumLinePart.IsEqualPtr(this))
		{
			if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
				log2file->Log("Datum point positions datum angle,angle itself,the design parameter of position is wrong,please redesign 0X%X angle's end position!",handle);
#else
				log2file->Log("���㶨λ��׼�Ǹּ��Ǹֱ���,λ����Ʋ�������,���������0X%X�Ǹ��ն�λ��!",handle);
#endif
			return;
		}
		//�����������������ѭ������֪�������ں��ֿ��ǲ�������˼�������ʶ wjh 2010-10-18
		//pFatherLinePart->ClearFlag();
		pDatumLinePart->CalPosition();	//���Ǹֵ�λ
		if(desEndPos.spatialOperationStyle<7)
		{	//Ԥ��׼��ͶӰ��ƽ��
			if(pDatumLinePart->GetClassTypeId()==CLS_LINETUBE)
				SnapPerp(&datum_pos,pDatumLinePart->Start(),pDatumLinePart->End(),pEnd->Position(true));
			else if(pDatumLinePart->IsAngle())
			{
				if(desEndPos.spatialOperationStyle==1)	//�Ը��Ǹ�����Ϊ��׼
				{
					//������´������Ľ��������Ķ˵�����ͶӰʱ������ֱ��
					//����GetDatumPosBer����Ϊ��ʱ���迼�ǹ̶�����ͷӰ��
					if(pEnd==pDatumLinePart->pEnd)
						datum_pos=pDatumLinePart->End();
					else if(pEnd==pDatumLinePart->pStart)
						datum_pos=pDatumLinePart->Start();
					else
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBer(pEnd);
					if(pDatumLinePart.LineAnglePointer()->IsReservedHeJiao())
					{	//�����б���֫��ȵ�����Ͻ�
						f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
						f3dPoint vec=End()-Start();
						f3dPoint work_norm=vec^len_offset_vec;
						normalize(work_norm);
						if( fabs(work_norm*pDatumLinePart.LineAnglePointer()->get_norm_x_wing())>
							fabs(work_norm*pDatumLinePart.LineAnglePointer()->get_norm_y_wing()))
							datum_pos+=pDatumLinePart.LineAnglePointer()->CalWingOffsetVec('X',0);
						else
							datum_pos+=pDatumLinePart.LineAnglePointer()->CalWingOffsetVec('Y',0);
					}
				}
				else if (desEndPos.spatialOperationStyle==2)	//�Ը��Ǹ�X֧Ϊ��׼
				{
					double dfSpecG=desEndPos.datumWingOffset.gStyle!=4?0:desEndPos.datumWingOffset.offsetDist;
					datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingX(pEnd,dfSpecG);
				}
				else if (desEndPos.spatialOperationStyle==3)	//�Ը��Ǹ�Y֧Ϊ��׼
				{
					double dfSpecG=desEndPos.datumWingOffset.gStyle!=4?0:desEndPos.datumWingOffset.offsetDist;
					datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingY(pEnd,dfSpecG);
				}
				else if(desEndPos.spatialOperationStyle==4)	//�Ը��Ǹ�����ƽ��Ϊ��׼
				{
					char cDatumWing=0;
					if(pDatumLinePart.LineAnglePointer()->IsReservedHeJiao())
					{	//�����б���֫��ȵ�����Ͻ�
						f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
						f3dPoint vec=End()-Start();
						f3dPoint work_norm=vec^len_offset_vec;
						normalize(work_norm);
						if( fabs(work_norm*pDatumLinePart.LineAnglePointer()->get_norm_x_wing())>
							fabs(work_norm*pDatumLinePart.LineAnglePointer()->get_norm_y_wing()))
							cDatumWing='X';
						else
							cDatumWing='Y';
					}
					f3dPoint face_norm=desEndPos.face_offset_norm;
					if(pLift&&BelongModel()->RepositionAlgor()<=2)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					if(fabs(lenStdVec*desEndPos.face_offset_norm)<0.25)	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
					{
						if(log2file)
							log2file->Log("0X%X�Ǹ��ն�ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBer(pEnd);
					}
					else	//��һ�д����������С�wjh-2012.4.28
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pEnd,face_norm,&init_pos,cDatumWing);
					/*if(desEndPos.bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,desEndPos.face_offset_norm,&pt,cDatumWing);
					}
					else
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,desEndPos.face_offset_norm,NULL,cDatumWing);*/
				}
				else if(desEndPos.spatialOperationStyle==5)	//�Ը��Ǹ�X֫Ϊ��׼
				{
					f3dPoint face_norm=desEndPos.face_offset_norm;
					if(pLift&&BelongModel()->RepositionAlgor()<=2)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					double dfSpecG=desEndPos.datumWingOffset.gStyle!=4?0:desEndPos.datumWingOffset.offsetDist;
					if(fabs(lenStdVec*desEndPos.face_offset_norm)<0.25)	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
					{
						if(log2file)
							log2file->Log("0X%X�Ǹ��ն�ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingX(pEnd,dfSpecG);
					}
					else	//��һ�д����������С�wjh-2012.4.28
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pEnd,face_norm,&init_pos,dfSpecG);
					/*if(desStartPos.bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm,&pt);
					}
					else
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm);*/
				}
				else //if(desEndPos.spatialOperationStyle==6)	//�Ը��Ǹ�Y֫Ϊ��׼
				{
					f3dPoint face_norm=desEndPos.face_offset_norm;
					if(pLift&&BelongModel()->RepositionAlgor()<=2)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					double dfSpecG=desEndPos.datumWingOffset.gStyle!=4?0:desEndPos.datumWingOffset.offsetDist;
					if(fabs(lenStdVec*desEndPos.face_offset_norm)<0.25)	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
					{
						if(log2file)
							log2file->Log("0X%X�Ǹ��ն�ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingY(pEnd,dfSpecG);
					}
					else	//��һ�д����������С�wjh-2012.4.28
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pEnd,face_norm,&init_pos,dfSpecG);
					/*if(desEndPos.bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm,&pt);
					}
					else
						datum_pos = pDatumLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm);*/
				}
			}
			//����Ǹ�ʼ�˴�ӻ�׼�˼�(�Ǹֻ�ֹ�)�ĳ��ȷ��򣬿��ǻ������췽��ƫ���� wht 11-05-19
			f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
			normalize(len_offset_vec);
			double actual_len_offset=desEndPos.len_offset_dist;
			if(desEndPos.len_offset_type==1)//����ƽ����,�ص�ǰ�˼���ֱ����ƽ��ƫ��
			{
				f3dPoint curvec=End()-Start();
				f3dPoint norm=curvec^len_offset_vec;
				f3dPoint offsetvec=norm^curvec;
				if(normalize(offsetvec))
					actual_len_offset/=fabs(offsetvec*len_offset_vec);
				else if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("The datum point on 0X%X angle's end position failed calculates parallel offsetting, automatic change to offset by datum rod's length extending direction.",handle);
#else
					log2file->Log("0X%X�Ǹֵ��ն�λ�û���ƽ��ƫ�Ƽ���ʧ��,�Զ�����Ϊ���ݻ�׼�˼��������췽�����ƫ��",handle);
#endif
			}
			else if(desEndPos.len_offset_type=='X')//�ش�ӻ�׼�˼�,ƫ��ָ��X�������
			{
				if(fabs(len_offset_vec.x)>0.01)
					actual_len_offset/=len_offset_vec.x;
				else if(log2file){
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Extending direction of joint rod on 0X%X angle's end vertical with X axis,can't specify X component along rod offset",handle);
#else
					log2file->Log("0X%X�Ǹֵ��ն˴�Ӹ˼����췽����X�ᴹֱ�޷��ظ˼�ƫ��ָ��X�������",handle);
#endif
					actual_len_offset=0;
				}
			}
			else if(desEndPos.len_offset_type=='Y')//�ش�ӻ�׼�˼�,ƫ��ָ��Y�������
			{
				if(fabs(len_offset_vec.y)>0.01)
					actual_len_offset/=len_offset_vec.y;
				else if(log2file){
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Extending direction of joint rod on 0X%X angle's end vertical with Y axis,can't specify Y component along rod offset",handle);
#else
					log2file->Log("0X%X�Ǹֵ��ն˴�Ӹ˼����췽����Y�ᴹֱ�޷��ظ˼�ƫ��ָ��Y�������",handle);
#endif
					actual_len_offset=0;
				}
			}
			else if(desEndPos.len_offset_type=='Z')//�ش�ӻ�׼�˼�,ƫ��ָ��Z�������
			{
				if(fabs(len_offset_vec.z)>0.01)
					actual_len_offset/=len_offset_vec.z;
				else if(log2file){
#ifdef AFX_TARG_ENU_ENGLISH
					log2file->Log("Extending direction of joint rod on 0X%X angle's end vertical with Z axis,can't specify Z component along rod offset",handle);
#else
					log2file->Log("0X%X�Ǹֵ��ն˴�Ӹ˼����췽����Z�ᴹֱ�޷��ظ˼�ƫ��ָ��Z�������",handle);
#endif
					actual_len_offset=0;
				}
			}
			//�ش�ӻ�׼�˼����ȷ���ƫ�Ƶ�Ч���ƫ����
			datum_pos+=len_offset_vec*actual_len_offset;
		}
		else //if(desEndPos.datum_pos_style<15)
		{	//���ǻ�����1������/2ƫ��׼�ߣ�
			f3dPoint norm;
			CSmartPtr<CLDSLineAngle>pHuoQuJg;
			if(desEndPos.huoqu_jg_h>0x20)
				pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(desEndPos.huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pHuoQuJg.IsNULL()&&pDatumLinePart->IsAngle())
				pHuoQuJg=pDatumLinePart.LineAnglePointer();
			int huoqu_x_wing0_y_wing1;
			if(desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2==1)
				huoqu_x_wing0_y_wing1 = 0;
			else if(desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2==2)
				huoqu_x_wing0_y_wing1 = 1;
			else if(pDatumLinePart==pHuoQuJg&&desEndPos.IsInDatumJgWingX())
				huoqu_x_wing0_y_wing1 = 0;
			else if(pDatumLinePart==pHuoQuJg&&desEndPos.IsInDatumJgWingY())
				huoqu_x_wing0_y_wing1 = 1;
			else if(desEndPos.IsInDatumJgWingX())
			{
				norm = pDatumLinePart.LineAnglePointer()->get_norm_x_wing();
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			else if(desEndPos.IsInDatumJgWingY())
			{
				norm = pDatumLinePart.LineAnglePointer()->get_norm_y_wing();
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			else
			{
				f3dPoint huoqu_vec,jg_vec;
				huoqu_vec = pHuoQuJg->pEnd->GetInverseNodePos(pHuoQuJg)-pHuoQuJg->pStart->GetInverseNodePos(pHuoQuJg);
				jg_vec = pEnd->GetInverseNodePos(this)-pStart->GetInverseNodePos(this);
				norm = huoqu_vec^jg_vec;
				normalize(norm);
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}

			BOOL bBerHuoQu = IsBerHuoQuJg(pHuoQuJg,Start()-End(),huoqu_x_wing0_y_wing1);
			f3dPoint huoqu_wing_vec,datum_face_norm,spread_face_norm;
			f3dLine huoqu_line,ray_line;
			ray_line.endPt = GetEndDatumPos();
			//���¼��и�Ϊֱ���ɽǸ����߷���ȷ�����߷��򣬵�û�о���ʵ�����飬��֪�Բ���
			//��Ҫ���� WJH-2005.03.07
			//Ϊ�˸������߲�����һ������ WJH-2004.05.31
			//if(pEnd->hFatherPart==handle)//&&pOldDesEndPos->offset_style!=1)
			//{	//�Ǹֶ˴��ڻ�׼λ��,��֪���ڻ����÷� WJH-2005.03.03
				ray_line.startPt=Start()-End()+ray_line.endPt;
			//}
			//else
			//	ray_line.endPt = GetStartDatumPos();
			if(huoqu_x_wing0_y_wing1==0)
			{
				huoqu_wing_vec = pHuoQuJg->GetWingVecX();
				//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
				ray_line.endPt+= pHuoQuJg->get_norm_x_wing()*desEndPos.datum_offset_dist;
			}
			else
			{
				huoqu_wing_vec = pHuoQuJg->GetWingVecY();
				//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
				ray_line.endPt+= pHuoQuJg->get_norm_y_wing()*desEndPos.datum_offset_dist;
			}

			huoqu_line.startPt = pHuoQuJg->Start();
			huoqu_line.endPt   = pHuoQuJg->End();
			if(bBerHuoQu)
			{
				huoqu_line.startPt = huoqu_line.startPt-huoqu_wing_vec*desEndPos.huoqu_dist;
				huoqu_line.endPt = huoqu_line.endPt-huoqu_wing_vec*desEndPos.huoqu_dist;
			}
			else
			{
				huoqu_line.startPt = huoqu_line.startPt+
					huoqu_wing_vec*(pHuoQuJg->GetWidth()+desEndPos.huoqu_dist);
				huoqu_line.endPt = huoqu_line.endPt+
					huoqu_wing_vec*(pHuoQuJg->GetWidth()+desEndPos.huoqu_dist);
			}
			//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
			if(huoqu_x_wing0_y_wing1==0)
			{
				huoqu_line.startPt+= pHuoQuJg->get_norm_x_wing()*desEndPos.datum_offset_dist;
				huoqu_line.endPt+= pHuoQuJg->get_norm_x_wing()*desEndPos.datum_offset_dist;
			}
			else
			{
				huoqu_line.startPt+= pHuoQuJg->get_norm_y_wing()*desEndPos.datum_offset_dist;
				huoqu_line.endPt+= pHuoQuJg->get_norm_y_wing()*desEndPos.datum_offset_dist;
			}
			f3dPoint huoqu_vec = huoqu_line.endPt-huoqu_line.startPt;
			f3dPoint ray_vec = ray_line.startPt-ray_line.endPt;
			spread_face_norm = huoqu_vec^ray_vec;
			if(desEndPos.IsInHuoQuLine())	//���ڻ�����׼�Ǹֻ�����
			{
				if(huoqu_x_wing0_y_wing1==0)	//������׼�Ǹ�X֫Ϊ��׼��
					datum_face_norm = pHuoQuJg->get_norm_x_wing();
				else
					datum_face_norm = pHuoQuJg->get_norm_y_wing();

				ray_line.endPt = SpreadFacePntToDatumFace(datum_face_norm,huoqu_line,
					spread_face_norm,ray_line.endPt);
				UCS_STRU inters_ucs;
				inters_ucs.axis_z = datum_face_norm.normalized();
				inters_ucs.axis_y = huoqu_vec.normalized();
				inters_ucs.axis_x = inters_ucs.axis_y^inters_ucs.axis_z;
				normalize(inters_ucs.axis_x);
				inters_ucs.axis_z = inters_ucs.axis_x^inters_ucs.axis_y;
				inters_ucs.origin = pDatumLinePart.LineAnglePointer()->GetDatumPosBer(pEnd);
				coord_trans(huoqu_line.startPt,inters_ucs,FALSE);
				coord_trans(huoqu_line.endPt,inters_ucs,FALSE);
				coord_trans(ray_line.startPt,inters_ucs,FALSE);
				coord_trans(ray_line.endPt,inters_ucs,FALSE);
				//�޸�Ϊ�Ի�����Ϊ׼������Z����Ϊ��(�Դ����׼֫����ƫ��) WJH-2003.12.23
				ray_line.startPt.z = ray_line.endPt.z = huoqu_line.startPt.z = huoqu_line.endPt.z;
				Int3dpl(huoqu_line,ray_line,datum_pos);
				coord_trans(datum_pos,inters_ucs,TRUE);
			}
			else if(desEndPos.IsInOffsetZhunLine())	//���ڻ�����׼�Ǹ�ƫ��׼��
			{
				f3dPoint spread_vec = spread_face_norm^huoqu_vec;
				if(spread_vec*ray_vec<0)	//����
					spread_vec*=-1;
				normalize(spread_vec);
				if(huoqu_x_wing0_y_wing1==0)	//������׼�Ǹ�X֫Ϊ��׼��
					datum_face_norm = pHuoQuJg->get_norm_x_wing();
				else
					datum_face_norm = pHuoQuJg->get_norm_y_wing();
				
				SnapPerp(&datum_pos,huoqu_line,pHuoQuJg->GetDatumPosBer(pEnd));//pStart->GetInverseNodePos()); //modified by wjh 04.11.11
				datum_pos +=spread_vec*desEndPos.offset_zhun;
				//����ڵ�������ӻ�׼�Ǹֳ��ȷ���ƫ�Ƶ�ƫ�Ʒ��� wht 10-12-09
				f3dPoint len_offset_vec=pDatumLinePart->End()-pDatumLinePart->Start();
				normalize(len_offset_vec);
				datum_pos+=len_offset_vec*desEndPos.len_offset_dist;
			}
		}
	}
	else if(desEndPos.spatialOperationStyle==15&&pEnd)
	{	//��֫�������
		if(desEndPos.cMainCoordCtrlWing=='X'||desEndPos.cMainCoordCtrlWing=='Y')
		{
			f3dPoint kp_main,kp_other;
			f3dPoint line_vec,main_wing_offset_vec,face_norm,other_wing_offset_vec;
			if(desEndPos.cMainCoordCtrlWing=='X')
			{
				if(desEndPos.ctrlWing.gStyle==4)
					main_wing_offset_vec=desEndPos.ctrlWing.offsetDist*GetWingVecX();
				else //if(desEndPos.ctrlWing.gStyle<4)
					main_wing_offset_vec=GetWingXG(desEndPos.ctrlWing.gStyle)*GetWingVecX();
				if(desEndPos.otherWing.gStyle==4)
					other_wing_offset_vec=desEndPos.otherWing.offsetDist*GetWingVecY();
				else //if(desEndPos.otherWing.gStyle<4)
					other_wing_offset_vec=GetWingYG(desEndPos.otherWing.gStyle)*GetWingVecY();
			}
			else //if(desEndPos.cMainCoordCtrlWing=='Y')
			{
				if(desEndPos.ctrlWing.gStyle==4)
					main_wing_offset_vec=desEndPos.ctrlWing.offsetDist*GetWingVecY();
				else //if(desEndPos.ctrlWing.gStyle<4)
					main_wing_offset_vec=GetWingYG(desEndPos.ctrlWing.gStyle)*GetWingVecY();
				if(desEndPos.otherWing.gStyle==4)
					other_wing_offset_vec=desEndPos.otherWing.offsetDist*GetWingVecX();
				else //if(desEndPos.otherWing.gStyle<4)
					other_wing_offset_vec=GetWingXG(desEndPos.otherWing.gStyle)*GetWingVecX();
			}
			//if(desEndPos.ctrlWing.operStyle!=0||desEndPos.otherWing.operStyle!=0)
			{	//��ƽ�ƹؼ���ʱ�����˼���(��)�ߵ����췽��
				line_vec=End()-Start();
				normalize(line_vec);
			}
			kp_main=End()+main_wing_offset_vec;
			f3dPoint kp_ber=End();
#ifndef __ALFA_TEST_VERSION_
			BOOL gbLegacyMode=FALSE;
			BOOL gbLegacyAngleCtrlWingModeForTMAV5_1_2=FALSE;
#endif
			//���㷨ģʽĬ������֫Ϊ����ͶӰ����ͨ����ϸУ�˳��ж����λֵ���������֫��ʽ��Ӱ�������) wjh-2016.6.04
			if(gbLegacyMode&&desEndPos.ctrlWing.operStyle!=0)
			{
				if(desEndPos.ctrlWing.operStyle==1)
					face_norm.Set(0,0,1);	//X-Yƽ����
				else if(desEndPos.ctrlWing.operStyle==2)
					face_norm.Set(1,0,0);	//Y-Zƽ����
				else if(desEndPos.ctrlWing.operStyle==3)
					face_norm.Set(0,1,0);	//X-Zƽ����
				if(pLift)
					face_norm=pLift->LiftVecTrans(face_norm,init_pos);
				Int3dlf(kp_main,kp_main,line_vec,init_pos,face_norm);
				//���ص�Ϊƽ�Ƶ�ʱ,Ӧ��ƽ�Ƶ㷴�����������,�������������ʼֵ����ʱ,���޷����� wjh-2012.5.1
				SnapPerp(&kp_ber,Start(),End(),kp_main);
			}
			kp_other=End()+other_wing_offset_vec;
			//���㷨ģʽ����֫ѡ��'����ͶӰ'ʱ����Ҫ���Ƕ����λֵ����������Ҫ�������ص� wjh-2016.6.04
			if(!gbLegacyMode||desEndPos.otherWing.operStyle!=0||desEndPos.ctrlWing.operStyle!=0)
			{	//ֻҪ����֫�򸨿�֫��һ֫Ϊƽ��,Ϊ��֤��������λ��ͬһ�Ǹֽ���ԭ��,�������ƽ�Ƽ���
				if(desEndPos.otherWing.operStyle==0)
					face_norm=line_vec;
				else if(desEndPos.otherWing.operStyle==1)
					face_norm.Set(0,0,1);	//X-Yƽ����
				else if(desEndPos.otherWing.operStyle==2)
					face_norm.Set(1,0,0);	//Y-Zƽ����
				else if(desEndPos.otherWing.operStyle==3)
					face_norm.Set(0,1,0);	//X-Zƽ����
				if(pLift)
					face_norm=pLift->LiftVecTrans(face_norm,init_pos);
				GEPOINT face_pick=init_pos;
				if(gbLegacyMode)
					Int3dlf(kp_other,kp_other,line_vec,init_pos,face_norm);
				else if(gbLegacyAngleCtrlWingModeForTMAV5_1_2)
				{
					if(desEndPos.ctrlWing.operStyle!=0&&desEndPos.otherWing.operStyle==0&&!gbLegacyMode)
						face_pick=kp_main+line_vec*desEndPos.fLenOffsetOfCoordWing;
					Int3dlf(kp_other,kp_other,line_vec,face_pick,face_norm);
					if(desEndPos.ctrlWing.operStyle==0&&desEndPos.otherWing.operStyle!=0&&!gbLegacyMode)
					{
						face_pick=kp_other+line_vec*desEndPos.fLenOffsetOfCoordWing;
						Int3dlf(kp_main,kp_main,line_vec,face_pick,face_norm);
					}
				}
				else
				{
					if(desEndPos.otherWing.operStyle==0)
						face_pick=kp_main+line_vec*desEndPos.fLenOffsetOfCoordWing;
					Int3dlf(kp_other,kp_other,line_vec,face_pick,face_norm);
				}
			}
			f3dPoint other_main=kp_main-kp_other;
			f3dPoint restore_vec=kp_ber-kp_main;//�˴�������������Ķ����,�����޷���������ʼֵ��������
			if(desEndPos.coordCtrlType==1)		//Y-Z����
				kp_main=init_pos+f3dPoint(other_main.x,0,0);
			else if(desEndPos.coordCtrlType==2)	//X-Y����
				kp_main=init_pos+f3dPoint(0,0,other_main.z);
			else //if(desEndPos.coordCtrlType==0)	//X-Z����
				kp_main=init_pos+f3dPoint(0,other_main.y,0);
			final_pos=kp_main+restore_vec;
		}
		else	//����ʼ�����
			final_pos=datum_pos=init_pos;
	}
	else if(desEndPos.spatialOperationStyle==16||desEndPos.spatialOperationStyle==17)
	{	//��ָ����ͶӰ��ƽ��
		CLDSPoint* pDatumPoint=NULL;
		CLDSLine* pDatumLine=desEndPos.hDatumLine>0X20 ? BelongModel()->FromLineHandle(desEndPos.hDatumLine) : NULL;
		if(pDatumLine)
		{	//�˴����»�׼�߷���������ģ�͸˼�λ�ø�������� wjh-2020.1.2
			if (pDatumLine->hPointArr[0].bSimpleNodePoint)
			{	//��ʱ�趨��׼�ߵ���ʼ�㶨�����
				desEndPos.datumLineStart.datum_pos_style=8;
				desEndPos.datumLineStart.des_para.hNode=pDatumLine->hPointArr[0];
				desEndPos.datumLineStart.UpdatePos(BelongModel(),pLift);
			}
			else if (pDatumLine->hPointArr[0].bRodTipStart||pDatumLine->hPointArr[0].bRodTipEnd)
			{	//�˼�ʼĩ�˵�Ϊ��׼����ʼ��
				desEndPos.datumLineStart.datum_pos_style=1;
				desEndPos.datumLineStart.des_para.RODEND.hRod=pDatumLine->hPointArr[0];
				desEndPos.datumLineStart.des_para.RODEND.direction=pDatumLine->hPointArr[0].bRodTipStart?0:1;
				desEndPos.datumLineStart.des_para.RODEND.SetWingOffset(0);
				desEndPos.datumLineStart.des_para.RODEND.norm_offset[0]=0;
				desEndPos.datumLineStart.UpdatePos(BelongModel(),pLift);
			}
			else //if (pDatumLine->hPointArr[0].ciHidType==0)
			{
				pDatumPoint=BelongModel()->FromPointHandle(pDatumLine->hPointArr[0]);
				if (pDatumPoint!=NULL)
				{
					pDatumPoint->UpdatePos(BelongModel(),pLift);
					desEndPos.datumLineStart=*pDatumPoint;
				}
				else
					logerr.Log("0x%X�Ǹ�ĩ��ͶӰ(ƽ��)ָ���ߵ�ʼ�˵㶨������!",handle);
			}
			if (pDatumLine->hPointArr[1].bSimpleNodePoint)
			{	//��ʱ�趨��׼�ߵ���ʼ�㶨�����
				desEndPos.datumLineEnd.datum_pos_style=8;
				desEndPos.datumLineEnd.des_para.hNode=pDatumLine->hPointArr[1];
				desEndPos.datumLineEnd.UpdatePos(BelongModel(),pLift);
			}
			else if (pDatumLine->hPointArr[1].bRodTipStart||pDatumLine->hPointArr[1].bRodTipEnd)
			{	//�˼�ʼĩ�˵�Ϊ��׼����ʼ��
				desEndPos.datumLineEnd.datum_pos_style=1;
				desEndPos.datumLineEnd.des_para.RODEND.hRod=pDatumLine->hPointArr[1];
				desEndPos.datumLineEnd.des_para.RODEND.direction=pDatumLine->hPointArr[1].bRodTipStart?0:1;
				desEndPos.datumLineEnd.des_para.RODEND.SetWingOffset(0);
				desEndPos.datumLineEnd.des_para.RODEND.norm_offset[0]=0;
				desEndPos.datumLineEnd.UpdatePos(BelongModel(),pLift);
			}
			else //if (pDatumLine->hPointArr[1].ciHidType==0)
			{
				pDatumPoint=BelongModel()->FromPointHandle(pDatumLine->hPointArr[1]);
				if (pDatumPoint!=NULL)
				{
					pDatumPoint->UpdatePos(BelongModel(),pLift);
					desEndPos.datumLineEnd=*pDatumPoint;
				}
				else
					logerr.Log("0x%X�Ǹ�ĩ��ͶӰ(ƽ��)ָ���ߵ��ն˵㶨������!",handle);
			}
		}
		else
		{
			desEndPos.datumLineStart.UpdatePos(BelongModel(),pLift);
			desEndPos.datumLineEnd.UpdatePos(BelongModel(),pLift);
		}
		f3dPoint start=desEndPos.datumLineStart.Position();
		f3dPoint end=desEndPos.datumLineEnd.Position();
		if(pLift&&desEndPos.datumLineStart.datum_pos_style==0)
			start=pLift->LiftPosTrans(desEndPos.datumLineStart.Position());
		if(pLift&&desEndPos.datumLineEnd.datum_pos_style==0)
			end=pLift->LiftPosTrans(desEndPos.datumLineEnd.Position());
		//Ϊ�˱������ɴ�����άʵ��ģ��ʱ����趨��������λ�ã����ӻ�׼�߲��նԳơ�wjh-2015.12.04
		if(desEndPos.hDatumLine>0X20&&toupper(desEndPos.cMirTransType)=='X')
		{
			start.y*=-1.0;
			end.y*=-1.0;
		}
		else if(desEndPos.hDatumLine>0X20&&toupper(desEndPos.cMirTransType)=='Y')
		{
			start.x*=-1.0;
			end.x*=-1.0;
		}
		else if(desEndPos.hDatumLine>0X20&&toupper(desEndPos.cMirTransType)=='Z')
		{
			start.x*=-1.0;
			start.y*=-1.0;
			end.x*=-1.0;
			end.y*=-1.0;
		}
		
		/*double alfa=Pi-GetRadWingAngle();
		sina=sin(alfa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		double w_d=(pOldDesStartPos->fStartJgLengOffsetY-pOldDesStartPos->fStartJgLengOffsetX*cos(alfa))/sina;
		start+=get_norm_x_wing()*pOldDesStartPos->fStartJgLengOffsetX-GetWingVecX()*w_d;

		alfa=Pi-GetRadWingAngle();
		sina=sin(alfa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		w_d=(pOldDesStartPos->fEndJgLengOffsetY-pOldDesStartPos->fEndJgLengOffsetX*cos(alfa))/sina;
		end+=get_norm_x_wing()*desEndPos.fEndJgLengOffsetX-GetWingVecX()*w_d;*/

		final_pos=End();
		if(pEnd&&pEnd->m_cPosCalType==4)	//���Ǹֽ��潻��
		{
			pEnd->GetIntersZhun(&init_pos);
			SnapPerp(&final_pos,start,end,init_pos);
			SetEnd(final_pos);
			return;
		}
		else if(pEnd&&pEnd->m_cPosCalType==7&&	//��ͨ�ظ˼����ȷ���ƫ�ƵĽڵ�
			(pEnd->m_cPosCalViceType!='X'&&pEnd->m_cPosCalViceType!='Y'&&pEnd->m_cPosCalViceType!='Z'))
		{	//GetDatumPosBer(pDatumNode)�õ��ĵ��λ�ã������ɽڵ���˼�ͶӰ����õ��ģ�����׼�ڵ�ĸ��˼���λ��ʽΪƽ�ƣ�
			//�����׼�ڵ㸸�˼������˼���(����ƫ�ƽڵ��)һ�ˣ���λ��ʽ�޸�Ϊƽ�Ʋ�������  wht 09-07-17	
			CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[0]);
			//ƫ�ƽڵ��׼�ڵ㸸�˼�Ϊ��ǰ�˼�ʱ��(pDatumNode->hFatherPart==handle)
			//���ݻ�׼�ڵ��Լ�ƫ�ƾ��������ƫ�ƽڵ�Ϊ�˵�ĸ˼���ʼ��λ�� wht 10-10-19
			CLDSLineAngle *pDatumFatherJg=GetEndNodeOffsetDatumAngle();
			if(pDatumFatherJg)
			{	//�����׼�ڵ㸸�˼���λ��ʽΪƽ��ʱ����GetDatumPosBerByFaceOffset() wht 10-07-30
				f3dPoint pos=pDatumFatherJg->GetDatumPosBer(pDatumNode);
				f3dPoint face_norm;
				if(pDatumFatherJg->desStartPos.IsFaceOffset())//||
					face_norm=pDatumFatherJg->desStartPos.face_offset_norm;
				else if(pDatumFatherJg->desEndPos.IsFaceOffset())
					face_norm=pDatumFatherJg->desEndPos.face_offset_norm;
				if(fabs(lenStdVec*face_norm)>0.25)	//��׼�ڵ㶨λ��ʽΪƽ��
				{	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
					if(pLift)
					{
						bool bInRightSide=true;
						if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
							bInRightSide=false;
						else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
							bInRightSide=false;
						face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
					}
					pos=pDatumFatherJg->GetDatumPosBerByFaceOffset(pDatumNode,face_norm,&pos);	
				}
				else if(desEndPos.IsFaceOffset())
				{
		#ifdef AFX_TARG_ENU_ENGLISH
					Log2File()->Log("The offset defination of angle 0X%X end is error or rod's extend direction is too close,planar loacation's calculation failed!",handle);
		#else 
					Log2File()->Log("0X%X�Ǹ��ն�ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
		#endif
				}
				f3dPoint line_vec=pDatumFatherJg->End()-pDatumFatherJg->Start();
				normalize(line_vec);
				CLDSNode* pAttachNode1=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[1]);
				CLDSNode* pAttachNode2=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[2]);
				if(pAttachNode1&&pAttachNode2&&(line_vec*(pAttachNode2->Position()-pAttachNode1->Position())<0))
					line_vec*=-1.0;
				//������ƫ����ͶӰ�������޷�ͬʱ��֤ƫ������ͶӰֵ
				final_pos=pos+line_vec*pEnd->attach_offset;
				SnapPerp(&final_pos,start,end,final_pos);
				SetEnd(final_pos);
				return;
			}
		}
		else
		{
			if(desEndPos.spatialOperationStyle==16)	//ͶӰ
				SnapPerp(&final_pos,start,end,init_pos);
			else //if(desEndPos.spatialOperationStyle=17)	//ƽ��
			{
				f3dPoint face_norm=desEndPos.face_offset_norm;
				if(pLift)
				{
					bool bInRightSide=true;
					if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
						bInRightSide=false;
					else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
						bInRightSide=false;
					face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
				}
				Int3dlf(final_pos,f3dLine(start,end),init_pos,face_norm);
				if(desEndPos.cFaceOffsetDatumLine=='X'||desEndPos.cFaceOffsetDatumLine=='Y')
				{	//ƽ�ƻ�׼��Ϊ֫����
					SetEnd(final_pos);
					f3dPoint wing_pos;//,wing_prj_vec
					//f3dPoint line_vec=final_pos-End();
					//normalize(line_vec);
					if(desEndPos.cFaceOffsetDatumLine=='X')	//ƽ�ƻ�׼��ΪX֫����
						wing_pos=GetSelfDatumPosWingXByFaceOffset(pEnd,face_norm,&init_pos);
					else
						wing_pos=GetSelfDatumPosWingYByFaceOffset(pEnd,face_norm,&init_pos);
					SnapPerp(&final_pos,Start(),End(),wing_pos);
				}
			}
			datum_pos=init_pos;
			SetEnd(final_pos);
			return;
		}
	}
	else //if(desEndPos.spatialOperationStyle==18||desEndPos.spatialOperationStyle==19)
		datum_pos=init_pos;	//������Ƶ�ͶӰ��ƽ��
	if(pDatumLinePart.IsHasPtr()&&pDatumLinePart->GetClassTypeId()==CLS_LINETUBE&&
		(desEndPos.spatialOperationStyle==1||desEndPos.spatialOperationStyle==4))
	{	//���Ǹִ���ڸֹ���ʱʹ��ƫ�ľ�������Ǹֲ����ڸֹ�������(��������1/4������)����� wht 09-10-17
		f3dLine datumLine(pDatumLinePart->Start(),pDatumLinePart->End());
		f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
		f3dPoint cur_vec=End()-Start();
		f3dPoint plane_norm=datum_vec^cur_vec;
		f3dPoint eccentricOffsetVec=datum_vec^plane_norm;
		normalize(eccentricOffsetVec);
		if(eccentricOffsetVec*cur_vec<EPS)
			eccentricOffsetVec*=-1.0;
		if(fabs(desEndPos.fEccentricDist)>EPS)
		{	//ƫ�ľ�
			if(desEndPos.spatialOperationStyle==1||desEndPos.face_offset_norm.IsZero()) //ͶӰ
				datum_pos+=eccentricOffsetVec*desEndPos.fEccentricDist;
			else //if(desEndPos.spatialOperationStyle==4) //ƽ��
			{	
				datumLine.startPt+=eccentricOffsetVec*desEndPos.fEccentricDist;
				datumLine.endPt+=eccentricOffsetVec*desEndPos.fEccentricDist;
				f3dPoint face_pick=datum_pos;
				f3dPoint face_norm=desEndPos.face_offset_norm;
				if(pLift)
				{
					bool bInRightSide=true;
					if(pLift->m_cRotAxis=='Y'&&init_pos.x<0)
						bInRightSide=false;
					else if(pLift->m_cRotAxis=='X'&&init_pos.y>0)
						bInRightSide=false;
					face_norm=pLift->LiftVecTrans(face_norm,bInRightSide);
				}
				Int3dlf(datum_pos,datumLine,face_pick,face_norm);
			}
		}
	}

	//������ ����->����ʸ��ƫ�ƣ��������ڴ�Ӹ˼���(��)���λ�ü���
	if((desEndPos.spatialOperationStyle>0&&desEndPos.spatialOperationStyle<15)&&//���޴�����������֫�������ʱ��λ�ÿ��ܻ����㡡wjh-2015.3.2
		desEndPos.wing_x_offset.gStyle==4&&fabs(desEndPos.wing_x_offset.offsetDist)<EPS &&
		desEndPos.wing_y_offset.gStyle==4&&fabs(desEndPos.wing_y_offset.offsetDist)<EPS)
		final_pos=datum_pos;
	if(desEndPos.spatialOperationStyle<15||desEndPos.spatialOperationStyle==18)
	{	//��ʽ�ӿ��ƽڵ�ͶӰ����Ǹ�������ͨ��Ӽ���Ǹ����
		f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
		short work_wing_style=desEndPos.cWorkWingStyle;
#ifndef __ALFA_TEST_VERSION_
		BOOL gbLegacyMode=FALSE;
#endif
		if(desEndPos.cWorkWingStyle==0&&pDatumLinePart.IsHasPtr())
		{	//��ʽ�ӿ��ƽڵ�ͶӰ����Ǹ����
			if(pDatumLinePart->handle==handle||desEndPos.spatialOperationStyle==18)
			{	//��ʼ�ڵ㸸�Ǹ�Ϊ��ǰ�Ǹ֣���ʱƫ�Ʒ����ؽǸ���֫����
				if( desEndPos.wing_x_offset.gStyle==4&&desEndPos.wing_x_offset.offsetDist==0&&(
					desEndPos.wing_y_offset.gStyle!=4||desEndPos.wing_y_offset.offsetDist!=0))
					work_wing_style=1;	//X֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��X֫Ϊ��ǰ����֫
				else if(desEndPos.wing_y_offset.gStyle==4&&desEndPos.wing_y_offset.offsetDist==0&&(
						desEndPos.wing_x_offset.gStyle!=4||desEndPos.wing_x_offset.offsetDist!=0))
					work_wing_style=2;	//Y֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��Y֫Ϊ��ǰ����֫
				else
					work_wing_style=3;
			}
			else
			{	//��ӽǸ��ص�ǰ����֫�ķ��߼�֫�������ƫ��
				f3dPoint father_vec,cur_vec,work_norm;
				if(pDatumLinePart.IsHasPtr())
				{
					cur_vec = End()-Start();
					father_vec = pDatumLinePart->End()-pDatumLinePart->Start();
					normalize(cur_vec);
					normalize(father_vec);
					if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
					{
						if(desEndPos.IsInDatumJgWingX())
							work_norm=pDatumLinePart.LineAnglePointer()->get_norm_x_wing();
						else if(desEndPos.IsInDatumJgWingY())
							work_norm=pDatumLinePart.LineAnglePointer()->get_norm_y_wing();
						else if(!gbLegacyMode&&
							(norm_x_wing*pDatumLinePart.LineAnglePointer()->vxWingNorm>EPS_COS2&&norm_y_wing*pDatumLinePart.LineAnglePointer()->vyWingNorm>EPS_COS2)||
							(norm_x_wing*pDatumLinePart.LineAnglePointer()->vyWingNorm>EPS_COS2&&norm_y_wing*pDatumLinePart.LineAnglePointer()->vxWingNorm>EPS_COS2))
							work_wing_style=3;	//�Զ��ж�Ϊ˫֫��λ
						else
							work_norm=father_vec^cur_vec;
					}
					else if(fabs(father_vec*cur_vec)<EPS_COS2)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
						work_norm = father_vec^cur_vec;
				}
				if(normalize(work_norm)!=0)
				{
					if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
						work_wing_style=1;
					else
						work_wing_style=2;
				}
				else
					work_wing_style=3;
			}
		}
		else if(desEndPos.cWorkWingStyle==0)
			work_wing_style=3;
		if(work_wing_style<3)
		{	//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
			f3dPoint* pSelfWorkNorm=NULL;
			if(work_wing_style==1)
			{
				offset_vec_x = get_norm_x_wing();
				offset_vec_y = -GetWingVecX();
				pSelfWorkNorm=&offset_vec_x;
			}
			else
			{
				offset_vec_x = -GetWingVecY();
				offset_vec_y = get_norm_y_wing();
				pSelfWorkNorm=&offset_vec_y;
			}
			if( pSelfWorkNorm&&pDatumLinePart.IsHasPtr()&&!gbLegacyMode&&BelongModel()->RepositionAlgor()!=1&&
				!desEndPos.IsInHuoQuLine()&&!desEndPos.IsInOffsetZhunLine()&&
				(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
			{
				f3dPoint vWorkNorm;
				if(desEndPos.IsInDatumJgWingX())
					vWorkNorm=pDatumLinePart.LineAnglePointer()->vxWingNorm;
				else if(desEndPos.IsInDatumJgWingY())
					vWorkNorm=pDatumLinePart.LineAnglePointer()->vyWingNorm;
				double justify=(*pSelfWorkNorm)*vWorkNorm;
				if(justify>EPS_COS2)
					(*pSelfWorkNorm)=vWorkNorm;
				else if(justify<-EPS_COS2)
					(*pSelfWorkNorm)=-vWorkNorm;
			}
			if(desEndPos.wing_x_offset.gStyle==0)
				final_pos=datum_pos+offset_vec_x*xWingYZhunJu.g;
			else if(desEndPos.wing_x_offset.gStyle==1)
				final_pos=datum_pos+offset_vec_x*xWingYZhunJu.g1;
			else if(desEndPos.wing_x_offset.gStyle==2)
				final_pos=datum_pos+offset_vec_x*xWingYZhunJu.g2;
			else if(desEndPos.wing_x_offset.gStyle==3)
				final_pos=datum_pos+offset_vec_x*xWingYZhunJu.g3;
			else //if(desEndPos.wing_x_offset.gStyle==4)
				final_pos=datum_pos+offset_vec_x*desEndPos.wing_x_offset.offsetDist;
			if(desEndPos.wing_y_offset.gStyle==0)
				final_pos+=offset_vec_y*xWingXZhunJu.g;
			else if(desEndPos.wing_y_offset.gStyle==1)
				final_pos+=offset_vec_y*xWingXZhunJu.g1;
			else if(desEndPos.wing_y_offset.gStyle==2)
				final_pos+=offset_vec_y*xWingXZhunJu.g2;
			else if(desEndPos.wing_y_offset.gStyle==3)
				final_pos+=offset_vec_y*xWingXZhunJu.g3;
			else //if(desEndPos.wing_y_offset.gStyle==4)
				final_pos+=offset_vec_y*desEndPos.wing_y_offset.offsetDist;
		}
		else
		{
			double a,b,c,alfa;
			if(desEndPos.wing_x_offset.gStyle==0)
				a = xWingYZhunJu.g;
			else if(desEndPos.wing_x_offset.gStyle==1)
				a = xWingYZhunJu.g1;
			else if(desEndPos.wing_x_offset.gStyle==2)
				a = xWingYZhunJu.g2;
			else if(desEndPos.wing_x_offset.gStyle==3)
				a = xWingYZhunJu.g3;
			else //if(desEndPos.wing_x_offset.gStyle==4)
				a = desEndPos.wing_x_offset.offsetDist;
			if(desEndPos.wing_y_offset.gStyle==0)
				b = xWingXZhunJu.g;
			else if(desEndPos.wing_y_offset.gStyle==1)
				b = xWingXZhunJu.g1;
			else if(desEndPos.wing_y_offset.gStyle==2)
				b = xWingXZhunJu.g2;
			else if(desEndPos.wing_y_offset.gStyle==3)
				b = xWingXZhunJu.g3;
			else //if(desEndPos.wing_y_offset.gStyle==4)
				b = desEndPos.wing_y_offset.offsetDist;
			alfa = GetRadWingAngle();
			/*c = sqrt((b*b-2*a*b*cos(alfa)+a*a*cos(alfa)*cos(alfa))/(sin(alfa)*sin(alfa))+a*a-b*b);
			//if(a>0)
			//start_xyz = start_pos+norm_y_wing*b-GetWingVecY()*c;
			//else
			//start_xyz = start_pos+norm_y_wing*b+GetWingVecY()*c;*/
			sina=sin(alfa);
			if(fabs(sina)<EPS)
				sina=1;		//�Է����ֳ�0
			c = (b-a*cos(alfa))/sina;
			final_pos=datum_pos-GetWingVecY()*a+get_norm_y_wing()*c;
		}
	}
	else if(desEndPos.spatialOperationStyle==19)
	{	//��ʽ�ӿ��ƽڵ�ƽ�Ƽ���Ǹ����
		if(fabs(lenStdVec*desEndPos.face_offset_norm)<0.25)	//ƽ���淨���뵱ǰ�Ǹ����췽��н�С��15����Ϊ�Ǵ������ݣ�ִ��ͶӰ wjh-2015.2.3
		{
			if(log2file)
				log2file->Log("0X%X�Ǹ��ն�ƽ���涨���������˼����췽����ڿ�����ƽ�Ƽ���ʧ��!",handle);
			return;
		}
		f3dPoint wing_x_pos=GetEndWingDatumPosByFaceOffset('X',desEndPos.face_offset_norm);
		//f3dPoint wing_x_pos=GetSelfDatumPosWingXByFaceOffset(pEnd,desEndPos.face_offset_norm);
		f3dPoint wing_y_pos=GetEndWingDatumPosByFaceOffset('Y',desEndPos.face_offset_norm);
		//f3dPoint wing_y_pos=GetSelfDatumPosWingYByFaceOffset(pEnd,desEndPos.face_offset_norm);
		f3dPoint end_pos=GetDatumPosBerByFaceOffset(pEnd,desEndPos.face_offset_norm);
		f3dPoint wing_x_prj_vec=wing_x_pos-end_pos;
		f3dPoint wing_y_prj_vec=wing_y_pos-end_pos;
		final_pos=CalOffsetAngleBer(pEnd->Position(true),wing_x_prj_vec,wing_y_prj_vec);
		SetEnd(final_pos);
		if(desEndPos.cFaceOffsetDatumLine=='X'||desEndPos.cFaceOffsetDatumLine=='Y')
		{	//ƽ�ƻ�׼��Ϊ֫����
			f3dPoint wing_prj_vec,wing_pos;
			//f3dPoint line_vec=Start()-End();
			//normalize(line_vec);
			if(desEndPos.cFaceOffsetDatumLine=='X')	//ƽ�ƻ�׼��ΪX֫����
				wing_pos=GetSelfDatumPosWingXByFaceOffset(pEnd,desEndPos.face_offset_norm);
			else //if(desEndPos.cFaceOffsetDatumLine=='Y')
				wing_pos=GetSelfDatumPosWingYByFaceOffset(pEnd,desEndPos.face_offset_norm);
			SnapPerp(&final_pos,Start(),End(),wing_pos);
		}
	}
	SetEnd(final_pos);
}
void CLDSLineAngle::cal_start_ber_pos()
{
	JGZJ jgzj;
	char sError[200];
	double sina;
	if(IsStartPosLocked())
		return;			//ʼ��λ���ѱ������������ǻ����㣩
	if(BelongModel()==NULL)
		return;
	if(pStart==NULL)	//�Ҳ���ʼ�ڵ�
	{
		pOldDesStartPos->datum_point.UpdatePos(BelongModel());
		SetStart(pOldDesStartPos->datum_point.Position());
		SetModified();
		return;
	}
	if(pStart->hFatherPart<0x20)
		return;
	SetModified();
	if(!LockStart())
		return;	//�ѵ�λ
	if(!getjgzj(jgzj,size_wide))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sError,"The specifications %.0fX%.0f of angle can't find in stadia library,failed calculate position!",size_wide,size_thick);
#else
		sprintf(sError,"%.0fX%.0f���ĽǸ���׼������Ҳ���,λ�ü���ʧ��!",size_wide,size_thick);
#endif
		throw sError;
	}
	if(pOldDesStartPos->jgber_cal_style==0)
	{	//������ƫ��
		CSuperSmartPtr<CLDSLinePart> pFatherLinePart;
		f3dPoint start_pos;	//�Ǹ���ʼ��ֹλ�õļ����׼��
		//---------------��һ��.���׼��--------------
		if(pOldDesStartPos->datum_pos_style==0)	//�Խڵ�Ϊ����λ��
			start_pos = pStart->Position(true);	//�Խڵ�����׼��λ��Ϊ��׼
		else if(pOldDesStartPos->datum_pos_style<7)	//Ԥ��׼��
		{
			f3dPoint datum_start,datum_end,datum_point;
			pFatherLinePart=(CLDSLinePart*)GetStartDatumPart();
			if(pFatherLinePart.IsNULL())
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Can't find the datum angle which lap with angle's start!";
#else
				throw "�Ҳ����Ǹ�ʼ�˴��λ�û�׼�Ǹ�!";
#endif
			if(pFatherLinePart.IsEqualPtr(this))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Datum point positions datum angle,angle itself,the design parameter of position is wrong,please redesign 0X%X angle's start position!",handle);
#else
				sprintf(sError,"���㶨λ��׼�Ǹּ��Ǹֱ���,λ����Ʋ�������,���������0X%X�Ǹ�ʼ��λ��!",handle);
#endif
				throw sError;
			}
			//�����������������ѭ������֪�������ں��ֿ��ǲ�������˼�������ʶ wjh 2010-10-18
			//pFatherLinePart->ClearFlag();
			pFatherLinePart->CalPosition();	//���Ǹֵ�λ
			
			if(pFatherLinePart->GetClassTypeId()==CLS_LINETUBE)
				SnapPerp(&start_pos,pFatherLinePart->Start(),pFatherLinePart->End(),pStart->Position(true));
			else if(pFatherLinePart->GetClassTypeId()==CLS_LINEANGLE||pFatherLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				//Ϊ�˽�����ƽ��(��׼λ��&���λ��)�ϲ�,��bDatumFaceOffset==1
				//offset_style==1 DatumFaceOffsetNorm->face_offset_norm WJH-2003.12.18
				if(pOldDesStartPos->datum_pos_style==1)	//�Ը��Ǹ�����Ϊ��׼
				{
					//������´������Ľ��������Ķ˵�����ͶӰʱ������ֱ��
					//����GetDatumPosBer����Ϊ��ʱ���迼�ǹ̶�����ͷӰ��
					if(pStart==pFatherLinePart->pStart)
						start_pos=pFatherLinePart->Start();
					else if(pStart==pFatherLinePart->pEnd)
						start_pos=pFatherLinePart->End();
					else
						start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosBer(pStart);
					if(pFatherLinePart.LineAnglePointer()->IsReservedHeJiao())
					{	//�����б���֫��ȵ�����Ͻ�
						f3dPoint len_offset_vec=pFatherLinePart->End()-pFatherLinePart->Start();
						f3dPoint vec=End()-Start();
						f3dPoint work_norm=vec^len_offset_vec;
						normalize(work_norm);
						if( fabs(work_norm*pFatherLinePart.LineAnglePointer()->get_norm_x_wing())>
							fabs(work_norm*pFatherLinePart.LineAnglePointer()->get_norm_y_wing()))
							start_pos+=pFatherLinePart.LineAnglePointer()->CalWingOffsetVec('X',0);
						else
							start_pos+=pFatherLinePart.LineAnglePointer()->CalWingOffsetVec('Y',0);
					}
				}
				else if(pOldDesStartPos->datum_pos_style==2)	//�Ը��Ǹ�X֧Ϊ��׼
					start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingX(pStart);
				else if(pOldDesStartPos->datum_pos_style==3)	//�Ը��Ǹ�Y֧Ϊ��׼
					start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingY(pStart);
				else if(pOldDesStartPos->datum_pos_style==4)	//�Ը��Ǹ�����ƽ��Ϊ��׼
				{
					char cDatumWing=0;
					if(pFatherLinePart.LineAnglePointer()->IsReservedHeJiao())
					{	//�����б���֫��ȵ�����Ͻ�
						f3dPoint len_offset_vec=pFatherLinePart->End()-pFatherLinePart->Start();
						f3dPoint vec=End()-Start();
						f3dPoint work_norm=vec^len_offset_vec;
						normalize(work_norm);
						if( fabs(work_norm*pFatherLinePart.LineAnglePointer()->get_norm_x_wing())>
							fabs(work_norm*pFatherLinePart.LineAnglePointer()->get_norm_y_wing()))
							cDatumWing='X';
						else
							cDatumWing='Y';
					}
					if(pOldDesStartPos->bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,&pt,cDatumWing);
					}
					else
						start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,NULL,cDatumWing);
				}
				else if(pOldDesStartPos->datum_pos_style==5)	//�Ը��Ǹ�X֫Ϊ��׼
				{
					if(pOldDesStartPos->bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,&pt);
					}
					else
						start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,pOldDesStartPos->face_offset_norm);
				}
				else //if(pOldDesStartPos->datum_pos_style==6)	//�Ը��Ǹ�Y֫Ϊ��׼
				{
					if(pOldDesStartPos->bByOtherEndOffset)
					{
						f3dPoint pt = GetEndDatumPos();
						start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,&pt);
					}
					else
						start_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,pOldDesStartPos->face_offset_norm);
				}
			}
			//����Ǹ�ʼ�˴�ӻ�׼�˼�(�Ǹֻ�ֹ�)�ĳ��ȷ��򣬿��ǻ������췽��ƫ���� wht 11-05-19
			f3dPoint len_offset_vec=pFatherLinePart->End()-pFatherLinePart->Start();
			normalize(len_offset_vec);
			start_pos+=len_offset_vec*pOldDesStartPos->len_offset_dist;
		}
		else if(pOldDesStartPos->datum_pos_style<15)
		{	//���ǻ�����1������/2ƫ��׼�ߣ�
			f3dPoint norm;
			CSmartPtr<CLDSLineAngle>pStartFatherJg,pHuoQuJg;
			CLDSLinePart *pPart=GetStartDatumPart();
			if(pPart&&(pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE))
				pStartFatherJg=(CLDSLineAngle*)pPart;
			if(pStartFatherJg.IsNULL())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Can't find the datum angle which 0X%X angle's start datum point positions!",handle);
#else
				sprintf(sError,"�Ҳ����Ǹ�0X%Xʼ�˻��㶨λ��׼�Ǹ�!",handle);
#endif
				throw sError;
			}
			if(pStartFatherJg.IsEqualPtr(this))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Datum point positions datum angle,angle itself,the design parameter of position is wrong,please redesign 0X%X angle's start position!",handle);
#else
				sprintf(sError,"���㶨λ��׼�Ǹּ��Ǹֱ���,λ����Ʋ�������,���������0X%X�Ǹ�ʼ��λ��!",handle);
#endif
				throw sError;
			}
			if(pOldDesStartPos->huoqu_jg_h>0x20)
				pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesStartPos->huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pHuoQuJg.IsNULL())
				pHuoQuJg=pStartFatherJg;
			int huoqu_x_wing0_y_wing1;
			if(pOldDesStartPos->huoqu_pos_auto_0_x_wing1_y_wing2==1)
				huoqu_x_wing0_y_wing1 = 0;
			else if(pOldDesStartPos->huoqu_pos_auto_0_x_wing1_y_wing2==2)
				huoqu_x_wing0_y_wing1 = 1;
			else if(pStartFatherJg==pHuoQuJg&&pOldDesStartPos->IsInDatumJgWingX())
				huoqu_x_wing0_y_wing1 = 0;
			else if(pStartFatherJg==pHuoQuJg&&pOldDesStartPos->IsInDatumJgWingY())
				huoqu_x_wing0_y_wing1 = 1;
			else if(pOldDesStartPos->IsInDatumJgWingX())
			{
				norm = pStartFatherJg->get_norm_x_wing();
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			else if(pOldDesStartPos->IsInDatumJgWingY())
			{
				norm = pStartFatherJg->get_norm_y_wing();
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			else
			{
				f3dPoint huoqu_vec,jg_vec;
				huoqu_vec = pHuoQuJg->pEnd->GetInverseNodePos(pHuoQuJg)-pHuoQuJg->pStart->GetInverseNodePos(pHuoQuJg);
				jg_vec = pEnd->GetInverseNodePos(this)-pStart->GetInverseNodePos(this);
				norm = huoqu_vec^jg_vec;
				normalize(norm);
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}

			BOOL bBerHuoQu = IsBerHuoQuJg(pHuoQuJg,End()-Start(),huoqu_x_wing0_y_wing1);
			f3dPoint huoqu_wing_vec,datum_face_norm,spread_face_norm;
			f3dLine huoqu_line,ray_line;
			ray_line.startPt = GetStartDatumPos();
			//���¼��и�Ϊֱ���ɽǸ����߷���ȷ�����߷��򣬵�û�о���ʵ�����飬��֪�Բ���
			//��Ҫ���� WJH-2005.03.07
			//Ϊ�˸������߲�����һ������ WJH-2004.05.31
			//if(pEnd->hFatherPart==handle)//&&des_end_pos.offset_style!=1)
			//{	//�Ǹֶ˴��ڻ�׼λ��,��֪���ڻ����÷� WJH-2005.03.03
				ray_line.endPt=End()-Start()+ray_line.startPt;
			//}
			//else
			//	ray_line.endPt = GetEndDatumPos();
			if(huoqu_x_wing0_y_wing1==0)
			{
				huoqu_wing_vec = pHuoQuJg->GetWingVecX();
				//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
				ray_line.startPt+= pHuoQuJg->get_norm_x_wing()*pOldDesStartPos->datum_offset_dist;
			}
			else
			{
				huoqu_wing_vec = pHuoQuJg->GetWingVecY();
				//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
				ray_line.startPt+= pHuoQuJg->get_norm_y_wing()*pOldDesStartPos->datum_offset_dist;
			}

			huoqu_line.startPt = pHuoQuJg->Start();
			huoqu_line.endPt   = pHuoQuJg->End();
			if(bBerHuoQu)
			{
				huoqu_line.startPt = huoqu_line.startPt-huoqu_wing_vec*pOldDesStartPos->huoqu_dist;
				huoqu_line.endPt = huoqu_line.endPt-huoqu_wing_vec*pOldDesStartPos->huoqu_dist;
			}
			else
			{
				huoqu_line.startPt = huoqu_line.startPt+
					huoqu_wing_vec*(pHuoQuJg->GetWidth()+pOldDesStartPos->huoqu_dist);
				huoqu_line.endPt = huoqu_line.endPt+
					huoqu_wing_vec*(pHuoQuJg->GetWidth()+pOldDesStartPos->huoqu_dist);
			}
			//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
			if(huoqu_x_wing0_y_wing1==0)
			{
				huoqu_line.startPt+= pHuoQuJg->get_norm_x_wing()*pOldDesStartPos->datum_offset_dist;
				huoqu_line.endPt+= pHuoQuJg->get_norm_x_wing()*pOldDesStartPos->datum_offset_dist;
			}
			else
			{
				huoqu_line.startPt+= pHuoQuJg->get_norm_y_wing()*pOldDesStartPos->datum_offset_dist;
				huoqu_line.endPt+= pHuoQuJg->get_norm_y_wing()*pOldDesStartPos->datum_offset_dist;
			}
			f3dPoint huoqu_vec = huoqu_line.endPt-huoqu_line.startPt;
			f3dPoint ray_vec = ray_line.endPt-ray_line.startPt;
			spread_face_norm = huoqu_vec^ray_vec;
			if(pOldDesStartPos->IsInHuoQuLine())	//���ڻ�����׼�Ǹֻ�����
			{
				if(huoqu_x_wing0_y_wing1==0)	//������׼�Ǹ�X֫Ϊ��׼��
					datum_face_norm = pHuoQuJg->get_norm_x_wing();
				else
					datum_face_norm = pHuoQuJg->get_norm_y_wing();

				ray_line.endPt = SpreadFacePntToDatumFace(datum_face_norm,huoqu_line,
					spread_face_norm,ray_line.endPt);
				UCS_STRU inters_ucs;
				inters_ucs.axis_z = datum_face_norm.normalized();
				inters_ucs.axis_y = huoqu_vec.normalized();
				inters_ucs.axis_x = inters_ucs.axis_y^inters_ucs.axis_z;
				normalize(inters_ucs.axis_x);
				inters_ucs.axis_z = inters_ucs.axis_x^inters_ucs.axis_y;
				inters_ucs.origin = pStartFatherJg->GetDatumPosBer(pStart);
				coord_trans(huoqu_line.startPt,inters_ucs,FALSE);
				coord_trans(huoqu_line.endPt,inters_ucs,FALSE);
				coord_trans(ray_line.startPt,inters_ucs,FALSE);
				coord_trans(ray_line.endPt,inters_ucs,FALSE);
				//�޸�Ϊ�Ի�����Ϊ׼������Z����Ϊ��(�Դ����׼֫����ƫ��) WJH-2003.12.23
				ray_line.startPt.z = ray_line.endPt.z = huoqu_line.startPt.z = huoqu_line.endPt.z;
				Int3dpl(huoqu_line,ray_line,start_pos);
				coord_trans(start_pos,inters_ucs,TRUE);
			}
			else if(pOldDesStartPos->IsInOffsetZhunLine())	//���ڻ�����׼�Ǹ�ƫ��׼��
			{
				f3dPoint spread_vec = spread_face_norm^huoqu_vec;
				if(spread_vec*ray_vec<0)	//����
					spread_vec*=-1;
				normalize(spread_vec);
				if(huoqu_x_wing0_y_wing1==0)	//������׼�Ǹ�X֫Ϊ��׼��
					datum_face_norm = pHuoQuJg->get_norm_x_wing();
				else
					datum_face_norm = pHuoQuJg->get_norm_y_wing();
				
				SnapPerp(&start_pos,huoqu_line,pHuoQuJg->GetDatumPosBer(pStart));//pStart->GetInverseNodePos()); //modified by wjh 04.11.11
				start_pos +=spread_vec*pOldDesStartPos->offset_zhun;
				//����ڵ�������ӻ�׼�Ǹֳ��ȷ���ƫ�Ƶ�ƫ�Ʒ��� wht 10-12-09
				f3dPoint len_offset_vec=pStartFatherJg->End()-pStartFatherJg->Start();
				normalize(len_offset_vec);
				start_pos+=len_offset_vec*pOldDesStartPos->len_offset_dist;
			}
		}
		else //if(pOldDesStartPos->datum_pos_style==15)	//����ָ����׼��λ��
		{
			pOldDesStartPos->datum_point.UpdatePos(BelongModel());
			start_pos=pOldDesStartPos->datum_point.Position();
		}
		
		//---------------�ڶ���.���ݻ�׼����Ǹ����--------------
		//����ʵ�ʽǸ�������ֹ������
		if(pOldDesStartPos->datum_to_ber_style==1&&pOldDesStartPos->datum_pos_style==0)
		{	//�ӽڵ�ƽ�Ƽ���Ǹ����
			if(pOldDesStartPos->face_offset_norm.IsZero())
				return;
			f3dPoint wing_x_pos=GetSelfDatumPosWingXByFaceOffset(pStart,pOldDesStartPos->face_offset_norm);
			f3dPoint wing_y_pos=GetSelfDatumPosWingYByFaceOffset(pStart,pOldDesStartPos->face_offset_norm);
			start_pos=GetDatumPosBerByFaceOffset(pStart,pOldDesStartPos->face_offset_norm);
			f3dPoint wing_x_prj_vec=wing_x_pos-start_pos;
			f3dPoint wing_y_prj_vec=wing_y_pos-start_pos;
			f3dPoint start_ber=CalOffsetAngleBer(pStart->Position(true),wing_x_prj_vec,wing_y_prj_vec);
			SetStart(start_ber);
			if(pOldDesStartPos->cFaceOffsetDatumLine=='X'||pOldDesStartPos->cFaceOffsetDatumLine=='Y')
			{	//ƽ�ƻ�׼��Ϊ֫����
				f3dPoint wing_prj_vec,wing_pos;
				f3dPoint line_vec=Start()-End();
				normalize(line_vec);
				if(pOldDesStartPos->cFaceOffsetDatumLine=='X')	//ƽ�ƻ�׼��ΪX֫����
					wing_pos=GetSelfDatumPosWingXByFaceOffset(pStart,pOldDesStartPos->face_offset_norm);
				else //if(pOldDesStartPos->cFaceOffsetDatumLine=='Y')
					wing_pos=GetSelfDatumPosWingYByFaceOffset(pStart,pOldDesStartPos->face_offset_norm);
				SnapPerp(&start_ber,Start(),End(),wing_pos);
				SetStart(start_ber);
			}
			//Int3dlf(start_xyz,ber_line,start_pos,pOldDesStartPos->face_offset_norm);
		}
		else //���ݻ�׼��ͶӰ����Ǹ����
		{
			f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
			short work_wing_style=pOldDesStartPos->work_wing_auto0_x1_y2_both3;
			if(pOldDesStartPos->work_wing_auto0_x1_y2_both3==0)
			{	//�˻�������ʹ�ø��˼�(ָ����׼�˼�ʱ��������),�жϵ�ǰ����֫Ƿ��,��������������������и��� wjh-2012.4.28
				if(pStart->hFatherPart==handle||pOldDesStartPos->datum_pos_style==0)
				{	//��ʼ�ڵ㸸�Ǹ�Ϊ��ǰ�Ǹ֣���ʱƫ�Ʒ����ؽǸ���֫����
					if( pOldDesStartPos->offset_X_dist_style==4&&pOldDesStartPos->offset_X_dist==0&&(
						pOldDesStartPos->offset_Y_dist_style!=4||pOldDesStartPos->offset_Y_dist!=0))
						work_wing_style=1;	//X֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��X֫Ϊ��ǰ����֫
					else if(pOldDesStartPos->offset_Y_dist_style==4&&pOldDesStartPos->offset_Y_dist==0&&(
							pOldDesStartPos->offset_X_dist_style!=4||pOldDesStartPos->offset_X_dist!=0))
						work_wing_style=2;	//Y֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��Y֫Ϊ��ǰ����֫
					else
						work_wing_style=3;
				}
				else	//��ӽǸ�
				{	//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
					f3dPoint father_vec,cur_vec,work_norm;
					CSuperSmartPtr<CLDSPart> pFatherPart;
					pFatherPart=BelongModel()->FromPartHandle(pStart->hFatherPart);
					if(pFatherPart.IsHasPtr()&&pFatherPart->IsLinePart())
					{
						cur_vec = End()-Start();
						father_vec = pFatherPart.LinePartPointer()->End()-pFatherPart.LinePartPointer()->Start();
						normalize(cur_vec);
						normalize(father_vec);
						if(pFatherPart->GetClassTypeId()==CLS_LINEANGLE||pFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
						{
							if(pOldDesStartPos->IsInDatumJgWingX())
								work_norm=pFatherPart.LineAnglePointer()->get_norm_x_wing();
							else if(pOldDesStartPos->IsInDatumJgWingY())
								work_norm=pFatherPart.LineAnglePointer()->get_norm_y_wing();
							else
								work_norm=father_vec^cur_vec;
						}
						else if(fabs(father_vec*cur_vec)<EPS_COS2)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
							work_norm = father_vec^cur_vec;
					}
					if(normalize(work_norm)!=0)
					{
						if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
							work_wing_style=1;
						else
							work_wing_style=2;
					}
					else
						work_wing_style=3;
				}
			}
			if(work_wing_style<3)
			{	//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
				if(work_wing_style==1)
				{
					offset_vec_x = get_norm_x_wing();
					offset_vec_y = -GetWingVecX();
				}
				else
				{
					offset_vec_x = -GetWingVecY();
					offset_vec_y = get_norm_y_wing();
				}
				if(pOldDesStartPos->offset_X_dist_style==0)
					SetStart(start_pos+offset_vec_x*xWingYZhunJu.g);
				else if(pOldDesStartPos->offset_X_dist_style==1)
					SetStart(start_pos+offset_vec_x*xWingYZhunJu.g1);
				else if(pOldDesStartPos->offset_X_dist_style==2)
					SetStart(start_pos+offset_vec_x*xWingYZhunJu.g2);
				else if(pOldDesStartPos->offset_X_dist_style==3)
					SetStart(start_pos+offset_vec_x*xWingYZhunJu.g3);
				else if(pOldDesStartPos->offset_X_dist_style==4)
					SetStart(start_pos+offset_vec_x*pOldDesStartPos->offset_X_dist);
				if(pOldDesStartPos->offset_Y_dist_style==0)
					SetStart(Start()+offset_vec_y*xWingXZhunJu.g);
				else if(pOldDesStartPos->offset_Y_dist_style==1)
					SetStart(Start()+offset_vec_y*xWingXZhunJu.g1);
				else if(pOldDesStartPos->offset_Y_dist_style==2)
					SetStart(Start()+offset_vec_y*xWingXZhunJu.g2);
				else if(pOldDesStartPos->offset_Y_dist_style==3)
					SetStart(Start()+offset_vec_y*xWingXZhunJu.g3);
				else if(pOldDesStartPos->offset_Y_dist_style==4)
					SetStart(Start()+offset_vec_y*pOldDesStartPos->offset_Y_dist);
			}
			else
			{
				double a,b,c,alfa;
				if(pOldDesStartPos->offset_X_dist_style==0)
					a = xWingYZhunJu.g;
				else if(pOldDesStartPos->offset_X_dist_style==1)
					a = xWingYZhunJu.g1;
				else if(pOldDesStartPos->offset_X_dist_style==2)
					a = xWingYZhunJu.g2;
				else if(pOldDesStartPos->offset_X_dist_style==3)
					a = xWingYZhunJu.g3;
				else if(pOldDesStartPos->offset_X_dist_style==4)
					a = pOldDesStartPos->offset_X_dist;
				if(pOldDesStartPos->offset_Y_dist_style==0)
					b = xWingXZhunJu.g;
				else if(pOldDesStartPos->offset_Y_dist_style==1)
					b = xWingXZhunJu.g1;
				else if(pOldDesStartPos->offset_Y_dist_style==2)
					b = xWingXZhunJu.g2;
				else if(pOldDesStartPos->offset_Y_dist_style==3)
					b = xWingXZhunJu.g3;
				else if(pOldDesStartPos->offset_Y_dist_style==4)
					b = pOldDesStartPos->offset_Y_dist;
				alfa = GetRadWingAngle();
				/*c = sqrt((b*b-2*a*b*cos(alfa)+a*a*cos(alfa)*cos(alfa))/(sin(alfa)*sin(alfa))+a*a-b*b);
				if(a>0)
				start_xyz = start_pos+norm_y_wing*b-GetWingVecY()*c;
				else
				start_xyz = start_pos+norm_y_wing*b+GetWingVecY()*c;*/
				sina=sin(alfa);
				if(fabs(sina)<EPS)
					sina=1;		//�Է����ֳ�0
				c = (b-a*cos(alfa))/sina;
				SetStart(start_pos-GetWingVecY()*a+get_norm_y_wing()*c);
			}
			if(pFatherLinePart.IsHasPtr()&&(pOldDesStartPos->datum_pos_style==1
				||pOldDesStartPos->datum_pos_style==4)&&pFatherLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//���Ǹִ���ڸֹ���ʱʹ��ƫ�ľ�������Ǹֲ����ڸֹ�������(��������1/4������)����� wht 09-10-17
				f3dLine datumLine(pFatherLinePart->Start(),pFatherLinePart->End());
				f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
				f3dPoint cur_vec=End()-Start();
				f3dPoint plane_norm=datum_vec^cur_vec;
				f3dPoint eccentricOffsetVec=datum_vec^plane_norm;
				normalize(eccentricOffsetVec);
				if(eccentricOffsetVec*cur_vec<EPS)
					eccentricOffsetVec*=-1.0;
				f3dPoint start=Start();
				if(fabs(pOldDesStartPos->fEccentricDist)>EPS)
				{	//ƫ�ľ�
					if(pOldDesStartPos->datum_pos_style==1) //ͶӰ
						start+=eccentricOffsetVec*pOldDesStartPos->fEccentricDist;
					else //if(pOldDesStartPos->datum_pos_style==4) //ƽ��
					{	
						datumLine.startPt+=eccentricOffsetVec*pOldDesStartPos->fEccentricDist;
						datumLine.endPt+=eccentricOffsetVec*pOldDesStartPos->fEccentricDist;
						if(pOldDesStartPos->face_offset_norm.IsZero())
							start+=eccentricOffsetVec*pOldDesStartPos->fEccentricDist;
						else
							Int3dlf(start,datumLine,start,pOldDesStartPos->face_offset_norm);
					}
				}
				SetStart(start);
			}
		}
	}
	else if(pOldDesStartPos->jgber_cal_style==1||pOldDesStartPos->jgber_cal_style==2)
	{	//�ڵ���ֱ��ͶӰ��ƽ��
		CSmartPtr<CLDSLineAngle>pStartJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesStartPos->start_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CSmartPtr<CLDSLineAngle>pEndJg  =(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesStartPos->end_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pStartJg.IsNULL()||pEndJg.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"0X%X angle miss position for datum angle",handle);
#else
			sprintf(sError,"0X%X�Ǹֶ�ʧλ�û�׼�Ǹ�",handle);
#endif
			throw sError;
		}
		f3dPoint start,end,cur;
		if(pOldDesStartPos->iStartJgBerStyle==0)
		{	//��ʼ�˽Ǹֵ�ʼ���Ϊ��׼���
			pStartJg->cal_start_ber_pos();
			start = pStartJg->Start();
		}
		else
		{	//��ʼ�˽Ǹֵ������Ϊ��׼���
			pStartJg->cal_end_ber_pos();
			start = pStartJg->End();
		}
		double alfa=Pi-GetRadWingAngle();
		sina=sin(alfa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		double w_d=(pOldDesStartPos->fStartJgLengOffsetY-pOldDesStartPos->fStartJgLengOffsetX*cos(alfa))/sina;
		start+=get_norm_x_wing()*pOldDesStartPos->fStartJgLengOffsetX-GetWingVecX()*w_d;

		if(pOldDesStartPos->iEndJgBerStyle==0)
		{
			pEndJg->cal_start_ber_pos();
			end = pEndJg->Start();
		}
		else
		{
			pEndJg->cal_end_ber_pos();
			end   = pEndJg->End();
		}
		alfa=Pi-GetRadWingAngle();
		sina=sin(alfa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		w_d=(pOldDesStartPos->fEndJgLengOffsetY-pOldDesStartPos->fEndJgLengOffsetX*cos(alfa))/sina;
		end+=get_norm_x_wing()*pOldDesStartPos->fEndJgLengOffsetX-GetWingVecX()*w_d;

		f3dPoint face_pick=pStart->Position(true),start_xyz=Start();
		if(pStart->m_cPosCalType==4)	//���Ǹֽ��潻��
		{
			pStart->GetIntersZhun(&face_pick);
			SnapPerp(&start_xyz,start,end,face_pick);
			SetStart(start_xyz);
		}
		else if(pStart->m_cPosCalType==7&&	//��ͨ�ظ˼����ȷ���ƫ�ƵĽڵ�
			(pStart->m_cPosCalViceType!='X'&&pStart->m_cPosCalViceType!='Y'&&pStart->m_cPosCalViceType!='Z'))
		{	//GetDatumPosBer(pDatumNode)�õ��ĵ��λ�ã������ɽڵ���˼�ͶӰ����õ��ģ�����׼�ڵ�ĸ��˼���λ��ʽΪƽ�ƣ�
			//�����׼�ڵ㸸�˼������˼���(����ƫ�ƽڵ��)һ�ˣ���λ��ʽ�޸�Ϊƽ�Ʋ�������  wht 09-07-17	
			CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(pStart->arrRelationNode[0]);
			//ƫ�ƽڵ��׼�ڵ㸸�˼�Ϊ��ǰ�˼�ʱ��(pDatumNode->hFatherPart==handle)
			//���ݻ�׼�ڵ��Լ�ƫ�ƾ��������ƫ�ƽڵ�Ϊ�˵�ĸ˼���ʼ��λ�� wht 10-10-19
			CLDSLineAngle *pDatumFatherJg=GetStartNodeOffsetDatumAngle();
			if(pDatumFatherJg)
			{	//�����׼�ڵ㸸�˼���λ��ʽΪƽ��ʱ����GetDatumPosBerByFaceOffset() wht 10-07-30
				f3dPoint pos=pDatumFatherJg->GetDatumPosBer(pDatumNode);
				if(pDatumFatherJg->pOldDesStartPos->IsFaceOffset()||pDatumFatherJg->pOldDesEndPos->IsFaceOffset())
				{	//��׼�ڵ㶨λ��ʽΪƽ��
					f3dPoint pt=pos;
					f3dPoint face_norm=pDatumFatherJg->pOldDesStartPos->face_offset_norm;
					pos=pDatumFatherJg->GetDatumPosBerByFaceOffset(pDatumNode,face_norm,&pt);	
				}
				f3dPoint line_vec=End()-Start();
				normalize(line_vec);
				//������ƫ����ͶӰ�������޷�ͬʱ��֤ƫ������ͶӰֵ
				start_xyz=pos+line_vec*pStart->attach_offset;
				SnapPerp(&start_xyz,start,end,start_xyz);
				SetStart(start_xyz);
			}
		}
		else
		{
			if(pOldDesStartPos->jgber_cal_style==1)	//ͶӰ
				SnapPerp(&start_xyz,start,end,pStart->Position(true));
			else //if(pOldDesStartPos->jgber_cal_style==2)	//ƽ��
			{
				Int3dlf(start_xyz,f3dLine(start,end),face_pick,pOldDesStartPos->face_offset_norm);
				if(pOldDesStartPos->cFaceOffsetDatumLine=='X'||pOldDesStartPos->cFaceOffsetDatumLine=='Y')
				{	//ƽ�ƻ�׼��Ϊ֫����
					SetStart(start_xyz);
					f3dPoint wing_prj_vec,wing_pos;
					f3dPoint line_vec=start_xyz-End();
					normalize(line_vec);
					if(pOldDesStartPos->cFaceOffsetDatumLine=='X')	//ƽ�ƻ�׼��ΪX֫����
						wing_pos=GetSelfDatumPosWingXByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,&face_pick);
					else
						wing_pos=GetSelfDatumPosWingYByFaceOffset(pStart,pOldDesStartPos->face_offset_norm,&face_pick);
					SnapPerp(&start_xyz,Start(),End(),wing_pos);
				}
			}
			SetStart(start_xyz);
		}
	}
	else if(pOldDesStartPos->jgber_cal_style==3)	//ָ������
		return;
}
void CLDSLineAngle::cal_end_ber_pos()
{
	JGZJ jgzj;
	char sError[200];
	double sina;
	if(IsEndPosLocked())
		return;			//�ն�λ���ѱ������������ǻ����㣩
	if(BelongModel()==NULL)
		return;
	if(pEnd==NULL)		//�Ҳ����ն˵�
	{
		pOldDesEndPos->datum_point.UpdatePos(BelongModel());
		SetEnd(pOldDesEndPos->datum_point.Position());
		SetModified();
		return;
	}
	if(pEnd->hFatherPart<0x20)
		return;
	if(!LockEnd())
		return;	//�ѵ�λ
	SetModified();
	if(!getjgzj(jgzj,size_wide))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		sprintf(sError,"The specifications %.0fX%.0f of angle can't find in stadia library,failed calculate position!",size_wide,size_thick);
#else
		sprintf(sError,"%.0fX%.0f���ĽǸ���׼������Ҳ���,λ�ü���ʧ��!",size_wide,size_thick);
#endif
		throw sError;
	}
	if(pOldDesEndPos->jgber_cal_style==0)
	{	//����ƫ��
		CSuperSmartPtr<CLDSLinePart>pFatherLinePart=GetEndDatumPart();
		f3dPoint end_pos;	//�Ǹ���ʼ��ֹλ�õļ����׼��
		//---------------��һ��.���׼��--------------
		if(pOldDesEndPos->datum_pos_style==0)	//�ڵ�Ϊ��׼��
			end_pos=pEnd->Position(true);
		else if(pOldDesEndPos->datum_pos_style<7)	//Ԥ��׼��
		{
			f3dPoint datum_start,datum_end,datum_point;
			if(pFatherLinePart.IsNULL())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Can't find the datum angle which 0X%X angle's end datum point positions!",handle);
#else
				sprintf(sError,"�Ҳ����Ǹ�0X%X�ն˻��㶨λ��׼�Ǹ�!",handle);
#endif
				throw sError;
			}
			if(pFatherLinePart.IsEqualPtr(this))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Datum point positions datum angle,angle itself,the design parameter of position is wrong,please redesign 0X%X angle's start position!",handle);
#else
				sprintf(sError,"���㶨λ��׼�Ǹּ��Ǹֱ���,λ����Ʋ�������,���������0X%X�Ǹ��ն�λ��!",handle);
#endif
				throw sError;;
			}
			//�����������������ѭ������֪�������ں��ֿ��ǲ�������˼�������ʶ wjh 2010-10-18
			//pFatherLinePart->ClearFlag();
			pFatherLinePart->CalPosition();	//���Ǹֵ�λ
			if(pFatherLinePart->GetClassTypeId()==CLS_LINETUBE)
				SnapPerp(&end_pos,pFatherLinePart->Start(),pFatherLinePart->End(),pEnd->Position(true));
			else if(pFatherLinePart->GetClassTypeId()==CLS_LINEANGLE||pFatherLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				if(pOldDesEndPos->datum_pos_style==1)	//�Ը��Ǹ�����Ϊ��׼
				{
					//������´������Ľ��������Ķ˵�����ͶӰʱ������ֱ��
					//����GetDatumPosBer����Ϊ��ʱ���迼�ǹ̶�����ͷӰ��
					if(pEnd==pFatherLinePart->pStart)
						end_pos=pFatherLinePart->Start();
					else if(pEnd==pFatherLinePart->pEnd)
						end_pos=pFatherLinePart->End();
					else
						end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosBer(pEnd);
					if(pFatherLinePart.LineAnglePointer()->IsReservedHeJiao())
					{	//�����б���֫��ȵ�����Ͻ�
						f3dPoint len_offset_vec=pFatherLinePart->End()-pFatherLinePart->Start();
						f3dPoint vec=End()-Start();
						f3dPoint work_norm=vec^len_offset_vec;
						normalize(work_norm);
						if( fabs(work_norm*pFatherLinePart.LineAnglePointer()->get_norm_x_wing())>
							fabs(work_norm*pFatherLinePart.LineAnglePointer()->get_norm_y_wing()))
							end_pos+=pFatherLinePart.LineAnglePointer()->CalWingOffsetVec('X',0);
						else
							end_pos+=pFatherLinePart.LineAnglePointer()->CalWingOffsetVec('Y',0);
					}
				}
				else if(pOldDesEndPos->datum_pos_style==2)	//�Ը��Ǹ�X֧Ϊ��׼
					end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingX(pEnd);
				else if(pOldDesEndPos->datum_pos_style==3)	//�Ը��Ǹ�Y֧Ϊ��׼
					end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingY(pEnd);
				else if(pOldDesEndPos->datum_pos_style==4)	//��ӵ�Ϊƽ�Ƽ���
				{	//�Ը��Ǹ�����Ϊ��׼ƽ��
					char cDatumWing=0;
					if(pFatherLinePart.LineAnglePointer()->IsReservedHeJiao())
					{	//�����б���֫��ȵ�����Ͻ�
						f3dPoint len_offset_vec=pFatherLinePart->End()-pFatherLinePart->Start();
						f3dPoint vec=End()-Start();
						f3dPoint work_norm=vec^len_offset_vec;
						normalize(work_norm);
						if( fabs(work_norm*pFatherLinePart.LineAnglePointer()->get_norm_x_wing())>
							fabs(work_norm*pFatherLinePart.LineAnglePointer()->get_norm_y_wing()))
							cDatumWing='X';
						else
							cDatumWing='Y';
					}
					if(pOldDesEndPos->bByOtherEndOffset)
					{
						f3dPoint pt = GetStartDatumPos();
						end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,&pt,cDatumWing);
					}
					else
						end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosBerByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,NULL,cDatumWing);
				}
				else if(pOldDesEndPos->datum_pos_style==5)	//��ӵ�Ϊƽ�Ƽ���
				{
					if(pOldDesEndPos->bByOtherEndOffset)
					{
						f3dPoint pt = GetStartDatumPos();
						end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,&pt);
					}
					else
						end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm);
				}
				else if(pOldDesEndPos->datum_pos_style==6)	//�Ը��Ǹ�Y֧Ϊ��׼
				{
					if(pOldDesEndPos->bByOtherEndOffset)
					{
						f3dPoint pt = GetStartDatumPos();
						end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,&pt);
					}
					else
						end_pos = pFatherLinePart.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm);
				}
			}
			//����Ǹ��ն˴�ӻ�׼�˼�(�Ǹֻ�ֹ�)�ĳ��ȷ��򣬿��ǻ������췽��ƫ���� wht 11-05-19
			f3dPoint len_offset_vec=pFatherLinePart->End()-pFatherLinePart->Start();
			normalize(len_offset_vec);
			end_pos+=pOldDesEndPos->len_offset_dist*len_offset_vec;
		}
		else if(pOldDesEndPos->datum_pos_style<15)
		{	//1�����߻�2ƫ��׼��
			f3dPoint norm;
			CSmartPtr<CLDSLineAngle>pEndFatherJg,pHuoQuJg;
			CLDSLinePart *pPart=GetEndDatumPart();
			if(pPart&&(pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE))
				pEndFatherJg = (CLDSLineAngle*)pPart;
			if(pEndFatherJg.IsNULL())
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Can't find the datum angle which lap with angle's end!";
#else
				throw "�Ҳ����Ǹ��ն˴��λ�û�׼�Ǹ�!";
#endif
			else if(pEndFatherJg.IsEqualPtr(this))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"Datum point positions datum angle,angle itself,the design parameter of position is wrong,please redesign 0X%X angle's start position!",handle);
#else
				sprintf(sError,"���㶨λ��׼�Ǹּ��Ǹֱ���,λ����Ʋ�������,���������0X%X�Ǹ��ն�λ��!",handle);
#endif
				throw sError;
			}
			if(pOldDesEndPos->huoqu_jg_h>0x20)
				pHuoQuJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesEndPos->huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pHuoQuJg.IsNULL())
				pHuoQuJg=pEndFatherJg;
			int huoqu_x_wing0_y_wing1;
			if(pOldDesEndPos->huoqu_pos_auto_0_x_wing1_y_wing2==1)
				huoqu_x_wing0_y_wing1 = 0;
			else if(pOldDesEndPos->huoqu_pos_auto_0_x_wing1_y_wing2==2)
				huoqu_x_wing0_y_wing1 = 1;
			else if(pEndFatherJg==pHuoQuJg&&pOldDesEndPos->IsInDatumJgWingX())
				huoqu_x_wing0_y_wing1 = 0;
			else if(pEndFatherJg==pHuoQuJg&&pOldDesEndPos->IsInDatumJgWingY())
				huoqu_x_wing0_y_wing1 = 1;
			else if(pOldDesEndPos->IsInDatumJgWingX())
			{
				norm = pEndFatherJg->get_norm_x_wing();
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			else if(pOldDesEndPos->IsInDatumJgWingY())
			{
				norm = pEndFatherJg->get_norm_y_wing();
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			else
			{
				f3dPoint huoqu_vec,jg_vec;
				huoqu_vec = pHuoQuJg->pEnd->GetInverseNodePos(pHuoQuJg)-pHuoQuJg->pStart->GetInverseNodePos(pHuoQuJg);
				jg_vec = pEnd->GetInverseNodePos()-pStart->GetInverseNodePos();
				norm = huoqu_vec^jg_vec;
				normalize(norm);
				CLDSLineAngle::IsInsideAngle(pHuoQuJg,norm,&huoqu_x_wing0_y_wing1);
			}
			
			BOOL bBerHuoQu = IsBerHuoQuJg(pHuoQuJg,Start()-End(),huoqu_x_wing0_y_wing1);
			f3dPoint huoqu_wing_vec,datum_face_norm,spread_face_norm;
			f3dLine huoqu_line,ray_line;
			ray_line.endPt = GetEndDatumPos();
			//���¼��и�Ϊֱ���ɽǸ����߷���ȷ�����߷��򣬵�û�о���ʵ�����飬��֪�Բ���
			//��Ҫ���� WJH-2005.03.07
			//Ϊ�˸������߲�����һ������ WJH-2004.05.31
			//if(pStart->hFatherPart==handle)//&&pOldDesStartPos->offset_style!=1)
			//{	//�Ǹֶ˴��ڻ�׼λ��,����һ���Ҳ����,��֪���ڻ����÷� WJH-2005.03.03
				ray_line.startPt=Start()-End()+ray_line.endPt;
			//}
			//else
			//	ray_line.startPt = GetStartDatumPos();
			if(huoqu_x_wing0_y_wing1==0)
			{
				huoqu_wing_vec = pHuoQuJg->GetWingVecX();
				//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
				ray_line.endPt+= pHuoQuJg->get_norm_x_wing()*pOldDesEndPos->datum_offset_dist;
			}
			else
			{
				huoqu_wing_vec = pHuoQuJg->GetWingVecY();
				//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
				ray_line.endPt+= pHuoQuJg->get_norm_y_wing()*pOldDesEndPos->datum_offset_dist;
			}

			huoqu_line.startPt = pHuoQuJg->Start();
			huoqu_line.endPt   = pHuoQuJg->End();
			if(bBerHuoQu)
			{
				huoqu_line.startPt = huoqu_line.startPt-huoqu_wing_vec*pOldDesEndPos->huoqu_dist;
				huoqu_line.endPt = huoqu_line.endPt-huoqu_wing_vec*pOldDesEndPos->huoqu_dist;
			}
			else
			{
				huoqu_line.startPt = huoqu_line.startPt+
					huoqu_wing_vec*(pHuoQuJg->GetWidth()+pOldDesEndPos->huoqu_dist);
				huoqu_line.endPt = huoqu_line.endPt+
					huoqu_wing_vec*(pHuoQuJg->GetWidth()+pOldDesEndPos->huoqu_dist);
			}
			//�¼����Դ���������ػ�����׼�Ǹֵ�ǰ(���)֫����ƫ�Ƶ����� WJH-2003/12/18
			if(huoqu_x_wing0_y_wing1==0)
			{
				huoqu_line.startPt+= pHuoQuJg->get_norm_x_wing()*pOldDesEndPos->datum_offset_dist;
				huoqu_line.endPt+= pHuoQuJg->get_norm_x_wing()*pOldDesEndPos->datum_offset_dist;
			}
			else
			{
				huoqu_line.startPt+= pHuoQuJg->get_norm_y_wing()*pOldDesEndPos->datum_offset_dist;
				huoqu_line.endPt+= pHuoQuJg->get_norm_y_wing()*pOldDesEndPos->datum_offset_dist;
			}
			
			f3dPoint huoqu_vec = huoqu_line.endPt-huoqu_line.startPt;
			f3dPoint ray_vec = ray_line.startPt-ray_line.endPt;
			spread_face_norm = huoqu_vec^ray_vec;
			if(pOldDesEndPos->IsInHuoQuLine())	//���ڻ�����׼�Ǹֻ�����
			{
				if(huoqu_x_wing0_y_wing1==0)	//������׼�Ǹ�X֫Ϊ��׼��
					datum_face_norm = pHuoQuJg->get_norm_x_wing();
				else
					datum_face_norm = pHuoQuJg->get_norm_y_wing();
				
				ray_line.endPt = SpreadFacePntToDatumFace(datum_face_norm,huoqu_line,
					spread_face_norm,ray_line.endPt);
				UCS_STRU inters_ucs;
				inters_ucs.axis_z = datum_face_norm.normalized();
				inters_ucs.axis_y = huoqu_vec.normalized();
				inters_ucs.axis_x = inters_ucs.axis_y^inters_ucs.axis_z;
				normalize(inters_ucs.axis_x);
				inters_ucs.axis_z = inters_ucs.axis_x^inters_ucs.axis_y;
				inters_ucs.origin = pEndFatherJg->GetDatumPosBer(pEnd);
				coord_trans(huoqu_line.startPt,inters_ucs,FALSE);
				coord_trans(huoqu_line.endPt,inters_ucs,FALSE);
				coord_trans(ray_line.startPt,inters_ucs,FALSE);
				coord_trans(ray_line.endPt,inters_ucs,FALSE);
				//�޸�Ϊ�Ի�����Ϊ׼������Z����Ϊ��(�Դ����׼֫����ƫ��) WJH-2003.12.23
				ray_line.startPt.z = ray_line.endPt.z = huoqu_line.startPt.z = huoqu_line.endPt.z;
				Int3dpl(huoqu_line,ray_line,end_pos);
				coord_trans(end_pos,inters_ucs,TRUE);
			}
			else if(pOldDesEndPos->IsInOffsetZhunLine())	//���ڻ�����׼�Ǹ�ƫ��׼��
			{
				f3dPoint spread_vec = spread_face_norm^huoqu_vec;
				if(spread_vec*ray_vec<0)	//����
					spread_vec*=-1;
				normalize(spread_vec);
				if(huoqu_x_wing0_y_wing1==0)	//������׼�Ǹ�X֫Ϊ��׼��
					datum_face_norm = pHuoQuJg->get_norm_x_wing();
				else
					datum_face_norm = pHuoQuJg->get_norm_y_wing();
				
				SnapPerp(&end_pos,huoqu_line,pHuoQuJg->GetDatumPosBer(pEnd));//pEnd->GetJgBerPrjPos());	//modified by wjh 04.11.11
				end_pos +=spread_vec*pOldDesEndPos->offset_zhun;
				//����ڵ�������ӻ�׼�Ǹֳ��ȷ���ƫ�Ƶ�ƫ�Ʒ��� wht 10-12-09
				f3dPoint len_offset_vec=pEndFatherJg->End()-pEndFatherJg->Start();
				normalize(len_offset_vec);
				end_pos+=len_offset_vec*pOldDesEndPos->len_offset_dist;
			}
		}
		else //if(pOldDesEndPos->datum_pos_style==15)	//ָ����׼��λ��
		{
			pOldDesEndPos->datum_point.UpdatePos(BelongModel());
			end_pos=pOldDesEndPos->datum_point.Position();
		}
		
		//---------------�ڶ���.���ݻ�׼����Ǹ����--------------
		//����ʵ�ʽǸ�������ֹ������
		if(pOldDesEndPos->datum_to_ber_style==1&&pOldDesEndPos->datum_pos_style==0)
		{	//�ӽڵ�ƽ�Ƽ���Ǹ����
			if(pOldDesEndPos->face_offset_norm.IsZero())
				return;
			f3dPoint wing_x_pos=GetSelfDatumPosWingXByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm);
			f3dPoint wing_y_pos=GetSelfDatumPosWingYByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm);
			end_pos=GetDatumPosBerByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm);
			f3dPoint wing_x_prj_vec=wing_x_pos-end_pos;
			f3dPoint wing_y_prj_vec=wing_y_pos-end_pos;
			f3dPoint end_ber=CalOffsetAngleBer(pEnd->Position(true),wing_x_prj_vec,wing_y_prj_vec);
			SetEnd(end_ber);
			if(pOldDesEndPos->cFaceOffsetDatumLine=='X'||pOldDesEndPos->cFaceOffsetDatumLine=='Y')
			{	//ƽ�ƻ�׼��Ϊ֫����
				f3dPoint wing_prj_vec,wing_pos;
				f3dPoint line_vec=End()-Start();
				normalize(line_vec);
				if(pOldDesEndPos->cFaceOffsetDatumLine=='X')	//ƽ�ƻ�׼��ΪX֫����
					wing_pos=GetSelfDatumPosWingXByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm);
				else
					wing_pos=GetSelfDatumPosWingYByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm);
				SnapPerp(&end_ber,Start(),End(),wing_pos);
				SetEnd(end_ber);
			}
		}
		else	//�ӻ�׼�㵽�Ǹ����ΪͶӰ����
		{
			f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
			short work_wing_style=pOldDesEndPos->work_wing_auto0_x1_y2_both3;
			if(pOldDesEndPos->work_wing_auto0_x1_y2_both3==0)
			{	//�˻�������ʹ�ø��˼�(ָ����׼�˼�ʱ��������),�жϵ�ǰ����֫Ƿ��,��������������������и��� wjh-2012.4.28
				if(pEnd->hFatherPart==handle||pOldDesEndPos->datum_pos_style==0)
				{	//��ֹ�ڵ㸸�Ǹ�Ϊ��ǰ�Ǹ֣���ʱƫ�Ʒ����ؽǸ���֫����
					if( pOldDesEndPos->offset_X_dist_style==4&&pOldDesEndPos->offset_X_dist==0&&(
						pOldDesEndPos->offset_Y_dist_style!=4||pOldDesEndPos->offset_Y_dist!=0))
						work_wing_style=1;	//X֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��X֫Ϊ��ǰ����֫
					else if(pOldDesEndPos->offset_Y_dist_style==4&&pOldDesEndPos->offset_Y_dist==0&&(
							pOldDesEndPos->offset_X_dist_style!=4||pOldDesEndPos->offset_X_dist!=0))
						work_wing_style=2;	//Y֫����ƫ��Ϊ0,Y֫����ƫ������Ϊ0,��Y֫Ϊ��ǰ����֫
					else
						work_wing_style=3;
				}
				else
				{
					f3dPoint father_vec,cur_vec,work_norm;
					CSuperSmartPtr<CLDSPart>pFatherPart;
					pFatherPart=BelongModel()->FromPartHandle(pEnd->hFatherPart);
					cur_vec = End()-Start();
					if(pFatherPart.IsHasPtr()&&pFatherPart->IsLinePart())
					{
						father_vec = pFatherPart.LinePartPointer()->End()-pFatherPart.LinePartPointer()->Start();
						normalize(father_vec);
						normalize(cur_vec);
						if(pFatherPart->GetClassTypeId()==CLS_LINEANGLE||pFatherPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
						{
							if(pOldDesEndPos->IsInDatumJgWingX())
								work_norm=pFatherPart.LineAnglePointer()->get_norm_x_wing();
							else if(pOldDesEndPos->IsInDatumJgWingY())
								work_norm=pFatherPart.LineAnglePointer()->get_norm_y_wing();
							else
								work_norm=father_vec^cur_vec;
						}
						else if(fabs(father_vec*cur_vec)<EPS_COS2)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
							work_norm = father_vec^cur_vec;
					}
					if(normalize(work_norm)!=0)
					{
						if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
							work_wing_style=1;
						else
							work_wing_style=2;
					}
					else
						work_wing_style=3;
				}
			}
			if(work_wing_style<3)
			{	//��ʼ�ڵ㸸�Ǹ�Ϊ��ǰ�Ǹ֣���ʱƫ�Ʒ����ؽǸ���֫����
				if(work_wing_style==1)	
				{	//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
					offset_vec_x = get_norm_x_wing();
					offset_vec_y = -GetWingVecX();
				}
				else
				{
					offset_vec_x = -GetWingVecY();
					offset_vec_y = get_norm_y_wing();
				}
				if(pOldDesEndPos->offset_X_dist_style==0)
					SetEnd(end_pos+offset_vec_x*xWingYZhunJu.g);
				else if(pOldDesEndPos->offset_X_dist_style==1)
					SetEnd(end_pos+offset_vec_x*xWingYZhunJu.g1);
				else if(pOldDesEndPos->offset_X_dist_style==2)
					SetEnd(end_pos+offset_vec_x*xWingYZhunJu.g2);
				else if(pOldDesEndPos->offset_X_dist_style==3)
					SetEnd(end_pos+offset_vec_x*xWingYZhunJu.g3);
				else if(pOldDesEndPos->offset_X_dist_style==4)
					SetEnd(end_pos+offset_vec_x*pOldDesEndPos->offset_X_dist);
				if(pOldDesEndPos->offset_Y_dist_style==0)
					SetEnd(End()+offset_vec_y*xWingXZhunJu.g);
				else if(pOldDesEndPos->offset_Y_dist_style==1)
					SetEnd(End()+offset_vec_y*xWingXZhunJu.g1);
				else if(pOldDesEndPos->offset_Y_dist_style==2)
					SetEnd(End()+offset_vec_y*xWingXZhunJu.g2);
				else if(pOldDesEndPos->offset_Y_dist_style==3)
					SetEnd(End()+offset_vec_y*xWingXZhunJu.g3);
				else if(pOldDesEndPos->offset_Y_dist_style==4)
					SetEnd(End()+offset_vec_y*pOldDesEndPos->offset_Y_dist);
			}
			else
			{
				double a,b,c,alfa;
				if(pOldDesEndPos->offset_X_dist_style==0)
					a = xWingYZhunJu.g;
				else if(pOldDesEndPos->offset_X_dist_style==1)
					a = xWingYZhunJu.g1;
				else if(pOldDesEndPos->offset_X_dist_style==2)
					a = xWingYZhunJu.g2;
				else if(pOldDesEndPos->offset_X_dist_style==3)
					a = xWingYZhunJu.g3;
				else if(pOldDesEndPos->offset_X_dist_style==4)
					a = pOldDesEndPos->offset_X_dist;
				if(pOldDesEndPos->offset_Y_dist_style==0)
					b = xWingXZhunJu.g;
				else if(pOldDesEndPos->offset_Y_dist_style==1)
					b = xWingXZhunJu.g1;
				else if(pOldDesEndPos->offset_Y_dist_style==2)
					b = xWingXZhunJu.g2;
				else if(pOldDesEndPos->offset_Y_dist_style==3)
					b = xWingXZhunJu.g3;
				else if(pOldDesEndPos->offset_Y_dist_style==4)
					b = pOldDesEndPos->offset_Y_dist;
				alfa = GetRadWingAngle();
				//c = sqrt((b*b-2*a*b*cos(alfa)+a*a*cos(alfa)*cos(alfa))/(sin(alfa)*sin(alfa))+a*a-b*b);
				/*if(a>0)
				end_xyz = end_pos+norm_y_wing*b-GetWingVecY()*c;
				else
				end_xyz = end_pos+norm_y_wing*b+GetWingVecY()*c;*/
				sina=sin(alfa);
				if(fabs(sina)<EPS)
					sina=1;		//�Է����ֳ�0
				c = (b-a*cos(alfa))/sina;
				//if(b>0)
				SetEnd(end_pos-GetWingVecY()*a+get_norm_y_wing()*c);
				//else
				//	end_xyz = end_pos+GetWingVecY()*b+norm_y_wing*c;
			}
			if(pFatherLinePart.IsHasPtr()&&(pOldDesEndPos->datum_pos_style==1
				||pOldDesEndPos->datum_pos_style==4)&&pFatherLinePart->GetClassTypeId()==CLS_LINETUBE)
			{	//���Ǹִ���ڸֹ���ʱʹ��ƫ�ľ�������Ǹֲ����ڸֹ�������(��������1/4������)����� wht 09-10-17
				f3dLine datumLine(pFatherLinePart->Start(),pFatherLinePart->End());
				f3dPoint datum_vec=datumLine.endPt-datumLine.startPt;
				f3dPoint cur_vec=Start()-End();
				f3dPoint plane_norm=datum_vec^cur_vec;
				f3dPoint eccentricOffsetVec=datum_vec^plane_norm;
				normalize(eccentricOffsetVec);
				if(eccentricOffsetVec*cur_vec<EPS)
					eccentricOffsetVec*=-1.0;
				f3dPoint end=End();
				if(fabs(pOldDesEndPos->fEccentricDist)>EPS)
				{	//ƫ�ľ�
					if(pOldDesEndPos->datum_pos_style==1) //ͶӰ
						end+=eccentricOffsetVec*pOldDesEndPos->fEccentricDist;
					else //if(pOldDesEndPos->datum_pos_style==4) //ƽ��
					{	
						datumLine.startPt+=eccentricOffsetVec*pOldDesEndPos->fEccentricDist;
						datumLine.endPt+=eccentricOffsetVec*pOldDesEndPos->fEccentricDist;
						if(pOldDesEndPos->face_offset_norm.IsZero())
							end+=eccentricOffsetVec*pOldDesEndPos->fEccentricDist;
						else
							Int3dlf(end,datumLine,end,pOldDesEndPos->face_offset_norm);
					}
				}
				SetEnd(end);
			}
		}
	}
	else if(pOldDesEndPos->jgber_cal_style==1||pOldDesEndPos->jgber_cal_style==2)
	{	//����ֱ��ͶӰ��ƽ��
		CSmartPtr<CLDSLineAngle>pStartJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesEndPos->start_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CSmartPtr<CLDSLineAngle>pEndJg  =(CLDSLineAngle*)BelongModel()->FromPartHandle(pOldDesEndPos->end_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pStartJg.IsNULL()||pEndJg.IsNULL())
		{
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"0X%X angle miss position for datum angle",handle);
#else
			sprintf(sError,"0X%X�Ǹֶ�ʧλ�û�׼�Ǹ�",handle);
#endif
			throw sError;
		}
		f3dPoint start,end,cur;
		if(pOldDesEndPos->iStartJgBerStyle==0)
		{
			pStartJg->cal_start_ber_pos();
			start = pStartJg->Start();
		}
		else
		{
			pStartJg->cal_end_ber_pos();
			start = pStartJg->End();
		}
		double alfa=Pi-GetRadWingAngle();
		sina=sin(alfa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		double w_d=(pOldDesEndPos->fStartJgLengOffsetY-pOldDesEndPos->fStartJgLengOffsetX*cos(alfa))/sina;
		start+=get_norm_x_wing()*pOldDesEndPos->fStartJgLengOffsetX-GetWingVecX()*w_d;

		if(pOldDesEndPos->iEndJgBerStyle==0)
		{
			pEndJg->cal_start_ber_pos();
			end = pEndJg->Start();
		}
		else
		{
			pEndJg->cal_end_ber_pos();
			end   = pEndJg->End();
		}
		alfa=Pi-GetRadWingAngle();
		sina=sin(alfa);
		sina=sin(alfa);
		if(fabs(sina)<EPS)
			sina=1;		//�Է����ֳ�0
		w_d=(pOldDesEndPos->fEndJgLengOffsetY-pOldDesEndPos->fEndJgLengOffsetX*cos(alfa))/sina;
		end+=get_norm_x_wing()*pOldDesEndPos->fEndJgLengOffsetX-GetWingVecX()*w_d;

		f3dPoint face_pick=pEnd->Position(true),end_xyz = End();
		if(pEnd->m_cPosCalType==4)	//���Ǹֽ��潻��
		{
			pEnd->GetIntersZhun(&face_pick);
			SnapPerp(&end_xyz,start,end,face_pick);
			SetEnd(end_xyz);
		}
		else if(pEnd->m_cPosCalType==7&&	//��ͨ�ظ˼����ȷ���ƫ�ƵĽڵ�
			(pEnd->m_cPosCalViceType!='X'&&pEnd->m_cPosCalViceType!='Y'&&pEnd->m_cPosCalViceType!='Z'))
		{
			//GetDatumPosBer(pDatumNode)�õ��ĵ��λ�ã������ɽڵ���˼�ͶӰ����õ��ģ�����׼�ڵ�ĸ��˼���λ��ʽΪƽ�ƣ�
			//�����׼�ڵ㸸�˼������˼���(����ƫ�ƽڵ��)һ�ˣ���λ��ʽ�޸�Ϊƽ�Ʋ�������  wht 09-07-17 ���&&pDatumNode->hFatherPart==handle
			CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[0]);
			//ƫ�ƽڵ��׼�ڵ㸸�˼�Ϊ��ǰ�˼�ʱ��(pDatumNode->hFatherPart==handle)
			//���ݻ�׼�ڵ��Լ�ƫ�ƾ��������ƫ�ƽڵ�Ϊ�˵�ĸ˼����ն�λ�� wht 10-10-19
			CLDSLineAngle *pDatumFatherJg=GetEndNodeOffsetDatumAngle();
			if(pDatumFatherJg)
			{	//�����׼�ڵ㸸�˼���λ��ʽΪƽ��ʱ����GetDatumPosBerByFaceOffset() wht 10-07-30
				f3dPoint pos=pDatumFatherJg->GetDatumPosBer(pDatumNode);
				if(pDatumFatherJg->pOldDesStartPos->IsFaceOffset()||pDatumFatherJg->pOldDesEndPos->IsFaceOffset())
				{	//��׼�ڵ㶨λ��ʽΪƽ��
					f3dPoint pt=pos;
					f3dPoint face_norm=pDatumFatherJg->pOldDesEndPos->face_offset_norm;
					pos=pDatumFatherJg->GetDatumPosBerByFaceOffset(pDatumNode,face_norm,&pt);	
				}
				f3dPoint line_vec=End()-Start();
				normalize(line_vec);
				//������ƫ����ͶӰ�������޷�ͬʱ��֤ƫ������ͶӰֵ
				end_xyz=pos+line_vec*pEnd->attach_offset;
				SnapPerp(&end_xyz,start,end,end_xyz);
				SetEnd(end_xyz);
			}
		}
		else
		{
			if(pOldDesEndPos->jgber_cal_style==1)
				SnapPerp(&end_xyz,start,end,pEnd->Position(true));
			else //if(pOldDesEndPos->jgber_cal_style==2)
			{	//����ƽ��
				Int3dlf(end_xyz,f3dLine(start,end),face_pick,pOldDesEndPos->face_offset_norm);
				if(pOldDesEndPos->cFaceOffsetDatumLine=='X'||pOldDesEndPos->cFaceOffsetDatumLine=='Y')
				{	//ƽ�ƻ�׼��Ϊ֫����
					SetEnd(end_xyz);
					f3dPoint wing_prj_vec,wing_pos;
					f3dPoint line_vec=end_xyz-Start();
					normalize(line_vec);
					if(pOldDesEndPos->cFaceOffsetDatumLine=='X')	//ƽ�ƻ�׼��ΪX֫����
						wing_pos=GetSelfDatumPosWingXByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,&face_pick);
					else
						wing_pos=GetSelfDatumPosWingYByFaceOffset(pEnd,pOldDesEndPos->face_offset_norm,&face_pick);
					SnapPerp(&end_xyz,Start(),End(),wing_pos);
				}
			}
			SetEnd(end_xyz);
		}
	}
	else //if(pOldDesEndPos->jgber_cal_style==3)		//ָ������
		return;
}
//�жϽǸ�ָ�������λ�ò����Ƿ��������������ѭ��
bool CLDSLineAngle::IsPosSelfDependent(bool bDetectStartTip)
{
	CLDSNode* pTipNode = bDetectStartTip?pStart:pEnd;
	CMultiOffsetPos* pDesTipPos = bDetectStartTip?&desStartPos:&desEndPos;
	short direction = bDetectStartTip ? 0 : 1;
	if( (pDesTipPos->spatialOperationStyle==16||pDesTipPos->spatialOperationStyle==17)&&(
		(pDesTipPos->datumLineStart.datum_pos_style==1&&(pDesTipPos->datumLineStart.des_para.RODEND.hRod==handle&&pDesTipPos->datumLineStart.des_para.RODEND.direction==direction))||
		(pDesTipPos->datumLineEnd.datum_pos_style==1&&(pDesTipPos->datumLineEnd.des_para.RODEND.hRod==handle&&pDesTipPos->datumLineEnd.des_para.RODEND.direction==direction))||
		(pDesTipPos->datumLineStart.datum_pos_style==2&&(pDesTipPos->datumLineStart.des_para.RODNODE.hRod==handle&&pDesTipPos->datumLineStart.des_para.RODNODE.direction==direction))||
		(pDesTipPos->datumLineEnd.datum_pos_style==2&&(pDesTipPos->datumLineEnd.des_para.RODNODE.hRod==handle&&pDesTipPos->datumLineEnd.des_para.RODNODE.direction==direction))))
		return true;	//ͶӰ��ƽ���ߵĶ˵�Ϊ�Ǹֵ�ǰ����������
	if(pTipNode && pTipNode->hFatherPart==handle&&(pDesTipPos->datum_jg_h==0||pDesTipPos->datum_jg_h==handle)&&
		(pDesTipPos->spatialOperationStyle>=1&&pDesTipPos->spatialOperationStyle<=14))
		return true;//�Ǹֵ�ǰ�����������ӵ���ǰ�Ǹ��������ߣ����ߡ������߻�ƫ��׼�ߣ��ϣ�����������
	return false;
}

void CLDSLineAngle::TransToCS(GECS& cs)
{
	GEPOINT start=cs.TransPToCS(baseline.Start());
	GEPOINT end=cs.TransPToCS(baseline.End());
	norm_x_wing=cs.TransVToCS(norm_x_wing);
	norm_y_wing=cs.TransVToCS(norm_y_wing);
	SetModified();
}
void CLDSLineAngle::Offset(const double* offset)
{
	GEPOINT offset3d(offset);
	CLDSLinePart::Offset(offset);
}
double CLDSLinePart::ODDMENT_MINIMAL_MODIFIED=0.5;		//����ͷ��С�䶯ֵ��mm
double CLDSLinePart::RODPOSITION_MINIMAL_MODIFIED=0.1;	//�˼��˵�λ���ƶ���С���ֵ��mm
BYTE CLDSLinePart::_localUpdateMovePartsFlag=0x02;	//0x01:�������������¼����;0x02:��ʱ��ֹ��¼�Ǹ�λ�ñ䶯��������

CHashSet<CLDSBolt*> CLDSLinePart::_localHashModifiedBolts;
CHashSet<CLDSLineAngle*> CLDSLinePart::_localHashModifiedAngles;
CHashSet<CLDSLineTube*> CLDSLinePart::_localHashModifiedTubes;
CHashSet<CLDSGeneralPlate*> CLDSLinePart::_localHashModifiedPlates;

BYTE CLDSLinePart::GetMovePartsUpdateFlag(){return _localUpdateMovePartsFlag;}
BYTE CLDSLinePart::SetMovePartsUpdateFlag(BYTE cbFlag/*=0x01*/){return _localUpdateMovePartsFlag=cbFlag;}

CLDSLinePart::MOVEROD_STACK::MOVEROD_STACK()
{
	m_bTerminateStack=false;
	m_cbOldUpdateMovePartsState=_localUpdateMovePartsFlag;
}
CLDSLinePart::MOVEROD_STACK::MOVEROD_STACK(bool enable)
{
	m_bTerminateStack=false;
	m_cbOldUpdateMovePartsState=_localUpdateMovePartsFlag;
	enable?Enable():Disable();
}
CLDSLinePart::MOVEROD_STACK::~MOVEROD_STACK()
{
	if(!m_bTerminateStack)
		_localUpdateMovePartsFlag=m_cbOldUpdateMovePartsState;
}
void CLDSLinePart::MOVEROD_STACK::RestoreStack()
{
	if(!m_bTerminateStack)
		_localUpdateMovePartsFlag=m_cbOldUpdateMovePartsState;
	m_bTerminateStack=true;
}
bool CLDSLinePart::MOVEROD_STACK::Enable()
{
	bool bOldEnableState=(_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)!=DISABLE_RECORD_RELA_PARTS;
	_localUpdateMovePartsFlag&=0xFD;//(0xff-DISABLE_RECORD_RELA_PARTS);
	return bOldEnableState;
}
bool CLDSLinePart::MOVEROD_STACK::Disable()
{
	bool bOldEnableState=(_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)!=DISABLE_RECORD_RELA_PARTS;
	_localUpdateMovePartsFlag|=DISABLE_RECORD_RELA_PARTS;
	return bOldEnableState;
}
//������һ�������һ��CLDSLineAngle::CalPositionӰ�쵽��λ����˨
CLDSBolt* CLDSLinePart::EnumFirstRelaMoveBolt()
{
	return _localHashModifiedBolts.GetFirst();
}
//������һ�������һ��CLDSLineAngle::CalPositionӰ�쵽��λ����˨
CLDSBolt* CLDSLinePart::EnumNextRelaMoveBolt()
{
	return _localHashModifiedBolts.GetNext();
}
//������һ�������һ��CLDSLineAngle::CalPositionӰ�쵽��λ�Ƹְ�
CLDSGeneralPlate* CLDSLinePart::EnumFirstRelaMovePlate()
{
	return _localHashModifiedPlates.GetFirst();
}
//������һ�������һ��CLDSLineAngle::CalPositionӰ�쵽��λ�Ƹְ�
CLDSGeneralPlate* CLDSLinePart::EnumNextRelaMovePlate()
{
	return _localHashModifiedPlates.GetNext();
}
void CLDSLinePart::ClearModifiedPartsBuff()
{
	if((_localUpdateMovePartsFlag&0x03)==CLEAR_BUFF_OF_RELA_PARTS)
	{	//��սǸ���λ����������¼����
		_localHashModifiedAngles.Empty();
		_localHashModifiedTubes.Empty();
		_localHashModifiedPlates.Empty();
		_localUpdateMovePartsFlag-=CLEAR_BUFF_OF_RELA_PARTS;
	}
}
bool CLDSLinePart::RemoveRelaModifiedPart(long hPart)		//�Ƴ��ӹ�����������Ƴ�ָ������������Undo/Redo��ɾ����ʱ��˲�����
{
	return false;
}
void CLDSLinePart::ClearUpdateMovePartsBuff()
{
	if((_localUpdateMovePartsFlag&0x03)==CLEAR_BUFF_OF_RELA_PARTS)
	{	//��սǸ���λ����������¼����
		_localHashModifiedBolts.Empty();
		_localHashModifiedAngles.Empty();
		_localHashModifiedTubes.Empty();
		_localHashModifiedPlates.Empty();
		_localUpdateMovePartsFlag-=CLEAR_BUFF_OF_RELA_PARTS;
	}
}
bool CLDSLinePart::IsHasModifiedRods(){
	return _localHashModifiedAngles.Count>0||_localHashModifiedTubes.Count>0;
}
//�Ƿ������Ǹַ����䶯����Ҫ����ʵ��Ĺ��� wjh-2018.2.22
bool CLDSLinePart::IsHasRodRelaModifiedParts(){
	return _localHashModifiedAngles.GetNodeNum()>0||_localHashModifiedBolts.GetNodeNum()>0||_localHashModifiedPlates.GetNodeNum()>0;
}
//��¼��Ҫ����ʵ��ĽǸ�
CLDSLineAngle* CLDSLineAngle::AppendModifiedAngles(CLDSLineAngle* pModifiedAngle)
{
	if(pModifiedAngle==NULL)
		return NULL;
	return _localHashModifiedAngles.SetValue(pModifiedAngle->handle,pModifiedAngle);
}
//������һ�������仯����Ҫ����ʵ��ĽǸ�
CLDSLineAngle* CLDSLineAngle::EnumFirstModifiedAngles()
{
	return _localHashModifiedAngles.GetFirst();
}
//������һ�������仯����Ҫ����ʵ��ĽǸ�
CLDSLineAngle* CLDSLineAngle::EnumNextModifiedAngles()
{
	return _localHashModifiedAngles.GetNext();
}
int CLDSLineAngle::GatherOddRelaParts(BYTE start1end2all3/*=3*/)
{
	CLsRef *pLsRef=NULL;
	CLDSBolt *pBolt=NULL,*pFirstBolt=NULL;
	int hits=0;
	for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		pBolt=pLsRef->GetLsPtr();
		CLDSPoint* pDatumPoint=&pBolt->des_base_pos.datumPoint;
		BOOL bIncOddEffect=false;
		if(pDatumPoint->datum_pos_style==7)//<DEVELOP_PROCESS_MARK nameId="">
			bIncOddEffect=true;
		else if(pDatumPoint->datum_pos_style==1)
			bIncOddEffect=pDatumPoint->des_para.RODEND.bIncOddEffect;
		if( (pDatumPoint->datum_pos_style==1|| pDatumPoint->datum_pos_style==7)&&pDatumPoint->des_para.RODEND.hRod==handle&&(
			(pDatumPoint->des_para.RODEND.direction==0&&(start1end2all3&0x01)&&bIncOddEffect)||//ʼ����˨
			(pDatumPoint->des_para.RODEND.direction==1&&(start1end2all3&0x02)&&bIncOddEffect)))//�ն���˨
		{
			if(pFirstBolt==NULL)
				pFirstBolt=pBolt;
			_localHashModifiedBolts.SetValue(pBolt->handle,pBolt);
		}
	}
	if(pFirstBolt)
	{
		THICK_ITEM item;
		for(item=pFirstBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pFirstBolt->GetNextL0ThickItem())
		{
			if(!item.IsHandle()||abs(item.val)==handle)
				continue;
			CLDSPlate* pPlate=(CLDSPlate*)BelongModel()->FromPartHandle(abs(item.val),CLS_PLATE);
			if(pPlate==NULL||_localHashModifiedPlates.GetValue(pPlate->handle)!=NULL)
				continue;
			_localHashModifiedPlates.SetValue(pPlate->handle,pPlate);
		}
	}
	return hits;
}
void CLDSLineAngle::CalPosition(bool bIgnoreRepeatFlag/*=FALSE*/)
{
	ClearUpdateMovePartsBuff();
	if(IsShadowObject())
	{
		CalShadowRodPos();
		return;
	}
	if(!m_bEnableTeG)
	{
		JGZJ jgzj;
		if(getjgzj(jgzj,size_wide))
			xWingXZhunJu = xWingYZhunJu = jgzj;
	}
	CLDSArcLift *pLift=NULL;
#ifdef __COMMON_PART_INC_
	if(m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
#endif
	if(bIgnoreRepeatFlag)
		ClearFlag();
	//if(desStartPos.jgber_cal_style==2&&desEndPos.jgber_cal_style==2)
	//{	//ʼ�˶˾�Ϊ�򵥶�λʱ,������˹���֫��ͬ,��Ϊ����ʱ��ʼĩ����һ��Ϊ����ָ�����㣬��ʱ�Զ��趨��Ӷ�λ��׼
	//	if((desStartPos.spatialOperationStyle==0||desEndPos.spatialOperationStyle==0)&&Layer(1)=='Z')
	//	{
	//		this->GetStartWorkWing()
	//	}
	//}
	GEPOINT oldPosStart=baseline.Start(),oldPosEnd=baseline.End();
	CalStartPosition(pLift);
	CalEndPosition(pLift);
	if((_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)==0)
	{	//�ڲ�汾���Ǹ�λ�÷����䶯ʱ,�Զ���λ��˨
		GEPOINT nowPosStart=baseline.Start(),nowPosEnd=baseline.End();
		GEPOINT vStartOffset=nowPosStart-oldPosStart;
		GEPOINT vEndOffset=nowPosEnd-oldPosEnd;
		if( fabs(vStartOffset.x)+fabs(vStartOffset.y)+fabs(vStartOffset.z)>=RODPOSITION_MINIMAL_MODIFIED||
			fabs(vEndOffset.x)+fabs(vEndOffset.y)+fabs(vEndOffset.z)>=RODPOSITION_MINIMAL_MODIFIED)	//���С��0.1mmʱ��������˨�Զ���λ���
		{
			if(_localHashModifiedAngles.GetValue(handle)==NULL)
			{
				AppendModifiedAngles(this);
				RecordRelaPartsAfterModified();
			}
		}
	}
}
bool CLDSLineAngle::RecordRelaPartsAfterModified()
{
	CLdsPartListStack stack(m_pModel);
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		CLDSPoint* pDatumPoint=&pLsRef->pBolt->des_base_pos.datumPoint;
		if( pLsRef->pBolt->des_base_pos.hPart==handle||
			pDatumPoint->m_hDatumRod==handle||
			(pDatumPoint->datum_pos_style==3&&(pDatumPoint->des_para.AXIS_INTERS.hDatum1==handle||pDatumPoint->des_para.AXIS_INTERS.hDatum2==handle))||
			(pDatumPoint->datum_pos_style==9&&pDatumPoint->des_para.hPart==handle))
		{
			pLsRef->pBolt->correct_worknorm();
			f3dPoint oldpos=pLsRef->pBolt->ucs.origin;
			pLsRef->pBolt->correct_pos();
			GEPOINT vBoltOffset=pLsRef->pBolt->ucs.origin-oldpos;
			if(fabs(vBoltOffset.x)+fabs(vBoltOffset.y)+fabs(vBoltOffset.z)>=0.1)
				vBoltOffset-=vBoltOffset*(vBoltOffset*pLsRef->pBolt->ucs.axis_z);
			if(fabs(vBoltOffset.x)+fabs(vBoltOffset.y)+fabs(vBoltOffset.z)>=0.1)
				_localHashModifiedBolts.SetValue(pLsRef->pBolt->handle,pLsRef->pBolt);
		}
	}
	CSuperSmartPtr<CLDSPart>pPart;
	for(pPart=m_pModel->EnumPartFirst();pPart.IsHasPtr();pPart=m_pModel->EnumPartNext())
	{
		if(pPart->GetClassTypeId()!=CLS_PLATE)
			continue;
		CLDSPlate *pPlate=(CLDSPlate*)pPart.pPlate;
		if(pPlate&&_localHashModifiedPlates.GetValue(pPart->handle)!=NULL)
			continue;	//�Ѵ���
		if(pPlate->designInfo.m_hBasePart==handle||pPlate->designInfo.partList.GetValue(handle)!=NULL)
		{
			if(!pPlate->designInfo.m_bEnableFlexibleDesign)
				continue;
			short siStyle=pPlate->designInfo.iProfileStyle0123;
			if(siStyle==1)
				pPlate->designInfo.iProfileStyle0123=3;
			_localHashModifiedPlates.SetValue(pPlate->handle,pPlate);
			pPlate->DesignPlate();
			pPlate->designInfo.iProfileStyle0123=siStyle;
		}
	}
	return true;
}
//��֫���߶����������
void DESIGN_JGWING_NORM::ReverseDirection()
{
	//ֱ��ָ������
	if(bSpecific)
		spec_norm.vector*=-1.0;
	else
	{
		if(spec_norm.norm_style == 0)
		{	//0.ֱ��ָ������
			near_norm*=-1.0;
			spec_norm.vector*=-1.0;
		}
		//1.�Ǹ�֫���߶�λ 3.������������
		else if(spec_norm.norm_style == 1||spec_norm.norm_style == 3)
			spec_norm.direction=(spec_norm.direction+1)%2;
		//2.���Ǹֽ����淨�� 4.������Է���
		else if(spec_norm.norm_style ==2||spec_norm.norm_style == 4)
			spec_norm.nearVector*=-1.0;
	}
}
//ciTurnStyle0or1
//0.��������ת	��֫��Ϊ����һ֫   ���򲻱�
//				����֫��Ϊ����һ֫ ������
//1.�����淭ת  ��֫��Ϊ����һ֫   ������
//				����֫��Ϊ����һ֫ ���򲻱�
bool CLDSLineAngle::TurnLineAngle(int work_wing_x0_y1,char ciTurnStyle0or1)
{
	if (ciTurnStyle0or1==0)
	{	//��������ת ����֫����
		if(work_wing_x0_y1==0)
			des_norm_x.ReverseDirection();
		else //if(work_wing_x0_y1==1)
			des_norm_y.ReverseDirection();
	}
	else //if(ciTurnStyle0or1==1)
	{	//���ҷ�ת ��֫����
		if(work_wing_x0_y1==0)
			des_norm_y.ReverseDirection();
		else //if(work_wing_x0_y1==1)
			des_norm_x.ReverseDirection();
	}
	//��֫����
	DESIGN_JGWING_NORM tempwingnorm=des_norm_x;
	des_norm_x = des_norm_y;
	des_norm_y = tempwingnorm;
	//��֫λ�ò�������
	int iTm=desStartPos.wing_x_offset.gStyle;
	desStartPos.wing_x_offset.gStyle=desStartPos.wing_y_offset.gStyle;
	desStartPos.wing_y_offset.gStyle=iTm;
	double fTm=desStartPos.wing_x_offset.offsetDist;
	desStartPos.wing_x_offset.offsetDist=desStartPos.wing_y_offset.offsetDist;
	desStartPos.wing_y_offset.offsetDist=fTm;
		
	iTm=desEndPos.wing_x_offset.gStyle;
	desEndPos.wing_x_offset.gStyle=desEndPos.wing_y_offset.gStyle;
	desEndPos.wing_y_offset.gStyle=iTm;
	fTm=desEndPos.wing_x_offset.offsetDist;
	desEndPos.wing_x_offset.offsetDist=desEndPos.wing_y_offset.offsetDist;
	desEndPos.wing_y_offset.offsetDist=fTm;
	if( desStartPos.spatialOperationStyle==15&&(
		desStartPos.cMainCoordCtrlWing=='X'||desStartPos.cMainCoordCtrlWing=='Y'))
	{
		desStartPos.cMainCoordCtrlWing='X'+'Y'-desStartPos.cMainCoordCtrlWing;
		WING_OFFSET_STYLE style=desStartPos.ctrlWing;
		desStartPos.ctrlWing=desStartPos.otherWing;
		desStartPos.otherWing=style;
	}
	if( desEndPos.spatialOperationStyle==15&&(
		desEndPos.cMainCoordCtrlWing=='X'||desEndPos.cMainCoordCtrlWing=='Y'))
	{
		desEndPos.cMainCoordCtrlWing='X'+'Y'-desEndPos.cMainCoordCtrlWing;
		WING_OFFSET_STYLE style=desEndPos.ctrlWing;
		desEndPos.ctrlWing=desEndPos.otherWing;
		desEndPos.otherWing=style;
	}
	cal_x_wing_norm();
	cal_y_wing_norm();
	if(ciTurnStyle0or1==1)
	{	//���ҷ�ת ��֫����
		GEPOINT vWorkWingVec;
		if(work_wing_x0_y1==0)
		{
			if(desStartPos.wing_x_offset.gStyle==4)
				desStartPos.wing_x_offset.offsetDist=GetWidth()-desStartPos.wing_x_offset.offsetDist;
			if(desEndPos.wing_x_offset.gStyle==4)
				desEndPos.wing_x_offset.offsetDist=GetWidth()-desEndPos.wing_x_offset.offsetDist;
			vWorkWingVec=GetWingVecX();
		}
		else //if(work_wing_x0_y1==1)
		{
			if(desStartPos.wing_y_offset.gStyle==4)
				desStartPos.wing_y_offset.offsetDist=GetWidth()-desStartPos.wing_y_offset.offsetDist;
			if(desEndPos.wing_y_offset.gStyle==4)
				desEndPos.wing_y_offset.offsetDist=GetWidth()-desEndPos.wing_y_offset.offsetDist;
			vWorkWingVec=GetWingVecY();
		}
		//Ϊ��һ����ͨ��CalPosition()ʹ�Ǹ�������Ŀ��λ�ã���Ҫ��ǰ�ֶ�����ö����λ�á�wjh-2016.5.31
		SetStart(Start()+vWorkWingVec*GetWidth());
		SetEnd(End()+vWorkWingVec*GetWidth());
	}
	UnlockStart();
	UnlockEnd();
	CalPosition();
	UpdateRelateRayJgPos();
	if(GetClassTypeId()==CLS_GROUPLINEANGLE)	//��ϽǸ�������ӽǸ�λ��
		((CLDSGroupLineAngle*)this)->UpdateSonJgWingNorm();
	return TRUE;
}
bool CLDSLineAngle::ShadowDesign()
{
	if(!IsShadowObject())
		return false;
	CLDSLinePart::ShadowDesign();
	CLDSLineAngle* pMotherAngle=(CLDSLineAngle*)MotherObject(false);
	if(m_iShadowObjType==SHADOW_ISOLATED)
	{
		CParaCS *pAcs=m_pModel->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
		pAcs->UpdateACS(pMotherAngle);
		if(pAcs&&pAcs->GetDefStyle()==CParaCS::CS_MIRTRANS&&pAcs->GetParamInfo().m_xMirInfo.axis_flag&0x13)
		{	//TODO:�澵��Գ�,��������,�����ϲ�Ӧ�öԽǸֽ��о���Ӱ�� wjh-2013-1-20
			norm_y_wing=pAcs->VectorTrans(pMotherAngle->norm_x_wing);
			norm_x_wing=pAcs->VectorTrans(pMotherAngle->norm_y_wing);
		}
		else
		{
			norm_x_wing=pAcs->VectorTrans(pMotherAngle->norm_x_wing);
			norm_y_wing=pAcs->VectorTrans(pMotherAngle->norm_y_wing);
		}
	}
	else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
	{
		CBlockParameter blockPara;
		if(!GetBlockParameter(blockPara))
			return false;
		UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
		norm_x_wing=pMotherAngle->get_norm_x_wing();
		norm_y_wing=pMotherAngle->get_norm_y_wing();
		vector_trans(norm_x_wing,cs,TRUE,TRUE);
		vector_trans(norm_y_wing,cs,TRUE,TRUE);
		if(!des_norm_x.bSpecific&&!des_norm_x.bByOtherWing)
			vector_trans(des_norm_x.near_norm,cs,TRUE,TRUE);
		if(!des_norm_y.bSpecific&&!des_norm_y.bByOtherWing)
			vector_trans(des_norm_y.near_norm,cs,TRUE,TRUE);
		vector_trans(des_norm_x.spec_norm.vector,cs,TRUE,TRUE);
		vector_trans(des_norm_x.spec_norm.nearVector,cs,TRUE,TRUE);
		vector_trans(des_norm_y.spec_norm.vector,cs,TRUE,TRUE);
		vector_trans(des_norm_y.spec_norm.nearVector,cs,TRUE,TRUE);
		if(desStartPos.spatialOperationStyle==0)
		{
			if( desStartPos.datumPoint.datum_pos_style==0||
				desStartPos.datumPoint.datum_pos_style==5||desStartPos.datumPoint.datum_pos_style==6)
			{
				//�����ֱ�Ӹ��»�׼���������ܳ�������:1)���������²���ʱ;2)ȱ�ٲ���ʱ��������޷����� wjh-2019.7.19
				GEPOINT xRsltPos = desStartPos.datumPoint.xRsltPosition;
				coord_trans(xRsltPos, cs, TRUE, TRUE);
				desStartPos.datumPoint.SetRsltPositionOnly(xRsltPos);
				//����ԭʼ��׼����
				coord_trans(desStartPos.datumPoint.datum_pos, cs, TRUE, TRUE);
			}
		}
		if(desEndPos.spatialOperationStyle==0)
		{
			if( desEndPos.datumPoint.datum_pos_style==0||
				desEndPos.datumPoint.datum_pos_style==5||desEndPos.datumPoint.datum_pos_style==6)
			{
				//�����ֱ�Ӹ��»�׼���������ܳ�������:1)���������²���ʱ;2)ȱ�ٲ���ʱ��������޷����� wjh-2019.7.19
				GEPOINT xRsltPos = desEndPos.datumPoint.xRsltPosition;
				coord_trans(xRsltPos, cs, TRUE, TRUE);
				desEndPos.datumPoint.SetRsltPositionOnly(xRsltPos);
				//����ԭʼ��׼����
				coord_trans(desEndPos.datumPoint.datum_pos, cs, TRUE, TRUE);
			}
		}
	}
	getUCS(ucs);
	return true;
}

f3dPoint CLDSLineAngle::CalWingOffsetVec(char cOffsetWing,double spec_g)
{
	if (fabs(spec_g)<EPS)
		return f3dPoint(0,0);
	if(cOffsetWing!='X'&&cOffsetWing!='Y')
		return f3dPoint(0,0);	//δָ��ƫ��֫
	f3dPoint xWingVecX=GetWingVecX();
	f3dPoint xWingVecY=GetWingVecY();
	if(m_fHeJiaoReservedWidth<=0||xWingVecX*xWingVecY<=0)
	{	//�Ǳ���һ��֫��ȵĳ���Ͻ�
		if(spec_g==0)
			return f3dPoint(0,0);		//�����������Ƶ�ʸߣ�ֱ�ӷ�����������߼���Ч��
		else if(cOffsetWing=='X')
			return xWingVecX*spec_g;	//���ط�����Ͻ�ʱ�ĳ���֫��ƫ��ʸ��
		else //if(cOffsetWing=='Y')
			return xWingVecY*spec_g;
	}
	//���غϽ�ʱ����������ƫ�Ƶĸ���ʸ��
	f3dPoint bisectVec=xWingVecX+xWingVecY;
	normalize(bisectVec);
	double innerReservedWidth,outterReservedWidth;
	double halfHeJiao=0.125*Pi-0.25*GetRadWingAngle();
	double increment=size_thick*tan(halfHeJiao);
	if(m_cHeJiaoReservedWidthType=='I')	//ָ��ֵΪ�ڲౣ�����
	{
		innerReservedWidth=m_fHeJiaoReservedWidth;
		outterReservedWidth=innerReservedWidth+size_thick+increment;
	}
	else //if(m_cHeJiaoReservedWidthType=='O')	//ָ��ֵΪ��ౣ�����
	{
		outterReservedWidth=m_fHeJiaoReservedWidth;
		innerReservedWidth=outterReservedWidth-size_thick-increment;
	}
	//�Ͻ�ǰX֫��չ�����ɽǷ��߷���˳ʱ����ת45�㣬�������
	f3dPoint xFormerWingVecX=bisectVec;
	RotateVectorAroundVector(xFormerWingVecX,-0.25*Pi,ucs.axis_z);
	//�Ͻ�ǰY֫��չ�����ɺϽ�ǰX֫��չ������ʱ����ת90�㣬�������
	f3dPoint xFormerWingVecY=bisectVec;
	RotateVectorAroundVector(xFormerWingVecY,0.25*Pi,ucs.axis_z);
	f3dPoint offset;	//�����ߵ㵽�����ƫ����
	if(spec_g>=innerReservedWidth)	//�ںϽ�����
	{
		if(cOffsetWing=='X')
			offset=xFormerWingVecX*outterReservedWidth+(spec_g-innerReservedWidth-size_thick+increment)*xWingVecX;
		else if(cOffsetWing=='Y')
			offset=xFormerWingVecY*outterReservedWidth+(spec_g-innerReservedWidth-size_thick+increment)*xWingVecY;
	}
	else if(spec_g>0)	//�ڱ���֫��Χ��
	{
		if(cOffsetWing=='X')
			offset=xFormerWingVecX*(spec_g-size_thick);
		else if(cOffsetWing=='Y')
			offset=xFormerWingVecY*(spec_g-size_thick);
	}
	else if(spec_g==0)	//Ӧ�ڸ�֫�Ͻ���ͶӰ������
	{
		double prjlen=outterReservedWidth*(xFormerWingVecX*xWingVecX);
		if(cOffsetWing=='X')
			offset=xFormerWingVecX*outterReservedWidth-prjlen*xWingVecX;
		else if(cOffsetWing=='Y')
			offset=xFormerWingVecY*outterReservedWidth-prjlen*xWingVecY;
	}
	return offset;
}
//��ʾб��,����ѹ�����⹤�մ����зֶδ���
BOOL CLDSLineAngle::UpdateInnerCircleSlopeFace(fBody* pBody,f3dPoint wing_x_vec,f3dPoint wing_y_vec,f3dPoint norm_Y)
{
	double dist=10;	//б��ĳ����ѹ�⴦�ļ��
	double length = DISTANCE(Start(),End());
	double R= library!=NULL?library->FindAngleInnerR(size_wide): FindJg_r(size_wide);
	double slope_wide=max(0,R*GetCutAngleCoefR()+m_nCutAngleSpaceOdd);
	if(slope_wide>size_wide)
		slope_wide=R*2.0/3;
	f3dPoint start_vertex,end_vertex,mid_start_vertex,mid_end_vertex;
	f3dPoint line_vec,bi_vec;
	bi_vec.Set(1+wing_y_vec.x,wing_y_vec.y,0);
	normalize(bi_vec);					//��λ��
	if(fabs(bi_vec.y)<EPS)
		return FALSE;
	double r_dist=size_thick+slope_wide;
	double l_dist=size_thick*(bi_vec.x/bi_vec.y);
	if(r_dist < l_dist)		//
		return FALSE;
	if(wing_y_vec.x>0&&m_fHeJiaoReservedWidth>0)
	{	//�Ͻ��ұ���һ���ĺϽǿ��,��Բ������
		double grad45=1.4142135623731;
		double cos45=0.70710678118655;
		double sin45=cos45;
		wing_x_vec.Set(bi_vec.x*cos45-bi_vec.y*-sin45,bi_vec.x*-sin45+bi_vec.y*cos45);
		wing_y_vec.Set(-wing_x_vec.y,wing_x_vec.x);
		l_dist=size.thick;
		double offset_len=size.thick*grad45;
		start_vertex.Set(offset_len*bi_vec.x,offset_len*bi_vec.y,-startOdd());
		end_vertex.Set(offset_len*bi_vec.x,offset_len*bi_vec.y,length+endOdd());
	}
	else
	{
		start_vertex.Set(l_dist,size_thick,-startOdd());
		end_vertex.Set(l_dist,size_thick,length+endOdd());
	}
	line_vec=start_vertex-end_vertex;
	normalize(line_vec);
	double diff=r_dist-l_dist;
	int start_cut_wing_status=0,end_cut_wing_status=0;	//0��ʾ������֫���;1��ʾX֫����֫;2��ʾY֫����֫
	//ʼ��
	if(start_push_flat_x1_y2>0)	//ѹ��
		start_vertex.z+=start_push_flat_length+dist;
	else if(cut_wing[0]>0&&cut_wing_para[0][1]<=GetThick())	//����֫
	{
		start_vertex.z+=cut_wing_para[0][0];
		start_cut_wing_status=1;
	}
	//�ն�
	if(end_push_flat_x1_y2>0)	//ѹ��
		end_vertex.z-=end_push_flat_length+dist;	
	else if(cut_wing[1]>0&&cut_wing_para[1][1]<=GetThick())	//����֫
	{	
		end_vertex.z-=cut_wing_para[1][0];
		end_cut_wing_status=1;
	}
	f3dPoint vertex_arr[6];
	vertex_arr[0]=start_vertex;
	vertex_arr[1]=start_vertex+wing_x_vec*diff;
	vertex_arr[2]=start_vertex+wing_y_vec*diff;
	vertex_arr[3]=end_vertex;
	vertex_arr[4]=end_vertex+wing_x_vec*diff;
	vertex_arr[5]=end_vertex+wing_y_vec*diff;
	//��������֫���
	f3dPoint plane_norm,plane_pick,vec1,vec2;
	if(cut_wing[0]>0&&start_cut_wing_status==0)	//ʼ����֫
	{
		plane_pick.z = -startOdd()+cut_wing_para[0][0];
		double offset_dist=0;
		if(fabs(cut_wing_para[0][2])<EPS2)
			offset_dist=1;	
		if(cut_wing[0]==1)	//X֫Ϊ�н���֫
		{
			vec1.Set(size_wide,0,offset_dist+cut_wing_para[0][2]-cut_wing_para[0][0]);
			vec2.Set(wing_y_vec.x*cut_wing_para[0][1],
				wing_y_vec.y*cut_wing_para[0][1],-cut_wing_para[0][0]);
			plane_norm = vec2^vec1;
		}
		else	//Y֫Ϊ�н���֫
		{
			vec1.Set(wing_y_vec.x*size_wide,size_wide*wing_y_vec.y,
				offset_dist+cut_wing_para[0][2]-cut_wing_para[0][0]);
			vec2.Set(cut_wing_para[0][1],0,-cut_wing_para[0][0]);
			plane_norm = vec1^vec2;	
		}
		if(plane_norm.z>0)
			plane_norm = -plane_norm;
		Int3dlf(vertex_arr[0],vertex_arr[0],line_vec,plane_pick,plane_norm);
		Int3dlf(vertex_arr[1],vertex_arr[1],line_vec,plane_pick,plane_norm);
		Int3dlf(vertex_arr[2],vertex_arr[2],line_vec,plane_pick,plane_norm);
		if(cut_wing_para[0][1]>size_thick && cut_wing_para[0][1]<r_dist)	
		{	//�����ط��н���֫�нǿ�Ƚ���֫���r_dist����Դ�֫�ϵ�б�µ������������
			if(cut_wing[0]==1)
				vertex_arr[2].z=-startOdd();
			else
				vertex_arr[1].z=-startOdd();
		}
	}
	if(cut_wing[1]>0&&end_cut_wing_status==0)	//�ն���֫
	{
		plane_pick.z = length+endOdd()-cut_wing_para[1][0];
		double offset_dist=0;
		if(fabs(cut_wing_para[1][2])<EPS2)
			offset_dist=1;	
		if(cut_wing[1]==1)	//X֫Ϊ�н���֫
		{
			vec1.Set(size_wide,0,-offset_dist-cut_wing_para[1][2]+cut_wing_para[1][0]);
			vec2.Set(wing_y_vec.x*cut_wing_para[1][1],wing_y_vec.y*cut_wing_para[1][1],cut_wing_para[1][0]);
			plane_norm = vec1^vec2;
		}
		else	//Y֫Ϊ�н���֫
		{
			vec1.Set(wing_y_vec.x*size_wide,size_wide*wing_y_vec.y,-offset_dist-cut_wing_para[1][2]+cut_wing_para[1][0]);
			vec2.Set(cut_wing_para[1][1],0,cut_wing_para[1][0]);
			plane_norm = vec2^vec1;
		}
		if(plane_norm.z<0)
			plane_norm = -plane_norm;
		Int3dlf(vertex_arr[3],vertex_arr[3],line_vec,plane_pick,plane_norm);
		Int3dlf(vertex_arr[4],vertex_arr[4],line_vec,plane_pick,plane_norm);
		Int3dlf(vertex_arr[5],vertex_arr[5],line_vec,plane_pick,plane_norm);
		if(cut_wing_para[1][1]>size_thick && cut_wing_para[1][1]<r_dist)
		{	//�����ط��н���֫�нǿ�Ƚ���֫���r_dist����Դ�֫�ϵ�б�µ������������
			if(cut_wing[1]==1)
				vertex_arr[5].z=length+endOdd();
			else
				vertex_arr[4].z=length+endOdd();
		}
	}
	//�����н����
	double wide=size_wide-slope_wide-size_thick;
	if(cut_angle[0][0]>0&&cut_angle[0][1]>wide)	//ʼ��X֫�н�
	{
		plane_pick.Set(size_wide-cut_angle[0][1],0,-startOdd());
		vec1.Set(cut_angle[0][1],0,cut_angle[0][0]);
		vec2.Set(0,1,0);
		plane_norm = vec2^vec1;
		if(plane_norm.z>0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
			plane_norm = -plane_norm;
		Int3dlf(vertex_arr[1],vertex_arr[1],line_vec,plane_pick,plane_norm);
		if(cut_angle[0][1]>=size_wide)
		{
			Int3dlf(vertex_arr[0],vertex_arr[0],line_vec,plane_pick,plane_norm);
			Int3dlf(vertex_arr[2],vertex_arr[2],line_vec,plane_pick,plane_norm);
		}
	}
	else if(cut_angle[1][0]>0&&cut_angle[1][1]>wide)	//ʼ��Y֫�н�
	{
		plane_pick.Set((size_wide-cut_angle[1][1])*wing_y_vec.x,
			(size_wide-cut_angle[1][1])*wing_y_vec.y,-startOdd());
		vec1.Set(cut_angle[1][1]*wing_y_vec.x,cut_angle[1][1]*wing_y_vec.y,
			cut_angle[1][0]);
		plane_norm = norm_Y^vec1;
		if(plane_norm.z>0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
			plane_norm = -plane_norm;
		Int3dlf(vertex_arr[2],vertex_arr[2],line_vec,plane_pick,plane_norm);
		if(cut_angle[1][1]>=size_wide)
		{
			Int3dlf(vertex_arr[0],vertex_arr[0],line_vec,plane_pick,plane_norm);
			Int3dlf(vertex_arr[1],vertex_arr[1],line_vec,plane_pick,plane_norm);
		}
	}
	if(cut_angle[2][0]>0&&cut_angle[2][1]>wide)	//�ն�X֫�н�
	{
		plane_pick.Set(size_wide-cut_angle[2][1],0,length+endOdd());
		vec1.Set(cut_angle[2][1],0,-cut_angle[2][0]);
		vec2.Set(0,1,0);
		plane_norm = vec1^vec2;
		if(plane_norm.z<0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
			plane_norm = -plane_norm;
		Int3dlf(vertex_arr[4],vertex_arr[4],line_vec,plane_pick,plane_norm);
		if(cut_angle[2][1]>=size_wide)
		{
			Int3dlf(vertex_arr[3],vertex_arr[3],line_vec,plane_pick,plane_norm);
			Int3dlf(vertex_arr[5],vertex_arr[5],line_vec,plane_pick,plane_norm);
		}
	}
	else if(cut_angle[3][0]>0&&cut_angle[3][1]>wide)	//�ն�Y֫�н�
	{
		plane_pick.Set((size_wide-cut_angle[3][1])*wing_y_vec.x,
			(size_wide-cut_angle[3][1])*wing_y_vec.y,
			length+endOdd());
		vec1.Set(cut_angle[3][1]*wing_y_vec.x,cut_angle[3][1]*wing_y_vec.y,
			-cut_angle[3][0]);
		plane_norm = vec1^norm_Y;
		if(plane_norm.z<0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
			plane_norm = -plane_norm;
		Int3dlf(vertex_arr[5],vertex_arr[5],line_vec,plane_pick,plane_norm);
		if(cut_angle[3][1]>=size_wide)
		{
			Int3dlf(vertex_arr[3],vertex_arr[3],line_vec,plane_pick,plane_norm);
			Int3dlf(vertex_arr[4],vertex_arr[4],line_vec,plane_pick,plane_norm);
		}
	}
	int iMarkStartIndex=pBody->vertex.GetNodeNum();
	for(int i=0;i<6;i++)
		pBody->vertex.append(vertex_arr[i]);
	//�м�ѹ�⴦��
	int midPushFlatX1Y2=mid_push_flat_x1_y2;
	f3dPoint mid_vertex_arr[6],mid_push_flat_pos;
	if(midPushFlatX1Y2>0)
	{
		CLDSNode *pMidPushFlatNode=BelongModel()->FromNodeHandle(mid_push_flat_node_handle);
		if(pMidPushFlatNode)
			mid_push_flat_pos=GetDatumPos(pMidPushFlatNode);
			//SnapPerp(&mid_push_flat_pos,Start(),End(),pMidPushFlatNode->Position(false));
		else 
			midPushFlatX1Y2=0;
	}
	double len_starttomid =DISTANCE(Start(),mid_push_flat_pos);
	if(midPushFlatX1Y2>0)
	{	//�м�ѹ��ʱ������������
		mid_start_vertex.Set(start_vertex.x,start_vertex.y,len_starttomid-mid_push_flat_start_offset-dist);
		mid_end_vertex.Set(start_vertex.x,start_vertex.y,len_starttomid+mid_push_flat_end_offset+dist);
		mid_vertex_arr[0]=mid_start_vertex;
		mid_vertex_arr[1]=mid_start_vertex+wing_x_vec*slope_wide;
		mid_vertex_arr[2]=mid_start_vertex+wing_y_vec*slope_wide;
		mid_vertex_arr[3]=mid_end_vertex;
		mid_vertex_arr[4]=mid_end_vertex+wing_x_vec*slope_wide;
		mid_vertex_arr[5]=mid_end_vertex+wing_y_vec*slope_wide;
		for(i=0;i<6;i++)
			pBody->vertex.append(mid_vertex_arr[i]);
	}
	//���³�ʼ��
	CCreateFace createFace;
	createFace.InitVertexList(pBody);
	//б�涥��
	COLORREF crAngleEdge=GetColor(CLS_LINEANGLE,'S',cMaterial);	//��ɫ
	f3dPolyFace *pFace=NULL;
	pFace=pBody->faceList.append();
	pFace->material=crAngleEdge;
	createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+2,iMarkStartIndex);
	createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+1);
	createFace.NewOutterEdgeLine(pFace,iMarkStartIndex);
	if(midPushFlatX1Y2>0)	//�м�ѹ��
	{	//��һ�ֶ�б������
		pFace=pBody->faceList.append();
		pFace->material=crAngleEdge;
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+2,iMarkStartIndex+1);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+2);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+1);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+1);
		//��һ�ֶ�б�����
		pFace=pBody->faceList.append();
		pFace->material=crAngleEdge;
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+1,iMarkStartIndex+6);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+2);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6);
		//�ڶ��ֶ�б�涥��
		pFace=pBody->faceList.append();
		pFace->material=crAngleEdge;
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+5,iMarkStartIndex+6+3);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+4);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+3);
		//�ڶ��ֶ�б������
		pFace=pBody->faceList.append();
		pFace->material=crAngleEdge;
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+5,iMarkStartIndex+6+4);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+5);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+4);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+6+4);
	}
	else
	{
		//б������
		pFace=pBody->faceList.append();
		pFace->material=crAngleEdge;
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+2,iMarkStartIndex+1);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+5);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+4);
		createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+1);
	}
	//б�����
	pFace=pBody->faceList.append();
	pFace->material=crAngleEdge;
	createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+4,iMarkStartIndex+3);
	createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+5);
	createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+3);
	return TRUE;
}
//�Ͻ�ʱ��Ҫ����һ���ĺϽǿ��m_fHeJiaoReservedWidth wjh-2011.10.20
BOOL CLDSLineAngle::CreateSpecialHeJiaoSolidModel(fBody* pBody,BOOL bDisplayHole/*=FALSE*/)
{
	f3dPoint vertex_arr[20];	//�б���֫��ȵ�����ϽǽǸֽ���������㣨ÿһ��10����㣩
	double length = DISTANCE(Start(),End());
	//���ϽǺ��X֫���߼���չ�����������
	f3dPoint xHeJiaoNormX(0,-1),xWingVecX(1,0);
	//���ϽǺ�Y֫���ߣ��������
	f3dPoint norm=get_norm_y_wing();	//��ʱʹ��
	f3dPoint xHeJiaoNormY(norm*ucs.axis_x,norm*ucs.axis_y);
	normalize(xHeJiaoNormY);				//��λ��
	//���ϽǺ�Y֫��չ�����������
	f3dPoint xWingVecY(xHeJiaoNormY.y,-xHeJiaoNormY.x);
	//�Ƿ��߷����������
	f3dPoint bi_vec(1+xWingVecY.x,xWingVecY.y,0);
	normalize(bi_vec);					//��λ��
	//�Ͻ�ǰX֫��չ�����ɽǷ��߷���˳ʱ����ת45�㣬�������
	double grad45=1.4142135623731;
	double cos45=0.70710678118655;	//cos(-45��)
	double sin45=cos45;
	f3dPoint xFormerWingVecX(bi_vec.x*cos45-bi_vec.y*-sin45,bi_vec.x*-sin45+bi_vec.y*cos45);
	//�Ͻ�ǰY֫��չ�����ɺϽ�ǰX֫��չ������ʱ����ת90�㣬�������
	f3dPoint xFormerWingVecY(-xFormerWingVecX.y,xFormerWingVecX.x);
	//֫�����ϽǱ������
	double innerReservedWidth,outterReservedWidth,increment;
	double halfHeJiao=0.125*Pi-0.25*GetRadWingAngle();
	increment=size_thick*tan(halfHeJiao);
	if(m_cHeJiaoReservedWidthType=='I')	//ָ��ֵΪ�ڲౣ�����
	{
		innerReservedWidth=m_fHeJiaoReservedWidth;
		outterReservedWidth=innerReservedWidth+size_thick+increment;
	}
	else //if(m_cHeJiaoReservedWidthType=='O')	//ָ��ֵΪ��ౣ�����
	{
		outterReservedWidth=m_fHeJiaoReservedWidth;
		innerReservedWidth=outterReservedWidth-size_thick-increment;
	}
	//ʼ�����
	vertex_arr[0].x = 0;
	vertex_arr[0].y = 0;
	vertex_arr[0].z = -startOdd();
	//Y֫���ϽǱ����յ�
	vertex_arr[1].x = outterReservedWidth*xFormerWingVecY.x;
	vertex_arr[1].y = outterReservedWidth*xFormerWingVecY.y;
	vertex_arr[1].z = -startOdd();
	//Y֫�������(2016.6.30�Ĺ���2017.12.12 ��������)
	vertex_arr[2] = vertex_arr[1]+xWingVecY*(size_wide-outterReservedWidth);
	//Y֫�ڲ�����
	vertex_arr[3] = vertex_arr[2]-xHeJiaoNormY*size_thick;
	//Y֫�ڲ�ϽǱ����յ�
	vertex_arr[4]=size_thick*xFormerWingVecX+(size_thick+innerReservedWidth)*xFormerWingVecY;
	vertex_arr[4].z = -startOdd();
	//Y֫��Բ���м�յ�,����Բ����������ʾ���ʴ˴����ٿ�����Բ��Ӱ�� wjh-2016.6.30
	double innerR=0;//library->FindAngleInnerR(size_wide);
	double offset_len=(size_thick+innerR)*grad45-innerR;
	vertex_arr[5].Set(offset_len*bi_vec.x,offset_len*bi_vec.y,-startOdd());
	//X֫�ڲ�ϽǱ����յ�
	vertex_arr[6]=size_thick*xFormerWingVecY+(size_thick+innerReservedWidth)*xFormerWingVecX;
	vertex_arr[6].z = -startOdd();
	//X֫���ϽǱ����յ�
	vertex_arr[9].x = outterReservedWidth*xFormerWingVecX.x;
	vertex_arr[9].y = outterReservedWidth*xFormerWingVecX.y;
	vertex_arr[9].z = -startOdd();
	//X֫�������(2017.12.12 ��������)
	vertex_arr[8] = vertex_arr[9]+xWingVecX*(size_wide-outterReservedWidth);
	//X֫�ڲ�����
	vertex_arr[7] = vertex_arr[8]-xHeJiaoNormX*size_thick;

	int i;
	for(i=0;i<10;i++)
		vertex_arr[i+10].Set(vertex_arr[i].x,vertex_arr[i].y,length+endOdd());
	f3dPoint start_cut_wing_vertex_arr[3],end_cut_wing_vertex_arr[3];
	int start_cut_wing_vertex_arr_index[3],end_cut_wing_vertex_arr_index[3];
	int start_cut_wing_status=0,end_cut_wing_status=0;	//0��ʾ������֫���;1��ʾX֫����֫;2��ʾY֫����֫
	/*if(cut_wing[0]==1&&cut_wing_para[0][1]<=GetThick())
	{	//ʼ��X֫����֫
		start_cut_wing_status=1;
		start_cut_wing_vertex_arr[0]= vertex_arr[3];
		start_cut_wing_vertex_arr[0].z += cut_wing_para[0][0];
		vertex_arr[4].z+=cut_wing_para[0][2];
		vertex_arr[5].z+=cut_wing_para[0][2];
		f3dPoint line_vec=vertex_arr[3]-vertex_arr[2];
		Int3dlf(start_cut_wing_vertex_arr[1],start_cut_wing_vertex_arr[0],line_vec,vertex_arr[0],xHeJiaoNormX);
		Int3dlf(start_cut_wing_vertex_arr[2],vertex_arr[3],line_vec,vertex_arr[0],xHeJiaoNormX);
	}
	else if(cut_wing[0]==2&&cut_wing_para[0][1]<=GetThick())
	{	//ʼ��Y֫����֫
		start_cut_wing_status=2;
		start_cut_wing_vertex_arr[0]= vertex_arr[3];
		start_cut_wing_vertex_arr[0].z += cut_wing_para[0][0];
		vertex_arr[1].z+=cut_wing_para[0][2];
		vertex_arr[2].z+=cut_wing_para[0][2];
		f3dPoint line_vec=vertex_arr[3]-vertex_arr[4];
		Int3dlf(start_cut_wing_vertex_arr[1],start_cut_wing_vertex_arr[0],line_vec,vertex_arr[0],xHeJiaoNormY);
		Int3dlf(start_cut_wing_vertex_arr[2],vertex_arr[3],line_vec,vertex_arr[0],xHeJiaoNormY);
	}
	if(cut_wing[1]==1&&cut_wing_para[1][1]<=GetThick())
	{	//�ն�X֫����֫
		end_cut_wing_status=1;
		end_cut_wing_vertex_arr[0]= vertex_arr[9];
		end_cut_wing_vertex_arr[0].z -= cut_wing_para[1][0];
		vertex_arr[10].z-=cut_wing_para[1][2];
		vertex_arr[11].z-=cut_wing_para[1][2];
		f3dPoint line_vec=vertex_arr[9]-vertex_arr[8];
		Int3dlf(end_cut_wing_vertex_arr[1],end_cut_wing_vertex_arr[0],line_vec,vertex_arr[6],xHeJiaoNormX);
		Int3dlf(end_cut_wing_vertex_arr[2],vertex_arr[9],line_vec,vertex_arr[6],xHeJiaoNormX);
	}
	else if(cut_wing[1]==2&&cut_wing_para[1][1]<=GetThick())
	{	//�ն�Y֫����֫
		end_cut_wing_status=2;
		end_cut_wing_vertex_arr[0]= vertex_arr[9];
		end_cut_wing_vertex_arr[0].z -= cut_wing_para[1][0];
		vertex_arr[7].z-=cut_wing_para[1][2];
		vertex_arr[8].z-=cut_wing_para[1][2];
		f3dPoint line_vec=vertex_arr[9]-vertex_arr[10];
		Int3dlf(end_cut_wing_vertex_arr[1],end_cut_wing_vertex_arr[0],line_vec,vertex_arr[6],xHeJiaoNormY);
		Int3dlf(end_cut_wing_vertex_arr[2],vertex_arr[9],line_vec,vertex_arr[6],xHeJiaoNormY);
	}*/

	for(i=0;i<20;i++)
		pBody->vertex.append(vertex_arr[i]);
	if(start_cut_wing_status!=0)
	{	//ʼ������֫
		for(int kk=0;kk<3;kk++)
		{
			pBody->vertex.append(start_cut_wing_vertex_arr[kk]);
			start_cut_wing_vertex_arr_index[kk] = pBody->vertex.GetCurrentIndex();
		}
	}
	if(end_cut_wing_status!=0)
	{	//�ն�����֫
		for(int kk=0;kk<3;kk++)
		{
			pBody->vertex.append(end_cut_wing_vertex_arr[kk]);
			end_cut_wing_vertex_arr_index[kk] = pBody->vertex.GetCurrentIndex();
		}
	}

	ATOM_LIST<BOLTHOLE_INFO> x_wing_ls,y_wing_ls;
	if(m_bDisplayHole || bDisplayHole)
	{
		CLsRef *pLsRef;
		for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			BOLTHOLE_INFO boltHole;
			boltHole.axis_x = (*pLsRef)->ucs.axis_x;
			boltHole.norm = (*pLsRef)->get_norm();
			boltHole.bolt_d = (*pLsRef)->get_d();
			boltHole.waistLen = pLsRef->waistLen;
			boltHole.hole_d = (*pLsRef)->hole_d_increment+(*pLsRef)->get_d();
			boltHole.centre = (*pLsRef)->ucs.origin;
			vector_trans(boltHole.axis_x,ucs,FALSE);
			coord_trans(boltHole.centre,ucs,FALSE,TRUE);
			vector_trans(boltHole.norm,ucs,FALSE);
			normalize(boltHole.norm);
			double ddx=xHeJiaoNormX*boltHole.norm;
			double ddy=xHeJiaoNormY*boltHole.norm;
			//if(fabs(boltHole.norm.x)>fabs(boltHole.norm.y))	//Y֫��˨
			//���ڿ��Ͻ��ҿ��ϽǶȹ�����Сʱ��һ�д����жϲ�׼ȷ,�޸�Ϊ���� wht 10-11-29
			if(fabs(ddx)>fabs(ddy))	//X֫��˨
				x_wing_ls.append(boltHole);
			else					//Y֫��˨
				y_wing_ls.append(boltHole);
		}
	}
	f3dPolyFace *pFace,*pUpFace,*pDownFace;//,*pStartUpFlatFace,*pStartDownFlatFace,*pEndUpFlatFace,*pEndDownFlatFace;
	COLORREF crAngleWingX=GetColor(CLS_LINEANGLE,'X',cMaterial);
	COLORREF crAngleWingY=GetColor(CLS_LINEANGLE,'Y',cMaterial);
	COLORREF crAngleEdge=GetColor(CLS_LINEANGLE,'E',cMaterial);
	if(pStart==NULL||pEnd==NULL)
		crAngleWingX=crAngleWingY=crAngleEdge=GetColor(CLS_LINEANGLE,'S',cMaterial);
	else if(group_father_jg_h>0x20)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pGroupJg&&pGroupJg->m_hDatumSonAngle!=handle)
		{	//��1���ӽǸְ�ָ����ɫ��������,1���ӽǸֱ�������ͨ�Ǹ���ɫһ�� wht 11-07-16
			crAngleWingX=GetColor(CLS_LINEANGLE,'x',cMaterial);
			crAngleWingY=GetColor(CLS_LINEANGLE,'y',cMaterial);
			crAngleEdge=GetColor(CLS_LINEANGLE,'e',cMaterial);
		}
	}
	if(m_bUserSpecColor)	//�û�ָ��������ɫ
		crAngleWingX=crAngleWingY=crAngleEdge=crMaterial;
	//��ʼ������
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	//if(start_push_flat_x1_y2<=0)	//ʼ�˲�ѹ��
	{
		//1.�Ǹֵ�һ������(��ʼ����)
		if(start_cut_wing_status==0)
		{	//ʼ�˲�����֫
			pFace = pBody->faceList.append();
			pFace->material = crAngleWingX;
			createFace.NewOutterEdgeLine(pFace,1,0);
			for(i=2;i<10;i++)
				createFace.NewOutterEdgeLine(pFace,i);
			createFace.NewOutterEdgeLine(pFace,0);
		}
		/*else if(start_cut_wing_status==1)
		{	//ʼ��X֫����֫
			pFace = pBody->faceList.append();
			pFace->material = crAngleWingX;
			createFace.NewOutterEdgeLine(pFace,1,0);
			createFace.NewOutterEdgeLine(pFace,2);
			createFace.NewOutterEdgeLine(pFace,3);
			createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
			createFace.NewOutterEdgeLine(pFace,0);
		}
		else if(start_cut_wing_status==2)
		{	//ʼ��Y֫����֫
			pFace = pBody->faceList.append();
			pFace->material = crAngleWingX;
			createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2],0);
			createFace.NewOutterEdgeLine(pFace,3);
			createFace.NewOutterEdgeLine(pFace,4);
			createFace.NewOutterEdgeLine(pFace,5);
			createFace.NewOutterEdgeLine(pFace,0);
		}*/
	}

	//if(end_push_flat_x1_y2<=0)	//�ն˲�ѹ��
	{
		//2.�Ǹֵڶ�������(��ֹ����)
		if(end_cut_wing_status==0)
		{	//�ն˲�����֫
			pFace = pBody->faceList.append();
			pFace->material = crAngleWingX;
			createFace.NewOutterEdgeLine(pFace,19,10);
			for(i=18;i>=10;i--)
				createFace.NewOutterEdgeLine(pFace,i);
		}
		/*else if(end_cut_wing_status==1)
		{	//�ն�X֫����֫
			pFace = pBody->faceList.append();
			pFace->material = crAngleWingX;
			createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2],6);
			createFace.NewOutterEdgeLine(pFace,9);
			createFace.NewOutterEdgeLine(pFace,8);
			createFace.NewOutterEdgeLine(pFace,7);
			createFace.NewOutterEdgeLine(pFace,6);
		}
		else if(end_cut_wing_status==2)
		{	//�ն�Y֫����֫
			pFace = pBody->faceList.append();
			pFace->material = crAngleWingX;
			createFace.NewOutterEdgeLine(pFace,11,6);
			createFace.NewOutterEdgeLine(pFace,10);
			createFace.NewOutterEdgeLine(pFace,9);
			createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
			createFace.NewOutterEdgeLine(pFace,0);
		}*/
	}

	//3.1�Ǹ�Y֫���ϽǱ���������
	pFace = pBody->faceList.append();
	pFace->material = crAngleWingY;
	int face_start_vertex_i=0;
	if(start_cut_wing_status==0)		//ʼ������֫��֫
	{
		createFace.NewOutterEdgeLine(pFace,0,1);
		face_start_vertex_i=1;
	}
	/*else if(start_cut_wing_status==1)	//ʼ��X֫��֫��֫
	{
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1],start_cut_wing_vertex_arr_index[0]);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,3);
		createFace.NewOutterEdgeLine(pFace,2);
		face_start_vertex_i=start_cut_wing_vertex_arr_index[0];
	}
	else if(start_cut_wing_status==2)	//ʼ��Y֫��֫��֫
	{
		createFace.NewOutterEdgeLine(pFace,2,start_cut_wing_vertex_arr_index[0]);
		face_start_vertex_i=start_cut_wing_vertex_arr_index[0];
	}*/
	createFace.NewOutterEdgeLine(pFace,10);
	if(end_cut_wing_status==0)		//�ն�����֫��֫
		createFace.NewOutterEdgeLine(pFace,11);
	/*else if(end_cut_wing_status==1)	//�ն�X֫��֫��֫
	{
		createFace.NewOutterEdgeLine(pFace,9);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
	}
	else if(end_cut_wing_status==2)	//�ն�Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);*/
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
	//3.2�Ǹ�Y֫���Ͻ�������
	pUpFace = pFace = pBody->faceList.append();
	pFace->material = crAngleWingY;
	if(start_cut_wing_status==0)		//ʼ������֫��֫
	{
		createFace.NewOutterEdgeLine(pFace,1,2);
		face_start_vertex_i=2;
	}
	/*else if(start_cut_wing_status==1)	//ʼ��X֫��֫��֫
	{
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1],start_cut_wing_vertex_arr_index[0]);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,3);
		createFace.NewOutterEdgeLine(pFace,2);
		face_start_vertex_i=start_cut_wing_vertex_arr_index[0];
	}
	else if(start_cut_wing_status==2)	//ʼ��Y֫��֫��֫
	{
		createFace.NewOutterEdgeLine(pFace,2,start_cut_wing_vertex_arr_index[0]);
		face_start_vertex_i=start_cut_wing_vertex_arr_index[0];
	}*/
	createFace.NewOutterEdgeLine(pFace,11);
	if(end_cut_wing_status==0)		//�ն�����֫��֫
		createFace.NewOutterEdgeLine(pFace,12);
	/*else if(end_cut_wing_status==1)	//�ն�X֫��֫��֫
	{
		createFace.NewOutterEdgeLine(pFace,9);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
	}
	else if(end_cut_wing_status==2)	//�ն�Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);*/
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

	//3.3.�Ǹ�Y֫�ڲ�Ͻ���
	pDownFace = pFace = pBody->faceList.append();
	if(pStart&&pEnd)
		pFace->material = crAngleWingY;
	else
		pFace->material = crAngleWingY;
	if(start_cut_wing_status!=2)
	{	//��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,3,4);
		face_start_vertex_i=4;
	}
	/*else
	{	//Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1],1);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,0);
		face_start_vertex_i=1;
	}*/
	createFace.NewOutterEdgeLine(pFace,13);
	if(end_cut_wing_status!=2)	//��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,14);
	/*else
	{	//Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,7);
	}*/
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

	//3.3.�Ǹ�Y֫�ڲ�ϽǱ�����
	pFace = pBody->faceList.append();
	if(pStart&&pEnd)
		pFace->material = crAngleWingY;
	else
		pFace->material = crAngleWingY;
	if(start_cut_wing_status!=2)
	{	//��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,4,5);
		face_start_vertex_i=5;
	}
	/*else
	{	//Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1],1);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,0);
		face_start_vertex_i=1;
	}*/
	createFace.NewOutterEdgeLine(pFace,14);
	if(end_cut_wing_status!=2)	//��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,15);
	/*else
	{	//Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,7);
	}*/
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
	//-----------VVVVV----�����˨��----VVVVV-------------------------
	if(m_bDisplayHole || bDisplayHole)
	{
		CHoleMaker hole_maker;
		UCS_STRU hole_ucs;
		BOLTHOLE_INFO *pLsCir;
		for(pLsCir=y_wing_ls.GetFirst();pLsCir;pLsCir=y_wing_ls.GetNext())
		{
			COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
			hole_ucs.origin = pLsCir->centre;
			hole_ucs.axis_x = pLsCir->axis_x;
			hole_ucs.axis_z = pLsCir->norm;
			if(hole_ucs.axis_x.IsZero())
				hole_ucs.axis_x=btc::CalLocalCS_X_AXIS(hole_ucs.axis_z);
			hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace,pDownFace,pBody,crLsHole);
		}
	}
	//--------------------�����˨��-----------------------------

	//4.�Ǹ�Y֫��������
	pFace = pBody->faceList.append();
	pFace->material = crAngleEdge;
	if(start_cut_wing_status!=2)
	{	//��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,2,3);
		face_start_vertex_i=3;
	}
	/*else
	{	//Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1],1);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,0);
		face_start_vertex_i=1;
	}*/
	createFace.NewOutterEdgeLine(pFace,12);
	if(end_cut_wing_status!=2)	//��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,13);
	/*else
	{	//Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,7);
	}*/
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

	//5.1�Ǹ�X֫�ڲ�ϽǱ�����
	pFace = pBody->faceList.append();
	pFace->material = crAngleWingX;
	if(start_cut_wing_status!=2)
	{	//��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,5,6);
		face_start_vertex_i=6;
	}
	/*else
	{	//Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1],1);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,0);
		face_start_vertex_i=1;
	}*/
	createFace.NewOutterEdgeLine(pFace,15);
	if(end_cut_wing_status!=2)	//��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,16);
	/*else
	{	//Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,7);
	}*/
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

	//5.2�Ǹ�X֫�ڲ�Ͻ���
	pDownFace = pFace = pBody->faceList.append();
	pFace->material = crAngleWingX;
	if(start_cut_wing_status!=1)
	{
		createFace.NewOutterEdgeLine(pFace,6,7);
		face_start_vertex_i=7;
	}
	/*else
	{	//ʼ��X֫����֫
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2],0);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,5);
		face_start_vertex_i=0;
	}*/
	createFace.NewOutterEdgeLine(pFace,16);
	/*if(end_cut_wing_status==1)
	{	//�ն�X֫����֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
	}*/
	createFace.NewOutterEdgeLine(pFace,17);
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

	//5.3�Ǹ�X֫���Ͻ�������
	pUpFace = pFace = pBody->faceList.append();
	if(pStart&&pEnd)
		pFace->material = crAngleWingX;
	else
		pFace->material = crAngleWingX;
	if(start_cut_wing_status==0)
	{	//ʼ������֫��֫
		createFace.NewOutterEdgeLine(pFace,8,9);
		face_start_vertex_i=9;
	}
	/*else if(start_cut_wing_status==1)
	{	//ʼ��X֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0],4);
		face_start_vertex_i=4;
	}
	else if(start_cut_wing_status==2)
	{	//ʼ��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,3,4);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0]);
		face_start_vertex_i=4;
	}*/
	if(end_cut_wing_status==0)		//�ն�����֫��֫
		createFace.NewOutterEdgeLine(pFace,18);
	/*else if(end_cut_wing_status==1)	//�ն�X֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
	else if(end_cut_wing_status==2)
	{	//�ն�Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,9);
	}*/
	createFace.NewOutterEdgeLine(pFace,19);
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

	//5.3�Ǹ�X֫���ϽǱ���������
	pFace = pBody->faceList.append();
	if(pStart&&pEnd)
		pFace->material = crAngleWingX;
	else
		pFace->material = crAngleWingX;
	if(start_cut_wing_status==0)
	{	//ʼ������֫��֫
		createFace.NewOutterEdgeLine(pFace,9,0);
		face_start_vertex_i=0;
	}
	/*else if(start_cut_wing_status==1)
	{	//ʼ��X֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0],4);
		face_start_vertex_i=4;
	}
	else if(start_cut_wing_status==2)
	{	//ʼ��Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,3,4);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0]);
		face_start_vertex_i=4;
	}*/
	if(end_cut_wing_status==0)		//�ն�����֫��֫
		createFace.NewOutterEdgeLine(pFace,19);
	/*else if(end_cut_wing_status==1)	//�ն�X֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
	else if(end_cut_wing_status==2)
	{	//�ն�Y֫��֫��֫
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
		createFace.NewOutterEdgeLine(pFace,9);
	}*/
	createFace.NewOutterEdgeLine(pFace,10);
	createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
	//-----------VVVVV----�����˨��----VVVVV-------------------------
	if(m_bDisplayHole || bDisplayHole)
	{
		CHoleMaker hole_maker;
		UCS_STRU hole_ucs;
		BOLTHOLE_INFO *pLsCir;
		for(pLsCir=x_wing_ls.GetFirst();pLsCir;pLsCir=x_wing_ls.GetNext())
		{
			COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
			hole_ucs.origin = pLsCir->centre;
			hole_ucs.axis_x = pLsCir->axis_x;
			hole_ucs.axis_z = pLsCir->norm;
			hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace,pDownFace,pBody,crLsHole);
		}
	}
	//--------------------�����˨��-----------------------------

	//6.�Ǹ�X֫��������
	pFace = pBody->faceList.append();
	pFace->material = crAngleEdge;
	createFace.NewOutterEdgeLine(pFace,7,8);
	createFace.NewOutterEdgeLine(pFace,17);
	createFace.NewOutterEdgeLine(pFace,18);
	createFace.NewOutterEdgeLine(pFace,8);


	//---------------�����н���֫---------------------------------
	//---------------����֫���и���---------------------------------
	/*COLORREF crCutFaceOrPolyJoint=GetColor(CLS_POLYJOINT,-1,' '); //�и���
	if(start_cut_wing_status==1)
	{	//ʼ��X֫����֫
		pFace = pBody->faceList.append();
		pFace->material = crCutFaceOrPolyJoint;
		createFace.NewOutterEdgeLine(pFace,5,4);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0]);
		createFace.NewOutterEdgeLine(pFace,4);
	}
	else if(start_cut_wing_status==2)
	{	//ʼ��Y֫����֫
		pFace = pBody->faceList.append();
		pFace->material = crCutFaceOrPolyJoint;
		createFace.NewOutterEdgeLine(pFace,2,1);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0]);
		createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,1);
	}
	if(end_cut_wing_status==1)
	{	//�ն�X֫����֫
		pFace = pBody->faceList.append();
		pFace->material = crCutFaceOrPolyJoint;
		createFace.NewOutterEdgeLine(pFace,10,11);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,11);
	}
	else if(end_cut_wing_status==2)
	{	//�ն�Y֫����֫
		pFace = pBody->faceList.append();
		pFace->material = crCutFaceOrPolyJoint;
		createFace.NewOutterEdgeLine(pFace,7,8);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
		createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
		createFace.NewOutterEdgeLine(pFace,8);
	}
	f3dPoint plane_norm,plane_pick,vec1,vec2;
	
	if(cut_wing[0]>0&&start_cut_wing_status==0)	//������ʼ������֫
	{
		plane_pick.z = -startOdd()+cut_wing_para[0][0];
		double offset_dist=0;
		//����֫����cut_wing_para[...][2]Ϊ0ʱ��������ʵ����ʾ wht 10-07-30
		if(fabs(cut_wing_para[0][2])<EPS2)
			offset_dist=1;	
		if(cut_wing[0]==1)	//X֫Ϊ�н���֫
		{
			vec1.Set(size.wide,0,offset_dist+cut_wing_para[0][2]-cut_wing_para[0][0]);
			vec2.Set(xWingVecY.x*cut_wing_para[0][1],
				xWingVecY.y*cut_wing_para[0][1],-cut_wing_para[0][0]);
			plane_norm = vec2^vec1;
			if(plane_norm.z>0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
				plane_norm = -plane_norm;
			pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
		}
		else	//Y֫Ϊ�н���֫
		{
			vec1.Set(xWingVecY.x*size.wide,size.wide*xWingVecY.y,
				offset_dist+cut_wing_para[0][2]-cut_wing_para[0][0]);
			vec2.Set(cut_wing_para[0][1],0,-cut_wing_para[0][0]);
			plane_norm = vec1^vec2;
			if(plane_norm.z>0)
				plane_norm = -plane_norm;
			pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
		}
	}
	if(cut_wing[1]>0&&end_cut_wing_status==0)		//������ֹ������֫
	{
		plane_pick.z = length+endOdd()-cut_wing_para[1][0];
		double offset_dist=0;
		//����֫����cut_wing_para[...][2]Ϊ0ʱ��������ʵ����ʾ wht 10-07-30
		if(fabs(cut_wing_para[1][2])<EPS2)
			offset_dist=1;	
		if(cut_wing[1]==1)	//X֫Ϊ�н���֫
		{
			vec1.Set(size.wide,0,-offset_dist-cut_wing_para[1][2]+cut_wing_para[1][0]);
			vec2.Set(xWingVecY.x*cut_wing_para[1][1],
				xWingVecY.y*cut_wing_para[1][1],cut_wing_para[1][0]);
			plane_norm = vec1^vec2;
			if(plane_norm.z<0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
				plane_norm = -plane_norm;
			pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
		}
		else	//Y֫Ϊ�н���֫
		{
			vec1.Set(xWingVecY.x*size.wide,size.wide*xWingVecY.y,-offset_dist-cut_wing_para[1][2]+cut_wing_para[1][0]);
			vec2.Set(cut_wing_para[1][1],0,cut_wing_para[1][0]);
			
			plane_norm = vec2^vec1;
			if(plane_norm.z<0)
				plane_norm = -plane_norm;
			pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
		}
	}
	//������ʼ����X֫�н�
	if(cut_angle[0][0]>0)	//X֫��֫
	{
		plane_pick.Set(size.wide-cut_angle[0][1],0,-startOdd());
		vec1.Set(cut_angle[0][1],0,cut_angle[0][0]);
		vec2.Set(0,1,0);
		plane_norm = vec2^vec1;
		if(plane_norm.z>0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
			plane_norm = -plane_norm;
		pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
	}
	//������ʼ����Y֫�н�
	if(cut_angle[1][0]>0)	//Y֫��֫
	{
		plane_pick.Set((size.wide-cut_angle[1][1])*xWingVecY.x,
			(size.wide-cut_angle[1][1])*xWingVecY.y,-startOdd());
		vec1.Set(cut_angle[1][1]*xWingVecY.x,cut_angle[1][1]*xWingVecY.y,
			cut_angle[1][0]);
		plane_norm = xHeJiaoNormY^vec1;
		if(plane_norm.z>0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
			plane_norm = -plane_norm;
		pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
	}
	//������ֹ����X֫�н�
	if(cut_angle[2][0]>0)	//X֫��֫
	{
		plane_pick.Set(size.wide-cut_angle[2][1],0,length+endOdd());
		vec1.Set(cut_angle[2][1],0,-cut_angle[2][0]);
		vec2.Set(0,1,0);
		plane_norm = vec1^vec2;
		if(plane_norm.z<0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
			plane_norm = -plane_norm;
		pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
	}
	//������ֹ����Y֫�н�
	if(cut_angle[3][0]>0)	//Y֫��֫
	{
		plane_pick.Set((size.wide-cut_angle[3][1])*xWingVecY.x,
			(size.wide-cut_angle[3][1])*xWingVecY.y,
			length+endOdd());
		vec1.Set(cut_angle[3][1]*xWingVecY.x,cut_angle[3][1]*xWingVecY.y,
			-cut_angle[3][0]);
		plane_norm = vec1^xHeJiaoNormY;
		if(plane_norm.z<0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
			plane_norm = -plane_norm;
		pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
	}
	if(CLDSLineAngle::m_bDisplaySolidAxisLine)
	{	//��ʾ�Ǹ����� wht 10-12-27
		int j=0;
		double offset_dist=0.5;
		for(i=0;i<2;i++)
		{
			JGZJ jgZj;
			getjgzj(jgZj,GetWidth());
			if(m_bEnableTeG)
				jgZj=xWingXZhunJu;
			COLORREF markClr=RGB(255,0,0);
			f3dPoint cur_wing_vec,cur_wing_norm;
			cur_wing_norm=xHeJiaoNormX;
			cur_wing_vec=xWingVecX;
			if(i==1)
			{
				markClr=RGB(0,255,0);
				cur_wing_norm=xHeJiaoNormY;
				cur_wing_vec=xWingVecY;
				if(m_bEnableTeG)
					jgZj=xWingYZhunJu;
			}
			f3dPoint start_vertex(0,0,-startOdd()),end_vertex(0,0,length+endOdd());
			start_vertex+=cur_wing_vec*jgZj.g;
			end_vertex+=cur_wing_vec*jgZj.g;
			vertex_arr[0] = start_vertex+cur_wing_norm*offset_dist;
			vertex_arr[1] = end_vertex+cur_wing_norm*offset_dist;
			vertex_arr[2] = end_vertex;
			vertex_arr[3] = start_vertex;
			int iMarkStartIndex=pBody->vertex.GetNodeNum();
			for(j=0;j<4;j++)
			{
				if(i==0)
				{
					pBody->vertex.append(vertex_arr[j]+cur_wing_vec*offset_dist);
					pBody->vertex.append(vertex_arr[j]-cur_wing_vec*offset_dist);
				}
				else
				{
					pBody->vertex.append(vertex_arr[j]-cur_wing_vec*offset_dist);
					pBody->vertex.append(vertex_arr[j]+cur_wing_vec*offset_dist);
				}
			}
			//���³�ʼ��
			createFace.InitVertexList(pBody);
			//��׼�涥��
			f3dPolyFace *pTopDatumFace=pBody->faceList.append();
			pTopDatumFace->material=markClr;
			for(j=0;j<4;j++)
				createFace.NewOutterEdgeLine(pTopDatumFace,iMarkStartIndex+((j+1)%4)*2+1,iMarkStartIndex+j*2+1);
			//��׼�����
			f3dPolyFace *pBottomDatumFace=pBody->faceList.append();
			pBottomDatumFace->material=markClr;
			for(j=4-1;j>=0;j--)
				createFace.NewOutterEdgeLine(pBottomDatumFace,iMarkStartIndex+j*2,iMarkStartIndex+((j+1)%4)*2);
			//��׼�����
			pFace;
			for(j=0;j<4;j++)
			{
				pFace=pBody->faceList.append();
				pFace->material=markClr;
				createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+((j+1)%4)*2,iMarkStartIndex+j*2);
				createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+((j+1)%4)*2+1);
				createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+j*2+1);
				createFace.NewOutterEdgeLine(pFace,iMarkStartIndex+j*2);
			}
		}
	}*/
	//�Ǹֽ��Ӵ���ʾ����
	UpdateInnerCircleSlopeFace(pBody,xWingVecX,xWingVecY,xHeJiaoNormY);
	for(f3dPoint *pPt=pBody->vertex.GetFirst();pPt;pPt=pBody->vertex.GetNext())
	{
		if(!coord_trans(*pPt,ucs,TRUE,TRUE))
		{
			char sError[100];
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(sError,"0X%X angle's coordinate error converts",handle);
#else
			sprintf(sError,"���Ϊ0X%X�ĽǸ�����ת������",handle);
#endif
			throw sError;
		}
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}
//�Ǹ�֫��ʾԲ����
BOOL CLDSLineAngle::UpdateWingFilletArcFace(fBody* pBody,f3dPoint wing_x_vec,f3dPoint wing_y_vec,f3dPoint norm_Y)
{
	COLORREF crAngleWingX=GetColor(CLS_LINEANGLE,'X',cMaterial);
	COLORREF crAngleWingY=GetColor(CLS_LINEANGLE,'Y',cMaterial);
	COLORREF crAngleEdge=GetColor(CLS_LINEANGLE,'E',cMaterial);
	if(pStart==NULL||pEnd==NULL)
		crAngleWingX=crAngleWingY=crAngleEdge=GetColor(CLS_LINEANGLE,'S',cMaterial);
	else if(group_father_jg_h>0x20)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pGroupJg&&pGroupJg->m_hDatumSonAngle!=handle)
		{	//��1���ӽǸְ�ָ����ɫ��������,1���ӽǸֱ�������ͨ�Ǹ���ɫһ�� wht 11-07-16
			crAngleWingX=GetColor(CLS_LINEANGLE,'x',cMaterial);
			crAngleWingY=GetColor(CLS_LINEANGLE,'y',cMaterial);
			crAngleEdge=GetColor(CLS_LINEANGLE,'e',cMaterial);
		}
	}
	if(m_bUserSpecColor)	//�û�ָ��������ɫ
		crAngleWingX=crAngleWingY=crAngleEdge=crMaterial;
	//
	double length = DISTANCE(Start(),End());
	for(int i=0;i<4;i++)
	{
		COLORREF crAngleWing;
		double fR=0;
		f3dPoint ptWC,ptBC,wingNorm;
		if(i==0 && (fR=dfFilletROfSX)>0)
		{	//ʼ��X֫��Բ��
			wingNorm.Set(0,-1,0);
			ptWC.Set((size_wide-fR)*wing_x_vec.x,(size_wide-fR)*wing_x_vec.y,-startOdd());
			ptBC.Set(size_wide*wing_x_vec.x,size_wide*wing_x_vec.y,-startOdd()+fR);
			crAngleWing=crAngleWingX;
		}
		else if(i==1 && (fR=dfFilletROfSY)>0)
		{	//ʼ��Y֫��Բ��
			wingNorm=norm_Y;
			ptWC.Set((size_wide-fR)*wing_y_vec.x,(size_wide-fR)*wing_y_vec.y,-startOdd());
			ptBC.Set(size_wide*wing_y_vec.x,size_wide*wing_y_vec.y,-startOdd()+fR);
			crAngleWing=crAngleWingY;
		}
		else if(i==2 && (fR=dfFilletROfEX)>0)
		{	//�ն�X֫��Բ��
			wingNorm.Set(0,-1,0);
			ptWC.Set((size_wide-fR)*wing_x_vec.x,(size_wide-fR)*wing_x_vec.y,length+endOdd());
			ptBC.Set(size_wide*wing_x_vec.x,size_wide*wing_x_vec.y,length+endOdd()-fR);
			crAngleWing=crAngleWingX;
		}
		else if(i==3 && (fR=dfFilletROfEY)>0)
		{	//�ն�Y֫��Բ��
			wingNorm=norm_Y;
			ptWC.Set((size_wide-fR)*wing_y_vec.x,(size_wide-fR)*wing_y_vec.y,length+endOdd());
			ptBC.Set(size_wide*wing_y_vec.x,size_wide*wing_y_vec.y,length+endOdd()-fR);
			crAngleWing=crAngleWingY;
		}
		//���ݵ��ǿ�Ƚ����и�,ɾ���и���
		if(fR<=0)
			continue;
		f3dPoint plane_pick=ptWC;
		f3dPoint cut_vec=(ptBC-ptWC).normalized();
		f3dPoint plane_norm=wingNorm^cut_vec;
		if((i<2&&plane_norm.z>0)||(i>=2&&plane_norm.z<0))
			plane_norm = -plane_norm;
		pBody->CutByPlane(plane_pick,plane_norm,0,false);
		//����Բ����
		f3dPoint ptS(ptWC.x,ptWC.y,ptBC.z);
		f3dPoint ptE=ptS-wingNorm*10;
		int slices=16;
		double slice_angle=0.5*Pi/(slices-1);
		f3dPoint vertex_arr[50];
		if(i==0||i==3)
			vertex_arr[0]=ptBC;
		else
			vertex_arr[0]=ptWC;
		for(int j=1;j<slices;j++)
		{
			vertex_arr[j]=vertex_arr[j-1];
			rotate_point_around_axis(vertex_arr[j],slice_angle,ptS,ptE);
		}
		//ʵ�������Բ��������
		int iStartIndex=pBody->vertex.GetNodeNum();
		for(int j=0;j<slices;j++)
		{
			f3dPoint vertice=vertex_arr[j];
			pBody->vertex.append(vertice);
			vertice-=size_thick*wingNorm;
			pBody->vertex.append(vertice);
		}
		//��ʼ������
		CCreateFace createFace;
		createFace.InitVertexList(pBody);
		//����
		f3dPolyFace *pTopFace=pBody->faceList.append();
		pTopFace->material=crAngleWing;
		for(int ii=0;ii<slices;ii++)
		{
			if(ii==0)
				createFace.NewOutterEdgeLine(pTopFace,iStartIndex+((ii+1)%slices)*2+1,iStartIndex+1);
			else
				createFace.NewOutterEdgeLine(pTopFace,iStartIndex+((ii+1)%slices)*2+1);
		}
		//����
		f3dPolyFace *pBtmFace=pBody->faceList.append();
		pBtmFace->material=crAngleWing;
		for(int ii=0;ii<slices;ii++)
		{
			if(ii==0)
				createFace.NewOutterEdgeLine(pBtmFace,iStartIndex+(slices-1-ii)*2,iStartIndex+0);
			else
				createFace.NewOutterEdgeLine(pBtmFace,iStartIndex+(slices-1-ii)*2);
		}
		//����
		for(int ii=0;ii<slices;ii++)
		{
			f3dPolyFace *pFace=pBody->faceList.append();
			pFace->material=crAngleEdge;
			createFace.NewOutterEdgeLine(pFace,iStartIndex+((ii+1)%slices)*2,iStartIndex+ii*2);
			createFace.NewOutterEdgeLine(pFace,iStartIndex+((ii+1)%slices)*2+1);
			createFace.NewOutterEdgeLine(pFace,iStartIndex+ii*2+1);
			createFace.NewOutterEdgeLine(pFace,iStartIndex+ii*2);
		}
	}
	return TRUE;
}
BOOL CLDSLineAngle::Create3dSolidModel(BOOL bDisplayHole,double scale_of_user2scr,double sample_len,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;
	else
	{
		fBody body;
		fBody* pBody=&body;
		if(pSolidBody!=NULL && !IsSolidModified())	//����δ�޸�
			return TRUE;
		else
		{
			if(pSolidBody==NULL)
				pSolidBody = new CSolidBody;
			else
				pSolidBody->Empty();
			SetModified(FALSE);
			if(norm_x_wing.IsZero())
			{	//�Ǹ���֫�����غ�,���ݴ���,��ʱ�������һ����֫����
				f3dPoint len_vec=End()-Start();
				norm_x_wing=inters_vec(len_vec);
				norm_x_wing=norm_x_wing^len_vec;
				normalize(norm_x_wing);
				des_norm_x.spec_norm.vector=norm_x_wing;
			}
			getUCS(ucs);
			f3dPoint vertex_arr[12],norm_X,norm_Y,wing_x_vec,wing_y_vec,bi_vec;
			if(norm_y_wing.IsZero())
			{
				norm_y_wing=-ucs.axis_x;
				des_norm_y.spec_norm.vector=norm_y_wing;
			}
			if(fabs(norm_x_wing*norm_y_wing)>0.9962)	//cos(5��)=0.9962
			{
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"The angle between two leg of 0X%X angle part is invalid, leg direction specified��again��",handle);
#else
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹ���֫�нǲ��ϳ���,������ָ���Ǹֵ�֫����",handle);
#endif
				norm_y_wing=-ucs.axis_x;
				des_norm_y.spec_norm.vector=norm_y_wing;
			}
			f3dPoint verify_z=norm_y_wing^norm_x_wing;
			if(verify_z*ucs.axis_z<0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"Conflict occured between two leg of 0X%Xangle, leg direction specified��again��",handle);
#else
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X�Ǹ���֫������Υ��,������ָ���Ǹֵ�֫����",handle);
#endif
				DESIGN_JGWING_NORM des_wing_norm=des_norm_x;
				des_norm_x=des_norm_y;
				des_norm_y=des_wing_norm;
				f3dPoint wing_norm=norm_x_wing;
				norm_x_wing=norm_y_wing;
				norm_y_wing=wing_norm;
			}
			//���Y֫�����������ϵ�µķ���
			norm_Y = get_norm_y_wing();
			vector_trans(norm_Y,ucs,FALSE);
			norm_Y.z=0;
			normalize(norm_Y);
			//���Y֫�������ϵ�µ����췽��
			wing_x_vec.Set(1,0,0);
			wing_y_vec.Set(norm_Y.y,-norm_Y.x);
			if(fabs(wing_y_vec.y)<EPS)
			{	//��֫������غ�ʱ���ܻᵼ�³�0�쳣 wjh 2011-4-29
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0X%X angle's two leg coincide or are 180��coplanar,data error",handle);
#else
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"���Ϊ0X%X�ĽǸ���֫�غϻ�180�㹲�棬ϵͳ�Զ�������֫�н�Ϊ90��",handle);
				if(!des_norm_x.bByOtherWing&&group_father_jg_h<0x20&&!norm_x_wing.IsZero())
					des_norm_y.bByOtherWing=true;	//��ϽǸ��ӽǸ�Ӧ������������������Ǹֿ��ƣ�������Ӳ�Ը�Ϊ��ֱ����һ֫
				cal_y_wing_norm(true);
				wing_y_vec.Set(0,1,0);
				norm_Y.Set(-1,0,0);
#endif
			}
			norm_X.Set(0,-1,0);
			if(wing_y_vec.x>0&&m_fHeJiaoReservedWidth>0)	//�Ͻ�ʱ��Ҫ����һ���ĺϽǿ��
				return CreateSpecialHeJiaoSolidModel(pBody,bDisplayHole);
			bi_vec.Set(1+wing_y_vec.x,wing_y_vec.y,0);
			normalize(bi_vec);
			if(start_push_flat_x1_y2!=1&&start_push_flat_x1_y2!=2)
				start_push_flat_length=0;
			if(end_push_flat_x1_y2!=1&&end_push_flat_x1_y2!=2)
				end_push_flat_length=0;
			//hxr 2012-10-09
			if(mid_push_flat_x1_y2!=1&&mid_push_flat_x1_y2!=2)
			{
				_mid_push_flat_length=0;
				_mid_push_flat_up_offset=0;
			}
			double length = DISTANCE(Start(),End());
			vertex_arr[0].x = 0;
			vertex_arr[0].y = 0;
			vertex_arr[0].z = -startOdd()+start_push_flat_length;

			double wing_wide_y=size_wide;
			if(IsUnequalAngle())//���ȱ߽Ǹ�
				wing_wide_y = size_height;
			double SIZE_WIDTH_OF_LEGX=size.wide;
			double SIZE_WIDTH_OF_LEGY=wing_wide_y;
			vertex_arr[1].x = wing_wide_y*wing_y_vec.x;
			vertex_arr[1].y = wing_wide_y*wing_y_vec.y;
			vertex_arr[1].z = -startOdd()+start_push_flat_length;

			vertex_arr[2].x = wing_wide_y*wing_y_vec.x-size_thick*norm_Y.x;
			vertex_arr[2].y = wing_wide_y*wing_y_vec.y-size_thick*norm_Y.y;
			vertex_arr[2].z = -startOdd()+start_push_flat_length;

			vertex_arr[3].x = size_thick*bi_vec.x/bi_vec.y;
			vertex_arr[3].y = size_thick;
			vertex_arr[3].z = -startOdd()+start_push_flat_length;

			vertex_arr[4].x = size_wide;
			vertex_arr[4].y = size_thick;
			vertex_arr[4].z = -startOdd()+start_push_flat_length;

			vertex_arr[5].x = size_wide;
			vertex_arr[5].y = 0;
			vertex_arr[5].z = -startOdd()+start_push_flat_length;

			int i;
			for(i=0;i<6;i++)
				vertex_arr[i+6].Set(vertex_arr[i].x,vertex_arr[i].y,length+endOdd()-end_push_flat_length);
			f3dPoint push_flat_vertex_arr[16],start_cut_wing_vertex_arr[3],end_cut_wing_vertex_arr[3];
			int start_cut_wing_vertex_arr_index[3],end_cut_wing_vertex_arr_index[3];
			int start_cut_wing_status=0,end_cut_wing_status=0;	//0��ʾ������֫���;1��ʾX֫����֫;2��ʾY֫����֫
			if(start_push_flat_x1_y2==1)	//ʼ��X֫ѹ��
			{
				vertex_arr[4].z+=(size_wide+size_thick)*2;
				vertex_arr[5].z+=size_wide*2;
				push_flat_vertex_arr[0].Set(0,0,-startOdd());
				push_flat_vertex_arr[1].Set(size_wide*wing_y_vec.x,size_wide*wing_y_vec.y,-startOdd());
				push_flat_vertex_arr[2].Set(size_wide*wing_y_vec.x-2.0*size_thick*norm_Y.x,size_wide*wing_y_vec.y-2.0*size_thick*norm_Y.y,-startOdd());
				push_flat_vertex_arr[3]=push_flat_vertex_arr[2]-wing_y_vec*(size_wide-size_thick);
				push_flat_vertex_arr[4].Set(-norm_Y.x*size_thick,-norm_Y.y*size_thick,-startOdd());
				push_flat_vertex_arr[5]=push_flat_vertex_arr[2]+f3dPoint(0,0,start_push_flat_length);
				push_flat_vertex_arr[6]=push_flat_vertex_arr[3]+f3dPoint(0,0,start_push_flat_length);
				push_flat_vertex_arr[7]=push_flat_vertex_arr[4]+f3dPoint(0,0,start_push_flat_length);
			}
			else if(start_push_flat_x1_y2==2)//Y֫ѹ��
			{
				vertex_arr[1].z+=size_wide*2;
				vertex_arr[2].z+=size_wide*2+size_thick;
				push_flat_vertex_arr[0].Set(0,0,-startOdd());
				push_flat_vertex_arr[1].Set(size_wide*wing_x_vec.x,size_wide*wing_x_vec.y,-startOdd());
				push_flat_vertex_arr[2].Set(size_wide*wing_x_vec.x-2.0*size_thick*norm_X.x,size_wide*wing_x_vec.y-2.0*size_thick*norm_X.y,-startOdd());
				push_flat_vertex_arr[3]=push_flat_vertex_arr[2]-wing_x_vec*(size_wide-size_thick);
				push_flat_vertex_arr[4].Set(-norm_X.x*size_thick,-norm_X.y*size_thick,-startOdd());
				push_flat_vertex_arr[5]=push_flat_vertex_arr[2]+f3dPoint(0,0,start_push_flat_length);
				push_flat_vertex_arr[6]=push_flat_vertex_arr[3]+f3dPoint(0,0,start_push_flat_length);
				push_flat_vertex_arr[7]=push_flat_vertex_arr[4]+f3dPoint(0,0,start_push_flat_length);
			}
			else if(cut_wing[0]==1&&cut_wing_para[0][1]<=GetThick()&&
				   (cut_wing_para[0][0]+cut_wing_para[0][1]+cut_wing_para[0][2])>0)
			{	//ʼ��X֫����֫
				start_cut_wing_status=1;
				start_cut_wing_vertex_arr[0]= vertex_arr[3];
				start_cut_wing_vertex_arr[0].z += cut_wing_para[0][0];
				vertex_arr[4].z+=cut_wing_para[0][2];
				vertex_arr[5].z+=cut_wing_para[0][2];
				f3dPoint line_vec=vertex_arr[3]-vertex_arr[2];
				Int3dlf(start_cut_wing_vertex_arr[1],start_cut_wing_vertex_arr[0],line_vec,vertex_arr[0],norm_X);
				Int3dlf(start_cut_wing_vertex_arr[2],vertex_arr[3],line_vec,vertex_arr[0],norm_X);
			}
			else if(cut_wing[0]==2&&cut_wing_para[0][1]<=GetThick()&&
					(cut_wing_para[0][0]+cut_wing_para[0][1]+cut_wing_para[0][2])>0)
			{	//ʼ��Y֫����֫
				start_cut_wing_status=2;
				start_cut_wing_vertex_arr[0]= vertex_arr[3];
				start_cut_wing_vertex_arr[0].z += cut_wing_para[0][0];
				vertex_arr[1].z+=cut_wing_para[0][2];
				vertex_arr[2].z+=cut_wing_para[0][2];
				f3dPoint line_vec=vertex_arr[3]-vertex_arr[4];
				Int3dlf(start_cut_wing_vertex_arr[1],start_cut_wing_vertex_arr[0],line_vec,vertex_arr[0],norm_Y);
				Int3dlf(start_cut_wing_vertex_arr[2],vertex_arr[3],line_vec,vertex_arr[0],norm_Y);
			}
			if(end_push_flat_x1_y2==1)//�ն�X֫ѹ��
			{
				vertex_arr[10].z-=size_wide*2+size_thick;
				vertex_arr[11].z-=size_wide*2;
				push_flat_vertex_arr[8].Set(0,0,length+endOdd());
				push_flat_vertex_arr[9].Set(size_wide*wing_y_vec.x,size_wide*wing_y_vec.y,length+endOdd());
				push_flat_vertex_arr[10].Set(size_wide*wing_y_vec.x-2.0*size_thick*norm_Y.x,size_wide*wing_y_vec.y-2.0*size_thick*norm_Y.y,length+endOdd());
				push_flat_vertex_arr[11]=push_flat_vertex_arr[10]-wing_y_vec*(size_wide-size_thick);
				push_flat_vertex_arr[12].Set(-norm_Y.x*size_thick,-norm_Y.y*size_thick,length+endOdd());
				push_flat_vertex_arr[13]=push_flat_vertex_arr[10]+f3dPoint(0,0,-end_push_flat_length);
				push_flat_vertex_arr[14]=push_flat_vertex_arr[11]+f3dPoint(0,0,-end_push_flat_length);
				push_flat_vertex_arr[15]=push_flat_vertex_arr[12]+f3dPoint(0,0,-end_push_flat_length);
			}
			else if(end_push_flat_x1_y2==2)//Y֫ѹ��
			{
				vertex_arr[7].z-=size_wide*2;
				vertex_arr[8].z-=size_wide*2+size_thick;
				push_flat_vertex_arr[8].Set(0,0,length+endOdd());
				push_flat_vertex_arr[9].Set(size_wide*wing_x_vec.x,size_wide*wing_x_vec.y,length+endOdd());
				push_flat_vertex_arr[10].Set(size_wide*wing_x_vec.x-2.0*size_thick*norm_X.x,size_wide*wing_x_vec.y-2.0*size_thick*norm_X.y,length+endOdd());
				push_flat_vertex_arr[11]=push_flat_vertex_arr[10]-wing_x_vec*(size_wide-size_thick);
				push_flat_vertex_arr[12].Set(-norm_X.x*size_thick,-norm_X.y*size_thick,length+endOdd());
				push_flat_vertex_arr[13]=push_flat_vertex_arr[10]+f3dPoint(0,0,-end_push_flat_length);
				push_flat_vertex_arr[14]=push_flat_vertex_arr[11]+f3dPoint(0,0,-end_push_flat_length);
				push_flat_vertex_arr[15]=push_flat_vertex_arr[12]+f3dPoint(0,0,-end_push_flat_length);
			}
			else if(cut_wing[1]==1&&cut_wing_para[1][1]<=GetThick()&&
					(cut_wing_para[1][0]+cut_wing_para[1][1]+cut_wing_para[1][2])>0)
			{	//�ն�X֫����֫
				end_cut_wing_status=1;
				end_cut_wing_vertex_arr[0]= vertex_arr[9];
				end_cut_wing_vertex_arr[0].z -= cut_wing_para[1][0];
				vertex_arr[10].z-=cut_wing_para[1][2];
				vertex_arr[11].z-=cut_wing_para[1][2];
				f3dPoint line_vec=vertex_arr[9]-vertex_arr[8];
				Int3dlf(end_cut_wing_vertex_arr[1],end_cut_wing_vertex_arr[0],line_vec,vertex_arr[6],norm_X);
				Int3dlf(end_cut_wing_vertex_arr[2],vertex_arr[9],line_vec,vertex_arr[6],norm_X);
			}
			else if(cut_wing[1]==2&&cut_wing_para[1][1]<=GetThick()&&
					(cut_wing_para[1][0]+cut_wing_para[1][1]+cut_wing_para[1][2])>0)
			{	//�ն�Y֫����֫
				end_cut_wing_status=2;
				end_cut_wing_vertex_arr[0]= vertex_arr[9];
				end_cut_wing_vertex_arr[0].z -= cut_wing_para[1][0];
				vertex_arr[7].z-=cut_wing_para[1][2];
				vertex_arr[8].z-=cut_wing_para[1][2];
				f3dPoint line_vec=vertex_arr[9]-vertex_arr[10];
				Int3dlf(end_cut_wing_vertex_arr[1],end_cut_wing_vertex_arr[0],line_vec,vertex_arr[6],norm_Y);
				Int3dlf(end_cut_wing_vertex_arr[2],vertex_arr[9],line_vec,vertex_arr[6],norm_Y);
			}
			for(i=0;i<12;i++)
				pBody->vertex.append(vertex_arr[i]);
			//hxr 2012-9-29
			int midPushFlatX1Y2=mid_push_flat_x1_y2;
			f3dPoint mid_push_flat_vertex_arr[14],mid_push_flat_pos;
			if(midPushFlatX1Y2>0)
			{
				CLDSNode *pMidPushFlatNode=BelongModel()->FromNodeHandle(mid_push_flat_node_handle);
				if(pMidPushFlatNode)
					mid_push_flat_pos=GetDatumPos(pMidPushFlatNode);
					//SnapPerp(&mid_push_flat_pos,Start(),End(),pMidPushFlatNode->Position(false));
				else 
					midPushFlatX1Y2=0;
			}
			double len_starttomid = startOdd()+DISTANCE(Start(),mid_push_flat_pos);
			int mid_push_flat_vertex_arr_index[14];
			if (midPushFlatX1Y2==1)	//�м�X֫ѹ��
			{
				mid_push_flat_vertex_arr[0] = f3dPoint(wing_wide_y*wing_y_vec.x-size_thick*norm_Y.x,wing_wide_y*wing_y_vec.y-size_thick*norm_Y.y,
					-startOdd()+mid_push_flat_end_offset+len_starttomid);
				mid_push_flat_vertex_arr[1] = f3dPoint(size_wide*wing_y_vec.x-2.0*size_thick*norm_Y.x,size_wide*wing_y_vec.y-2.0*size_thick*norm_Y.y,
					-startOdd()+mid_push_flat_end_offset+len_starttomid);
				mid_push_flat_vertex_arr[2] = f3dPoint(size_wide*wing_y_vec.x-2.0*size_thick*norm_Y.x,size_wide*wing_y_vec.y-2.0*size_thick*norm_Y.y,
					-startOdd()+mid_push_flat_end_offset+len_starttomid)-wing_y_vec*(size_wide-size_thick);
				mid_push_flat_vertex_arr[3] = f3dPoint(-norm_Y.x*size_thick,-norm_Y.y*size_thick,
					-startOdd()+mid_push_flat_end_offset+len_starttomid);
				mid_push_flat_vertex_arr[4] = f3dPoint(size_wide,         0,-startOdd()+mid_push_flat_end_offset+len_starttomid+size_wide*2);
				mid_push_flat_vertex_arr[5] = f3dPoint(size_wide,size_thick,-startOdd()+mid_push_flat_end_offset+len_starttomid+(size_wide+size_thick)*2);
				mid_push_flat_vertex_arr[6] = f3dPoint(size_thick*bi_vec.x/bi_vec.y,size_thick,-startOdd()+mid_push_flat_end_offset+len_starttomid);
			}
			else if (midPushFlatX1Y2==2)	//�м�Y֫ѹ��
			{
				mid_push_flat_vertex_arr[0] = f3dPoint(size_wide,size_thick,-startOdd()+mid_push_flat_end_offset+len_starttomid);
				mid_push_flat_vertex_arr[1] = f3dPoint(size_wide*wing_x_vec.x-2.0*size_thick*norm_X.x,size_wide*wing_x_vec.y-2.0*size_thick*norm_X.y,
					-startOdd()+mid_push_flat_end_offset+len_starttomid);
				mid_push_flat_vertex_arr[2] = f3dPoint(size_wide*wing_x_vec.x-2.0*size_thick*norm_X.x,size_wide*wing_x_vec.y-2.0*size_thick*norm_X.y,
					-startOdd()+mid_push_flat_end_offset+len_starttomid)-wing_x_vec*(size_wide-size_thick);
				mid_push_flat_vertex_arr[3] = f3dPoint(-norm_X.x*size_thick,-norm_X.y*size_thick,
					-startOdd()+mid_push_flat_end_offset+len_starttomid);
				mid_push_flat_vertex_arr[4] = f3dPoint(wing_wide_y*wing_y_vec.x,wing_wide_y*wing_y_vec.y,
					-startOdd()+mid_push_flat_end_offset+size_wide*2+len_starttomid);
				mid_push_flat_vertex_arr[5] = f3dPoint(wing_wide_y*wing_y_vec.x-size_thick*norm_Y.x,wing_wide_y*wing_y_vec.y-size_thick*norm_Y.y,
					-startOdd()+mid_push_flat_end_offset+size_wide*2+size_thick+len_starttomid);
				mid_push_flat_vertex_arr[6] = f3dPoint(size_thick*bi_vec.x/bi_vec.y,size_thick,
					-startOdd()+mid_push_flat_end_offset+len_starttomid);
			}
			if (midPushFlatX1Y2>0)
			{
				mid_push_flat_vertex_arr[7] = mid_push_flat_vertex_arr[0]+f3dPoint(0,0,-mid_push_flat_length);
				mid_push_flat_vertex_arr[8] = mid_push_flat_vertex_arr[1]+f3dPoint(0,0,-mid_push_flat_length);
				mid_push_flat_vertex_arr[9] = mid_push_flat_vertex_arr[2]+f3dPoint(0,0,-mid_push_flat_length);
				mid_push_flat_vertex_arr[10] = mid_push_flat_vertex_arr[3]+f3dPoint(0,0,-mid_push_flat_length);
				mid_push_flat_vertex_arr[11] = mid_push_flat_vertex_arr[4]+
					f3dPoint(0,0,-2*fabs(mid_push_flat_vertex_arr[4].z-mid_push_flat_vertex_arr[0].z)-mid_push_flat_length);
				mid_push_flat_vertex_arr[12] = mid_push_flat_vertex_arr[5]+
					f3dPoint(0,0,-2*fabs(mid_push_flat_vertex_arr[5].z-mid_push_flat_vertex_arr[0].z)-mid_push_flat_length);
				mid_push_flat_vertex_arr[13] = mid_push_flat_vertex_arr[6]+f3dPoint(0,0,-mid_push_flat_length);
			}

			//wht 2007-10-24
			//��ʼ�˲�ѹ��ҲҪ��ǰ8��������뵽�����У�������ֹ��ѹ��Ͳ��÷ֿ�������
			if(start_push_flat_x1_y2>0||end_push_flat_x1_y2>0)
			{
				for(i=0;i<16;i++)
					pBody->vertex.append(push_flat_vertex_arr[i]);
			}
			//hxr 2012-9-29
			if (midPushFlatX1Y2>0)
			{
				for(int ii=0;ii<14;ii++)
				{
					pBody->vertex.append(mid_push_flat_vertex_arr[ii]);
					mid_push_flat_vertex_arr_index[ii] = pBody->vertex.GetCurrentIndex();
				}
			}
			if(start_cut_wing_status!=0)
			{	//ʼ������֫
				for(int kk=0;kk<3;kk++)
				{
					pBody->vertex.append(start_cut_wing_vertex_arr[kk]);
					start_cut_wing_vertex_arr_index[kk] = pBody->vertex.GetCurrentIndex();
				}
			}
			if(end_cut_wing_status!=0)
			{	//�ն�����֫
				for(int kk=0;kk<3;kk++)
				{
					pBody->vertex.append(end_cut_wing_vertex_arr[kk]);
					end_cut_wing_vertex_arr_index[kk] = pBody->vertex.GetCurrentIndex();
				}
			}

			ATOM_LIST<BOLTHOLE_INFO> x_wing_ls,y_wing_ls;
			CLsRef *pLsRef;
			for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				if(m_bDisplayHole || bDisplayHole|| pLsRef->GetLsPtr()->m_bVirtualPart)
				{	//ϵͳҪ����ʾ��˨�׻�ǰ��˨Ϊ������˨
					BOLTHOLE_INFO boltHole;
					boltHole.axis_x = (*pLsRef)->ucs.axis_x;
					boltHole.norm = (*pLsRef)->get_norm();
					boltHole.bolt_d = (*pLsRef)->get_d();
					boltHole.waistLen = pLsRef->waistLen;
					boltHole.hole_d = (*pLsRef)->hole_d_increment+(*pLsRef)->get_d();
					boltHole.centre = (*pLsRef)->ucs.origin;
					vector_trans(boltHole.axis_x,ucs,FALSE);
					coord_trans(boltHole.centre,ucs,FALSE,TRUE);
					vector_trans(boltHole.norm,ucs,FALSE);
					normalize(boltHole.norm);
					double ddx=norm_X*boltHole.norm;
					double ddy=norm_Y*boltHole.norm;
					//if(fabs(boltHole.norm.x)>fabs(boltHole.norm.y))	//Y֫��˨
					//���ڿ��Ͻ��ҿ��ϽǶȹ�����Сʱ��һ�д����жϲ�׼ȷ,�޸�Ϊ���� wht 10-11-29
					if(fabs(ddx)>fabs(ddy))	//X֫��˨
						x_wing_ls.append(boltHole);
					else					//Y֫��˨
						y_wing_ls.append(boltHole);
				}
			}
			f3dPolyFace *pFace,*pUpFace,*pDownFace,*pStartUpFlatFace,*pStartDownFlatFace,*pEndUpFlatFace,*pEndDownFlatFace;
			f3dPolyFace* pTopFaceOfLegX=NULL,*pBtmFaceOfLegX=NULL,*pTopFaceOfLegY=NULL,*pBtmFaceOfLegY=NULL;
			COLORREF crAngleWingX=GetColor(CLS_LINEANGLE,'X',cMaterial);
			COLORREF crAngleWingY=GetColor(CLS_LINEANGLE,'Y',cMaterial);
			COLORREF crAngleEdge=GetColor(CLS_LINEANGLE,'E',cMaterial);
			if(pStart==NULL||pEnd==NULL)
				crAngleWingX=crAngleWingY=crAngleEdge=GetColor(CLS_LINEANGLE,'S',cMaterial);
			else if(group_father_jg_h>0x20)
			{
				CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(group_father_jg_h,CLS_GROUPLINEANGLE);
				if(pGroupJg&&pGroupJg->m_hDatumSonAngle!=handle)
				{	//��1���ӽǸְ�ָ����ɫ��������,1���ӽǸֱ�������ͨ�Ǹ���ɫһ�� wht 11-07-16
					crAngleWingX=GetColor(CLS_LINEANGLE,'x',cMaterial);
					crAngleWingY=GetColor(CLS_LINEANGLE,'y',cMaterial);
					crAngleEdge=GetColor(CLS_LINEANGLE,'e',cMaterial);
				}
			}
			if(m_bUserSpecColor)	//�û�ָ��������ɫ
				crAngleWingX=crAngleWingY=crAngleEdge=crMaterial;
			//��ʼ������
			CCreateFace createFace;
			createFace.InitVertexList(pBody);

			if(start_push_flat_x1_y2<=0)	//ʼ�˲�ѹ��
			{
				//1.�Ǹֵ�һ������(��ʼ����)
				if(start_cut_wing_status==0)
				{	//ʼ�˲�����֫
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,1,0);
					createFace.NewOutterEdgeLine(pFace,2);
					createFace.NewOutterEdgeLine(pFace,3);
					createFace.NewOutterEdgeLine(pFace,4);
					createFace.NewOutterEdgeLine(pFace,5);
					createFace.NewOutterEdgeLine(pFace,0);
				}
				else if(start_cut_wing_status==1)
				{	//ʼ��X֫����֫
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,1,0);
					createFace.NewOutterEdgeLine(pFace,2);
					createFace.NewOutterEdgeLine(pFace,3);
					createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
					createFace.NewOutterEdgeLine(pFace,0);
				}
				else if(start_cut_wing_status==2)
				{	//ʼ��Y֫����֫
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2],0);
					createFace.NewOutterEdgeLine(pFace,3);
					createFace.NewOutterEdgeLine(pFace,4);
					createFace.NewOutterEdgeLine(pFace,5);
					createFace.NewOutterEdgeLine(pFace,0);
				}
			}
			else	//ʼ��ѹ�⴦��
			{
				if(start_push_flat_x1_y2==1)	//X֫ѹ��
				{
					//1.1ѹ���һ������(��ʼ����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingY;
					createFace.NewOutterEdgeLine(pFace,12+1,12+0);
					createFace.NewOutterEdgeLine(pFace,12+2);
					createFace.NewOutterEdgeLine(pFace,12+3);
					createFace.NewOutterEdgeLine(pFace,12+4);
					createFace.NewOutterEdgeLine(pFace,12+0);
					//1.2ѹ��ڶ�������(Y֫��ƽ��)
					pStartUpFlatFace=pFace = pBody->faceList.append();
					pFace->material = crAngleWingY;
					createFace.NewOutterEdgeLine(pFace,0,12+0);
					createFace.NewOutterEdgeLine(pFace,1);
					createFace.NewOutterEdgeLine(pFace,12+1);
					createFace.NewOutterEdgeLine(pFace,12+0);
					//1.3ѹ�����������(Y֫��ƽ��)
					pStartDownFlatFace=pFace = pBody->faceList.append();
					pFace->material = crAngleWingY;
					createFace.NewOutterEdgeLine(pFace,12+5,12+2);
					createFace.NewOutterEdgeLine(pFace,12+6);
					createFace.NewOutterEdgeLine(pFace,12+3);
					createFace.NewOutterEdgeLine(pFace,12+2);
					//1.4ѹ�����������(Y֫����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,12+5,1);
					createFace.NewOutterEdgeLine(pFace,12+2);
					createFace.NewOutterEdgeLine(pFace,12+1);
					createFace.NewOutterEdgeLine(pFace,1);
					//1.5ѹ�����������(X֫��ƽ��)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingY;
					createFace.NewOutterEdgeLine(pFace,12+0,0);
					createFace.NewOutterEdgeLine(pFace,12+4);
					createFace.NewOutterEdgeLine(pFace,12+7);
					createFace.NewOutterEdgeLine(pFace,0);
					//1.6ѹ�����������(X֫б����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingY;
					createFace.NewOutterEdgeLine(pFace,12+6,12+3);
					createFace.NewOutterEdgeLine(pFace,12+7);
					createFace.NewOutterEdgeLine(pFace,12+4);
					createFace.NewOutterEdgeLine(pFace,12+3);
					//1.7ѹ�����X֫��ƽ��
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingY;
					createFace.NewOutterEdgeLine(pFace,5,12+5);
					createFace.NewOutterEdgeLine(pFace,12+6);
					createFace.NewOutterEdgeLine(pFace,12+5);
					//1.8ѹ�����X֫��ƽ��
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingY;
					createFace.NewOutterEdgeLine(pFace,2,4);
					createFace.NewOutterEdgeLine(pFace,3);
					createFace.NewOutterEdgeLine(pFace,4);
					//1.9ѹ�����X֫������ƽ��1
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,2,12+5);
					createFace.NewOutterEdgeLine(pFace,5);
					createFace.NewOutterEdgeLine(pFace,12+5);
					//1.10ѹ�����X֫������ƽ��2
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,5,4);
					createFace.NewOutterEdgeLine(pFace,2);
					createFace.NewOutterEdgeLine(pFace,4);
					//1.11ѹ����ɱ���������ƽ��1
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,0,5);
					createFace.NewOutterEdgeLine(pFace,12+7);
					createFace.NewOutterEdgeLine(pFace,5);
					//1.12ѹ����ɱ���������ƽ��2
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,12+7,5);
					createFace.NewOutterEdgeLine(pFace,12+6);
					createFace.NewOutterEdgeLine(pFace,5);
				}
				else //if(start_push_flat_x1_y2==2)	//Y֫ѹ��
				{
					//1.1ѹ���һ������(��ʼ����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,12+4,12+0);
					createFace.NewOutterEdgeLine(pFace,12+3);
					createFace.NewOutterEdgeLine(pFace,12+2);
					createFace.NewOutterEdgeLine(pFace,12+1);
					createFace.NewOutterEdgeLine(pFace,12+0);
					//1.2ѹ��ڶ�������(X֫��ƽ��)
					pStartUpFlatFace = pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,12+0,0);
					createFace.NewOutterEdgeLine(pFace,12+1);
					createFace.NewOutterEdgeLine(pFace,5);
					createFace.NewOutterEdgeLine(pFace,0);
					//1.3ѹ�����������(X֫��ƽ��)
					pStartDownFlatFace = pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,12+3,12+2);
					createFace.NewOutterEdgeLine(pFace,12+6);
					createFace.NewOutterEdgeLine(pFace,12+5);
					createFace.NewOutterEdgeLine(pFace,12+2);
					//1.4ѹ�����������(X֫����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,12+1,5);
					createFace.NewOutterEdgeLine(pFace,12+2);
					createFace.NewOutterEdgeLine(pFace,12+5);
					createFace.NewOutterEdgeLine(pFace,5);
					//1.5ѹ�����������(Y֫��ƽ��)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,0,12+0);
					createFace.NewOutterEdgeLine(pFace,12+7);
					createFace.NewOutterEdgeLine(pFace,12+4);
					createFace.NewOutterEdgeLine(pFace,12+0);
					//1.6ѹ�����������(Y֫б����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,12+4,12+3);
					createFace.NewOutterEdgeLine(pFace,12+7);
					createFace.NewOutterEdgeLine(pFace,12+6);
					createFace.NewOutterEdgeLine(pFace,12+3);
					//1.7ѹ�����Y֫��ƽ��
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,12+5,1);
					createFace.NewOutterEdgeLine(pFace,12+6);
					createFace.NewOutterEdgeLine(pFace,1);
					//1.8ѹ�����Y֫��ƽ��
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,3,2);
					createFace.NewOutterEdgeLine(pFace,4);
					createFace.NewOutterEdgeLine(pFace,2);
					//1.9ѹ�����Y֫������ƽ��1
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,12+5,4);
					createFace.NewOutterEdgeLine(pFace,1);
					createFace.NewOutterEdgeLine(pFace,4);
					//1.10ѹ�����Y֫������ƽ��2
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,2,1);
					createFace.NewOutterEdgeLine(pFace,4);
					createFace.NewOutterEdgeLine(pFace,1);
					//1.11ѹ����ɱ���������ƽ��1
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,12+7,1);
					createFace.NewOutterEdgeLine(pFace,0);
					createFace.NewOutterEdgeLine(pFace,1);
					//1.12ѹ����ɱ���������ƽ��2
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,12+6,1);
					createFace.NewOutterEdgeLine(pFace,12+7);
					createFace.NewOutterEdgeLine(pFace,1);
				}
			}
			//ʼ�˱��Ϊ��ɫ, �뵥��ʼ�˱��һ�� wjh-2013.8.4
			pFace->material = RGB(255,255,0);

			if(end_push_flat_x1_y2<=0)	//�ն˲�ѹ��
			{
				//2.�Ǹֵڶ�������(��ֹ����)
				if(end_cut_wing_status==0)
				{	//�ն˲�����֫
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,11,6);
					createFace.NewOutterEdgeLine(pFace,10);
					createFace.NewOutterEdgeLine(pFace,9);
					createFace.NewOutterEdgeLine(pFace,8);
					createFace.NewOutterEdgeLine(pFace,7);
					createFace.NewOutterEdgeLine(pFace,6);
				}
				else if(end_cut_wing_status==1)
				{	//�ն�X֫����֫
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2],6);
					createFace.NewOutterEdgeLine(pFace,9);
					createFace.NewOutterEdgeLine(pFace,8);
					createFace.NewOutterEdgeLine(pFace,7);
					createFace.NewOutterEdgeLine(pFace,6);
				}
				else if(end_cut_wing_status==2)
				{	//�ն�Y֫����֫
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,11,6);
					createFace.NewOutterEdgeLine(pFace,10);
					createFace.NewOutterEdgeLine(pFace,9);
					createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
					createFace.NewOutterEdgeLine(pFace,0);
				}
			}
			else //�ն�ѹ�⴦��
			{
				if(end_push_flat_x1_y2==1)	//X֫ѹ��
				{
					//1.1ѹ���һ������(��ֹ����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+4,20+0);
					createFace.NewOutterEdgeLine(pFace,20+3);
					createFace.NewOutterEdgeLine(pFace,20+2);
					createFace.NewOutterEdgeLine(pFace,20+1);
					createFace.NewOutterEdgeLine(pFace,20+0);
					//1.2ѹ��ڶ�������(Y֫��ƽ��)
					pEndUpFlatFace = pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+0,6);
					createFace.NewOutterEdgeLine(pFace,20+1);
					createFace.NewOutterEdgeLine(pFace,7);
					createFace.NewOutterEdgeLine(pFace,6);
					//1.3ѹ�����������(Y֫��ƽ��)
					pEndDownFlatFace = pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+3,20+2);
					createFace.NewOutterEdgeLine(pFace,20+6);
					createFace.NewOutterEdgeLine(pFace,20+5);
					createFace.NewOutterEdgeLine(pFace,20+2);
					//1.4ѹ�����������(Y֫����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,20+1,7);
					createFace.NewOutterEdgeLine(pFace,20+2);
					createFace.NewOutterEdgeLine(pFace,20+5);
					createFace.NewOutterEdgeLine(pFace,7);
					//1.5ѹ�����������(X֫��ƽ��)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,6,20+0);
					createFace.NewOutterEdgeLine(pFace,20+7);
					createFace.NewOutterEdgeLine(pFace,20+4);
					createFace.NewOutterEdgeLine(pFace,20+0);
					//1.6ѹ�����������(X֫б����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+4,20+3);
					createFace.NewOutterEdgeLine(pFace,20+7);
					createFace.NewOutterEdgeLine(pFace,20+6);
					createFace.NewOutterEdgeLine(pFace,20+3);
					//1.7ѹ�����X֫��ƽ��
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+5,11);
					createFace.NewOutterEdgeLine(pFace,20+6);
					createFace.NewOutterEdgeLine(pFace,11);
					//1.8ѹ�����X֫��ƽ��
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,9,10);
					createFace.NewOutterEdgeLine(pFace,8);
					createFace.NewOutterEdgeLine(pFace,10);
					//1.9ѹ�����X֫������ƽ��1
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,20+5,8);
					createFace.NewOutterEdgeLine(pFace,11);
					createFace.NewOutterEdgeLine(pFace,8);
					//1.10ѹ�����X֫������ƽ��2
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,10,11);
					createFace.NewOutterEdgeLine(pFace,8);
					createFace.NewOutterEdgeLine(pFace,11);
					//1.11ѹ����ɱ���������ƽ��1
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+7,11);
					createFace.NewOutterEdgeLine(pFace,6);
					createFace.NewOutterEdgeLine(pFace,11);
					//1.12ѹ����ɱ���������ƽ��2
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+6,11);
					createFace.NewOutterEdgeLine(pFace,20+7);
					createFace.NewOutterEdgeLine(pFace,11);
				}
				else if(end_push_flat_x1_y2==2)	//Y֫ѹ��
				{
					//1.1ѹ���һ������(��ֹ����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+1,20+0);
					createFace.NewOutterEdgeLine(pFace,20+2);
					createFace.NewOutterEdgeLine(pFace,20+3);
					createFace.NewOutterEdgeLine(pFace,20+4);
					createFace.NewOutterEdgeLine(pFace,20+0);
					//1.2ѹ��ڶ�������(X֫��ƽ��)
					pEndUpFlatFace = pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,6,20+0);
					createFace.NewOutterEdgeLine(pFace,11);
					createFace.NewOutterEdgeLine(pFace,20+1);
					createFace.NewOutterEdgeLine(pFace,20+0);
					//1.3ѹ�����������(X֫��ƽ��)
					pEndDownFlatFace = pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+5,20+2);
					createFace.NewOutterEdgeLine(pFace,20+6);
					createFace.NewOutterEdgeLine(pFace,20+3);
					createFace.NewOutterEdgeLine(pFace,20+2);
					//1.4ѹ�����������(X֫����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,20+5,11);
					createFace.NewOutterEdgeLine(pFace,20+2);
					createFace.NewOutterEdgeLine(pFace,20+1);
					createFace.NewOutterEdgeLine(pFace,11);
					//1.5ѹ�����������(Y֫��ƽ��)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+0,6);
					createFace.NewOutterEdgeLine(pFace,20+4);
					createFace.NewOutterEdgeLine(pFace,20+7);
					createFace.NewOutterEdgeLine(pFace,6);
					//1.6ѹ�����������(Y֫б����)
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+6,20+3);
					createFace.NewOutterEdgeLine(pFace,20+7);
					createFace.NewOutterEdgeLine(pFace,20+4);
					createFace.NewOutterEdgeLine(pFace,20+3);
					//1.7ѹ�����Y֫��ƽ��
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,7,20+5);
					createFace.NewOutterEdgeLine(pFace,20+6);
					createFace.NewOutterEdgeLine(pFace,20+5);
					//1.8ѹ�����Y֫��ƽ��
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,10,8);
					createFace.NewOutterEdgeLine(pFace,9);
					createFace.NewOutterEdgeLine(pFace,8);
					//1.9ѹ�����Y֫������ƽ��1
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,10,20+5);
					createFace.NewOutterEdgeLine(pFace,7);
					createFace.NewOutterEdgeLine(pFace,20+5);
					//1.10ѹ�����Y֫������ƽ��2
					pFace = pBody->faceList.append();
					pFace->material = crAngleEdge;
					createFace.NewOutterEdgeLine(pFace,7,8);
					createFace.NewOutterEdgeLine(pFace,10);
					createFace.NewOutterEdgeLine(pFace,8);
					//1.11ѹ����ɱ���������ƽ��1
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,6,7);
					createFace.NewOutterEdgeLine(pFace,20+7);
					createFace.NewOutterEdgeLine(pFace,7);
					//1.12ѹ����ɱ���������ƽ��2
					pFace = pBody->faceList.append();
					pFace->material = crAngleWingX;
					createFace.NewOutterEdgeLine(pFace,20+7,7);
					createFace.NewOutterEdgeLine(pFace,20+6);
					createFace.NewOutterEdgeLine(pFace,7);
				}
			}

			//hxr 12-10-09
			f3dPolyFace *pUpFace2=NULL,*pMiddleUpFlatFace=NULL,*pMiddleDownFlatFace=NULL,*pDownFace2=NULL;
			if (midPushFlatX1Y2==1)	//�м�X֫ѹ�⴦��
			{
				//1.1�м�������(Y֫��ƽ��)
				pMiddleUpFlatFace = pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[1],mid_push_flat_vertex_arr_index[8]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[9]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[8]);
				//1.2�м�С������(X֫б����)
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[2],mid_push_flat_vertex_arr_index[9]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[3]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[10]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[9]);
				//1.3��ѹ�����X֫������ƽ��1
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11],mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[12]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				//1.4��ѹ�����X֫������ƽ��2
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[8],mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				//1.5��ѹ�����X֫��ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[9],mid_push_flat_vertex_arr_index[8]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[8]);
				//1.6��ѹ����ȱ���������ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[10],mid_push_flat_vertex_arr_index[9]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[9]);
				//1.7��ѹ�����X֫��ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[12],mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[13]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				//1.8��ѹ�����X֫������ƽ��1
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[5],mid_push_flat_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0]);
				//1.9��ѹ�����X֫������ƽ��2
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4],mid_push_flat_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0]);
				//1.10��ѹ�����X֫��ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4],mid_push_flat_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[1]);
				//1.11��ѹ����ȱ���������ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4],mid_push_flat_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[3]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[2]);
				//1.12��ѹ�����X֫��ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[6],mid_push_flat_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[5]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0]);
			}
			else if (midPushFlatX1Y2==2)	//�м�Y֫ѹ�⴦��
			{
				//1.1�м�������(X֫��ƽ��)
				pMiddleDownFlatFace = pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[9],mid_push_flat_vertex_arr_index[8]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[8]);
				//1.2�м�С������(Y֫б����)
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[10],mid_push_flat_vertex_arr_index[9]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[3]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[9]);
				//1.3��ѹ�����Y֫������ƽ��1
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[12],mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				//1.4��ѹ�����Y֫������ƽ��2
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11],mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[8]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				//1.5��ѹ�����Y֫��ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11],mid_push_flat_vertex_arr_index[8]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[9]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[8]);
				//1.6��ѹ����ȱ���������ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11],mid_push_flat_vertex_arr_index[9]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[10]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[9]);
				//1.7��ѹ�����Y֫��ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[13],mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[12]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				//1.8��ѹ�����Y֫������ƽ��1
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4],mid_push_flat_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[5]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0]);
				//1.9��ѹ�����Y֫������ƽ��2
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[1],mid_push_flat_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0]);
				//1.10��ѹ�����Y֫��ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[2],mid_push_flat_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[1]);
				//1.11��ѹ����ȱ���������ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[3],mid_push_flat_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[2]);
				//1.12��ѹ�����Y֫��ƽ��
				pFace = pBody->faceList.append();
				pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[5],mid_push_flat_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[6]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0]);
			}

			//3.�Ǹֵ���������(Y֫�ϵ���)
			pBtmFaceOfLegY=pUpFace = pFace = pBody->faceList.append();
			if(pStart&&pEnd)
				pFace->material = crAngleWingY;
			else
				pFace->material = crAngleWingY;
			int face_start_vertex_i=0;
			if(start_cut_wing_status==0)		//ʼ������֫��֫
			{
				createFace.NewOutterEdgeLine(pFace,2,3);
				face_start_vertex_i=3;
			}
			else if(start_cut_wing_status==1)	//ʼ��X֫��֫��֫
			{
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1],start_cut_wing_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,3);
				createFace.NewOutterEdgeLine(pFace,2);
				face_start_vertex_i=start_cut_wing_vertex_arr_index[0];
			}
			else if(start_cut_wing_status==2)	//ʼ��Y֫��֫��֫
			{
				createFace.NewOutterEdgeLine(pFace,2,start_cut_wing_vertex_arr_index[0]);
				face_start_vertex_i=start_cut_wing_vertex_arr_index[0];
			}
			//hxr 2012-9-29
			if (midPushFlatX1Y2==1)		//�м�X֫ѹ��
			{
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[13]);
				createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
				pUpFace2 = pFace = pBody->faceList.append();
				if(pStart&&pEnd)
					pFace->material = crAngleWingY;
				else
					pFace->material = crAngleWingY;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0],mid_push_flat_vertex_arr_index[6]);
				face_start_vertex_i=mid_push_flat_vertex_arr_index[6];
			}
			else if (midPushFlatX1Y2==2)		//�м�Y֫ѹ��
			{
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[12]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[13]);
				createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
				pUpFace2 = pFace = pBody->faceList.append();
				if(pStart&&pEnd)
					pFace->material = crAngleWingY;
				else
					pFace->material = crAngleWingY;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[5],mid_push_flat_vertex_arr_index[6]);
				face_start_vertex_i=mid_push_flat_vertex_arr_index[6];
			}
			createFace.NewOutterEdgeLine(pFace,8);
			if(end_cut_wing_status==0)		//�ն�����֫��֫
				createFace.NewOutterEdgeLine(pFace,9);
			else if(end_cut_wing_status==1)	//�ն�X֫��֫��֫
			{
				createFace.NewOutterEdgeLine(pFace,9);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
			}
			else if(end_cut_wing_status==2)	//�ն�Y֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
			createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

			//4.�Ǹֵ���������(Y֫�µ���)
			pTopFaceOfLegY=pDownFace = pFace = pBody->faceList.append();
			if(pStart&&pEnd)
				pFace->material = crAngleWingY;
			else
				pFace->material = crAngleWingY;
			if(start_cut_wing_status!=2)
			{	//��Y֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,0,1);
				face_start_vertex_i=1;
			}
			else
			{	//Y֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1],1);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,0);
				face_start_vertex_i=1;
			}
			createFace.NewOutterEdgeLine(pFace,6);
			if(end_cut_wing_status!=2)	//��Y֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,7);
			else
			{	//Y֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,7);
			}
			//hxr 2012-9-29
			if (midPushFlatX1Y2==2)
			{	//�м�Y֫ѹ��
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[3]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[10]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11]);
			}
			createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
			//-----------VVVVV----�����˨��----VVVVV-------------------------
			CHoleMaker hole_maker;
			UCS_STRU hole_ucs;
			BOLTHOLE_INFO *pLsCir;
			for(pLsCir=y_wing_ls.GetFirst();pLsCir;pLsCir=y_wing_ls.GetNext())
			{
				COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
				hole_ucs.origin = pLsCir->centre;
				hole_ucs.axis_x = pLsCir->axis_x;
				hole_ucs.axis_z = pLsCir->norm;
				if(start_push_flat_x1_y2==1)	//ʼ��X֫ѹ��,Y֫�Ͽɴ��
				{
					if(hole_ucs.origin.z<start_push_flat_length-startOdd())
					{
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pStartUpFlatFace,pStartDownFlatFace,pBody,crLsHole);
						continue;
					}
				}
				if(end_push_flat_x1_y2==1)	//�ն�ѹ��
				{
					if(hole_ucs.origin.z>length+endOdd()-end_push_flat_length)
					{
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pEndUpFlatFace,pEndDownFlatFace,pBody,crLsHole);
						continue;
					}
				}
				//hxr 2012-10-09
				if (midPushFlatX1Y2==1)		//�м�X֫ѹ��,Y֫�Ͽɴ��
				{
					if (hole_ucs.origin.z>len_starttomid-mid_push_flat_start_offset
						&&hole_ucs.origin.z<len_starttomid+mid_push_flat_end_offset)
					{
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pMiddleUpFlatFace,pDownFace,pBody,crLsHole);
						continue;
					}
				}
				//hxr 2012-10-09
				if (midPushFlatX1Y2>0)
				{
					if (hole_ucs.origin.z<len_starttomid-mid_push_flat_start_offset)
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace,pDownFace,pBody,crLsHole);
					else 
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace2,pDownFace,pBody,crLsHole);
				}
				else
					hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace,pDownFace,pBody,crLsHole);
			}
			//--------------------�����˨��-----------------------------

			//5.�Ǹֵ���������(X֫�����)
			pTopFaceOfLegX=pUpFace = pFace = pBody->faceList.append();
			if(pStart&&pEnd)
				pFace->material = crAngleWingX;
			else
				pFace->material = crAngleWingX;
			if(start_cut_wing_status==1)
			{	//ʼ��X֫����֫
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2],0);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,5);
				face_start_vertex_i=0;
			}
			else
			{
				createFace.NewOutterEdgeLine(pFace,5,0);
				face_start_vertex_i=0;
			}
			//hxr 2012-9-29
			if (midPushFlatX1Y2==1)
			{	//�м�X֫ѹ��
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[10]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[3]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4]);
			}
			createFace.NewOutterEdgeLine(pFace,11);
			if(end_cut_wing_status==1)
			{	//�ն�X֫����֫
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
			}
			createFace.NewOutterEdgeLine(pFace,6);
			createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

			//6.�Ǹֵ���������(X֫�ҵ���)
			pBtmFaceOfLegX = pDownFace = pFace = pBody->faceList.append();
			if(pStart&&pEnd)
				pFace->material = crAngleWingX;
			else
				pFace->material = crAngleWingX;
			if(start_cut_wing_status==0)
			{	//ʼ������֫��֫
				createFace.NewOutterEdgeLine(pFace,3,4);
				face_start_vertex_i=4;
			}
			else if(start_cut_wing_status==1)
			{	//ʼ��X֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0],4);
				face_start_vertex_i=4;
			}
			else if(start_cut_wing_status==2)
			{	//ʼ��Y֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,3,4);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0]);
				face_start_vertex_i=4;
			}
			//hxr 2012-9-29
			if (midPushFlatX1Y2==1)		//�м�X֫ѹ��
			{
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[13]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[12]);
				createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
				pDownFace2 = pFace = pBody->faceList.append();
				if(pStart&&pEnd)
					pFace->material = crAngleWingX;
				else
					pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[6],mid_push_flat_vertex_arr_index[5]);
				face_start_vertex_i=mid_push_flat_vertex_arr_index[5];
			}
			else if (midPushFlatX1Y2==2)		//�м�Y֫ѹ��
			{
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[13]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
				pDownFace2 = pFace = pBody->faceList.append();
				if(pStart&&pEnd)
					pFace->material = crAngleWingX;
				else
					pFace->material = crAngleWingX;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[6],mid_push_flat_vertex_arr_index[0]);
				face_start_vertex_i=mid_push_flat_vertex_arr_index[0];
			}
			if(end_cut_wing_status==0)		//�ն�����֫��֫
				createFace.NewOutterEdgeLine(pFace,9);
			else if(end_cut_wing_status==1)	//�ն�X֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
			else if(end_cut_wing_status==2)
			{	//�ն�Y֫��֫��֫
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[2]);
				createFace.NewOutterEdgeLine(pFace,9);
			}
			createFace.NewOutterEdgeLine(pFace,10);
			createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
			//-----------VVVVV----�����˨��----VVVVV-------------------------
			for(pLsCir=x_wing_ls.GetFirst();pLsCir;pLsCir=x_wing_ls.GetNext())
			{
				COLORREF crLsHole=GetColor(CLS_BOLT,ftoi(pLsCir->bolt_d),' ');
				hole_ucs.origin = pLsCir->centre;
				hole_ucs.axis_x = pLsCir->axis_x;
				hole_ucs.axis_z = pLsCir->norm;
				if(start_push_flat_x1_y2==2)	//ʼ��Y֫ѹ��,X֫�Ͽɴ��
				{
					if(hole_ucs.origin.z<start_push_flat_length-startOdd())
					{
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pStartUpFlatFace,pStartDownFlatFace,pBody,crLsHole);
						continue;
					}
				}
				else if(end_push_flat_x1_y2==2)	//�ն�Y֫ѹ��,X֫�Ͽɴ��
				{
					if(hole_ucs.origin.z>length+endOdd()-end_push_flat_length)
					{
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pEndUpFlatFace,pEndDownFlatFace,pBody,crLsHole);
						continue;
					}
				}
				//hxr 2012-10-09
				if (midPushFlatX1Y2==2)		//�м�Y֫ѹ��,X֫�Ͽɴ��
				{
					if (hole_ucs.origin.z>len_starttomid-mid_push_flat_start_offset
						&&hole_ucs.origin.z<len_starttomid+mid_push_flat_end_offset)
					{
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace,pMiddleDownFlatFace,pBody,crLsHole);
						continue;
					}
				}
				//hxr 2012-10-09
				if (midPushFlatX1Y2>0)
				{
					//if(hole_ucs.origin.z<len_starttomid-mid_push_flat_length/2)
					if(hole_ucs.origin.z<len_starttomid-mid_push_flat_start_offset)
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace,pDownFace,pBody,crLsHole);
					else 
						hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace,pDownFace2,pBody,crLsHole);
				}
				else 
					hole_maker.Create(hole_ucs,pLsCir->hole_d,pLsCir->waistLen,pUpFace,pDownFace,pBody,crLsHole);
			}
			//--------------------�����˨��-----------------------------

			//7.�Ǹֵ���������(X֫�϶���)
			pFace = pBody->faceList.append();
			pFace->material = crAngleEdge;
			createFace.NewOutterEdgeLine(pFace,4,5);
			//hxr 2012-9-29
			face_start_vertex_i=5;
			if (midPushFlatX1Y2==1)
			{	//�м�X֫ѹ��
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[12]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11]);
				createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[5],mid_push_flat_vertex_arr_index[4]);
				face_start_vertex_i=mid_push_flat_vertex_arr_index[4];
			}
			else if (midPushFlatX1Y2==2)
			{	//�м�Y֫ѹ��
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[8]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0]);
			}
			createFace.NewOutterEdgeLine(pFace,10);
			createFace.NewOutterEdgeLine(pFace,11);
			createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

			//8.�Ǹֵڰ�������(Y֫�Ҷ���)
			pFace = pBody->faceList.append();
			pFace->material = crAngleEdge;
			createFace.NewOutterEdgeLine(pFace,1,2);
			//hxr 2012-9-29
			face_start_vertex_i=2;
			if (midPushFlatX1Y2==2)
			{	//�м�Y֫ѹ��
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[11]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[12]);
				createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);
				pFace = pBody->faceList.append();
				pFace->material = crAngleEdge;
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[4],mid_push_flat_vertex_arr_index[5]);
				face_start_vertex_i=mid_push_flat_vertex_arr_index[5];
			}
			createFace.NewOutterEdgeLine(pFace,7);
			createFace.NewOutterEdgeLine(pFace,8);
			//hxr 2012-9-29
			if (midPushFlatX1Y2==1)
			{	//�м�X֫ѹ��
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[8]);
				createFace.NewOutterEdgeLine(pFace,mid_push_flat_vertex_arr_index[7]);
			}
			createFace.NewOutterEdgeLine(pFace,face_start_vertex_i);

			//---------------�����н���֫---------------------------------
			//---------------����֫���и���---------------------------------
			COLORREF crCutFaceOrPolyJoint=GetColor(CLS_POLYJOINT,-1,' '); //�и���
			if(start_cut_wing_status==1)
			{	//ʼ��X֫����֫
				pFace = pBody->faceList.append();
				pFace->material = crCutFaceOrPolyJoint;
				createFace.NewOutterEdgeLine(pFace,5,4);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,4);
			}
			else if(start_cut_wing_status==2)
			{	//ʼ��Y֫����֫
				pFace = pBody->faceList.append();
				pFace->material = crCutFaceOrPolyJoint;
				createFace.NewOutterEdgeLine(pFace,2,1);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,start_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,1);
			}
			if(end_cut_wing_status==1)
			{	//�ն�X֫����֫
				pFace = pBody->faceList.append();
				pFace->material = crCutFaceOrPolyJoint;
				createFace.NewOutterEdgeLine(pFace,10,11);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,11);
			}
			else if(end_cut_wing_status==2)
			{	//�ն�Y֫����֫
				pFace = pBody->faceList.append();
				pFace->material = crCutFaceOrPolyJoint;
				createFace.NewOutterEdgeLine(pFace,7,8);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[1]);
				createFace.NewOutterEdgeLine(pFace,end_cut_wing_vertex_arr_index[0]);
				createFace.NewOutterEdgeLine(pFace,8);
			}
			f3dPoint plane_norm,plane_pick,vec1,vec2;

			if(cut_wing[0]>0&&start_cut_wing_status==0&&
				(cut_wing_para[0][0]+cut_wing_para[0][1]+cut_wing_para[0][2])>0)	//������ʼ������֫
			{
				plane_pick.z = -startOdd()+cut_wing_para[0][0];
				double offset_dist=0;
				//����֫����cut_wing_para[...][2]Ϊ0ʱ��������ʵ����ʾ wht 10-07-30
				if(fabs(cut_wing_para[0][2])<EPS2)
					offset_dist=1;	
				if(cut_wing[0]==1)	//X֫Ϊ�н���֫
				{
					vec1.Set(size_wide,0,offset_dist+cut_wing_para[0][2]-cut_wing_para[0][0]);
					vec2.Set(wing_y_vec.x*cut_wing_para[0][1],
						wing_y_vec.y*cut_wing_para[0][1],-cut_wing_para[0][0]);
					plane_norm = vec2^vec1;
					if(plane_norm.z>0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
						plane_norm = -plane_norm;
					pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
				}
				else	//Y֫Ϊ�н���֫
				{
					vec1.Set(wing_y_vec.x*size_wide,size_wide*wing_y_vec.y,
						offset_dist+cut_wing_para[0][2]-cut_wing_para[0][0]);
					vec2.Set(cut_wing_para[0][1],0,-cut_wing_para[0][0]);
					plane_norm = vec1^vec2;
					if(plane_norm.z>0)
						plane_norm = -plane_norm;
					pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
				}
			}
			if(cut_wing[1]>0&&end_cut_wing_status==0&&
				(cut_wing_para[1][0]+cut_wing_para[1][1]+cut_wing_para[1][2])>0)		//������ֹ������֫
			{
				plane_pick.z = length+endOdd()-cut_wing_para[1][0];
				double offset_dist=0;
				//����֫����cut_wing_para[...][2]Ϊ0ʱ��������ʵ����ʾ wht 10-07-30
				if(fabs(cut_wing_para[1][2])<EPS2)
					offset_dist=1;	
				if(cut_wing[1]==1)	//X֫Ϊ�н���֫
				{
					vec1.Set(size_wide,0,-offset_dist-cut_wing_para[1][2]+cut_wing_para[1][0]);
					vec2.Set(wing_y_vec.x*cut_wing_para[1][1],
						wing_y_vec.y*cut_wing_para[1][1],cut_wing_para[1][0]);
					plane_norm = vec1^vec2;
					if(plane_norm.z<0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
						plane_norm = -plane_norm;
					pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
				}
				else	//Y֫Ϊ�н���֫
				{
					vec1.Set(wing_y_vec.x*size_wide,size_wide*wing_y_vec.y,-offset_dist-cut_wing_para[1][2]+cut_wing_para[1][0]);
					vec2.Set(cut_wing_para[1][1],0,cut_wing_para[1][0]);

					plane_norm = vec2^vec1;
					if(plane_norm.z<0)
						plane_norm = -plane_norm;
					pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
				}
			}
			//������ʼ����X֫�н�
			if(cut_angle[0][0]>0)	//X֫��֫
			{
				plane_pick.Set(size_wide-cut_angle[0][1],0,-startOdd());
				vec1.Set(cut_angle[0][1],0,cut_angle[0][0]);
				vec2.Set(0,1,0);
				plane_norm = vec2^vec1;
				if(plane_norm.z>0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
					plane_norm = -plane_norm;
				pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
			}
			//������ʼ����Y֫�н�
			if(cut_angle[1][0]>0)	//Y֫��֫
			{
				plane_pick.Set((size_wide-cut_angle[1][1])*wing_y_vec.x,
					(size_wide-cut_angle[1][1])*wing_y_vec.y,-startOdd());
				vec1.Set(cut_angle[1][1]*wing_y_vec.x,cut_angle[1][1]*wing_y_vec.y,
					cut_angle[1][0]);
				plane_norm = norm_Y^vec1;
				if(plane_norm.z>0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
					plane_norm = -plane_norm;
				pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
			}
			//������ֹ����X֫�н�
			if(cut_angle[2][0]>0)	//X֫��֫
			{
				plane_pick.Set(size_wide-cut_angle[2][1],0,length+endOdd());
				vec1.Set(cut_angle[2][1],0,-cut_angle[2][0]);
				vec2.Set(0,1,0);
				plane_norm = vec1^vec2;
				if(plane_norm.z<0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
					plane_norm = -plane_norm;
				pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
			}
			//������ֹ����Y֫�н�
			if(cut_angle[3][0]>0)	//Y֫��֫
			{
				plane_pick.Set((size_wide-cut_angle[3][1])*wing_y_vec.x,
					(size_wide-cut_angle[3][1])*wing_y_vec.y,
					length+endOdd());
				vec1.Set(cut_angle[3][1]*wing_y_vec.x,cut_angle[3][1]*wing_y_vec.y,
					-cut_angle[3][0]);
				plane_norm = vec1^norm_Y;
				if(plane_norm.z<0)	//��ʼ���и���ӦΪ�������Z�Ḻ����
					plane_norm = -plane_norm;
				pBody->CutByPlane(plane_pick,plane_norm,crCutFaceOrPolyJoint);
			}
			//ȥ����ʵ�巽ʽ��ʾ�Ǹ���֫����,��Ϊ��ͼ����ֱ�ӻ����� wjh-2015.2.3
			//if(CLDSLineAngle::m_bDisplaySolidAxisLine)
			//�Ǹֽ��Ӵ���ʾ����
			UpdateInnerCircleSlopeFace(pBody,wing_x_vec,wing_y_vec,norm_Y);
			//�Ǹ�֫��ʾԲ����
			UpdateWingFilletArcFace(pBody,wing_x_vec,wing_y_vec,norm_Y);
			//�Ǹ�֫���ʵ�崦����Ҫ���ڹ��߽Ǹ��Ͻ����ת��ڴ��� wjh-2019.3.26
			for(i=0;i<(int)this->xarrOpenCuts.Count;i++)
			{
				OPENING_CUT cut=xarrOpenCuts[i];
				GEPOINT xarrCutPolyVertexes[4];
				if(cut.dfLateralBtmXorY<0&&pTopFaceOfLegX!=NULL&&pBtmFaceOfLegX!=NULL)
				{	//X֫���
					xarrCutPolyVertexes[1].Set(-cut.dfLateralBtmXorY,0,cut.dfLenPosTo);
					GEPOINT vOffset(SIZE_WIDTH_OF_LEGX+cut.dfLateralBtmXorY,0,cut.dfTipDeltaE);
					xarrCutPolyVertexes[0]=xarrCutPolyVertexes[1]+vOffset*2;
					xarrCutPolyVertexes[2].Set(-cut.dfLateralBtmXorY,0,cut.dfLenPosFrom);
					vOffset.Set(SIZE_WIDTH_OF_LEGX+cut.dfLateralBtmXorY,0,cut.dfTipDeltaS);
					xarrCutPolyVertexes[3]=xarrCutPolyVertexes[2]+vOffset*2;
					CFlatBiteCut::Cut(xarrCutPolyVertexes,4,pTopFaceOfLegX,pBtmFaceOfLegX,pBody);
				}
				else if(cut.dfLateralBtmXorY>0&&pTopFaceOfLegY!=NULL&&pBtmFaceOfLegY!=NULL)
				{	//Y֫���
					xarrCutPolyVertexes[1]=wing_y_vec*cut.dfLateralBtmXorY;
					xarrCutPolyVertexes[1].z=cut.dfLenPosFrom;
					GEPOINT vOffset=wing_y_vec*(SIZE_WIDTH_OF_LEGY-cut.dfLateralBtmXorY);
					vOffset.z=cut.dfTipDeltaS;
					xarrCutPolyVertexes[0]=xarrCutPolyVertexes[1]+vOffset*2;
					xarrCutPolyVertexes[2]=wing_y_vec*cut.dfLateralBtmXorY;
					xarrCutPolyVertexes[2].z=cut.dfLenPosTo;
					vOffset=wing_y_vec*(SIZE_WIDTH_OF_LEGY-cut.dfLateralBtmXorY);
					vOffset.z=cut.dfTipDeltaE;
					xarrCutPolyVertexes[3]=xarrCutPolyVertexes[2]+vOffset*2;
					CFlatBiteCut::Cut(xarrCutPolyVertexes,4,pTopFaceOfLegY,pBtmFaceOfLegY,pBody);
				}
			}
			//
			for(f3dPoint *pPt=pBody->vertex.GetFirst();pPt;pPt=pBody->vertex.GetNext())
			{
				if(!coord_trans(*pPt,ucs,TRUE,TRUE))
				{
					char sError[100];
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"0X%X Angle's coordinate error converts",handle);
#else
					sprintf(sError,"���Ϊ0X%X�ĽǸ�����ת������",handle);
#endif
					throw sError;
				}
			}
		}
		pSolidBody->ConvertFrom(pBody);
	}
	return TRUE;
}

void CLDSLineAngle::CopyProperty(CLDSDbObject* pObj)
{
    SetLayer(pObj->layer());						// ���Ǹ�����ͼ��.
	iSeg = pObj->iSeg;
	cfgword = pObj->cfgword;
	if(pObj->IsPart())
	{
		CSuperSmartPtr<CLDSPart> pPart=(CLDSPart*)pObj;
		SetPartNo(pPart->GetPartNo());
		m_bVirtualPart = pPart->m_bVirtualPart;
		cMaterial = pPart->cMaterial;
		cbVirginQualityLevel=pPart->cbVirginQualityLevel;
#ifdef __PART_DESIGN_INC_
		if(pPart->IsLinePart())
		{
			m_uStatMatNo=pPart.LinePartPointer()->m_uStatMatNo;
			m_bNeedSelSize=pPart.LinePartPointer()->m_bNeedSelSize;
			start_force_type=pPart.LinePartPointer()->start_force_type;
			end_force_type=pPart.LinePartPointer()->end_force_type;
			start_joint_type=pPart.LinePartPointer()->start_joint_type;
			end_joint_type=pPart.LinePartPointer()->end_joint_type;
			CalLenCoef.iTypeNo=pPart.LinePartPointer()->CalLenCoef.iTypeNo;
			if(pPart->IsAngle())
			{
				hole_out=pPart.LineAnglePointer()->hole_out;
				m_bAutoCalHoleOutCoef=pPart.LineAnglePointer()->m_bAutoCalHoleOutCoef;
			}
			m_cbRodFlag=pPart.pRod->m_cbRodFlag;
		}
#endif
	}
	is_data_modified = TRUE;
	if(!pObj->IsPart()||!((CLDSLinePart*)pObj)->IsLinePart())
		return;
	CLDSLinePart* pRod=(CLDSLinePart*)pObj;
	size_wide=pRod->GetWidth();
	size_thick=pRod->GetThick();
	//start_oddment = pJg->startOdd();	//ʵ��֤������ͷ�ԳƲ��ã�������
	//end_oddment = pJg->endOdd();
	if(pObj->GetClassTypeId()!=CLS_LINEANGLE&&pObj->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		return;
	CLDSLineAngle *pJg=(CLDSLineAngle*)pObj;
	nominal_length = pJg->nominal_length;
	//m_bEnableTeG=pJg->m_bEnableTeG;
	//xWingXZhunJu=pJg->xWingXZhunJu;
	//xWingYZhunJu=pJg->xWingYZhunJu;
	if(GetClassTypeId()==CLS_LINEANGLE)
	{	//�Ǹ�
		group_father_jg_h = pJg->group_father_jg_h;
		connectStart.CopyBasicBoltInfo(pJg->connectStart);
		connectEnd.CopyBasicBoltInfo(pJg->connectEnd);
		MirLayStyle=pJg->MirLayStyle;
		if(!IsStartPosLocked())
		{
			desStartPos.jgber_cal_style=pJg->desStartPos.jgber_cal_style;
			desStartPos.origin_pos_style=pJg->desStartPos.origin_pos_style;
			desStartPos.ChangeToDatumType(pJg->desStartPos.GetDatumType());	//ͶӰƽ�Ʒ�ʽ WJH-2004.09.20
			//des_start_pos.datum_to_ber_style=pJg->des_start_pos.datum_to_ber_style;
			//des_start_pos.bByOtherEndOffset=pJg->des_start_pos.bByOtherEndOffset;
			desStartPos.face_offset_norm=pJg->desStartPos.face_offset_norm;
			//Y��Գ���������ʱ���´�������� wjh-2015.8.26
			desStartPos.len_offset_type=pJg->desStartPos.len_offset_type;
			desStartPos.len_offset_dist=pJg->desStartPos.len_offset_dist;
			desStartPos.datum_offset_dist=pJg->desStartPos.datum_offset_dist;
		}
		if(!IsEndPosLocked())
		{
			desEndPos.jgber_cal_style=pJg->desEndPos.jgber_cal_style;
			desEndPos.origin_pos_style=pJg->desEndPos.origin_pos_style;
			desEndPos.ChangeToDatumType(pJg->desEndPos.GetDatumType());	//ͶӰƽ�Ʒ�ʽ WJH-2004.09.20
			//des_end_pos.datum_to_ber_style=pJg->des_end_pos.datum_to_ber_style;
			//des_end_pos.bByOtherEndOffset=pJg->des_end_pos.bByOtherEndOffset;
			desEndPos.face_offset_norm=pJg->desEndPos.face_offset_norm;
			desEndPos.len_offset_type=pJg->desEndPos.len_offset_type;
			desEndPos.len_offset_dist=pJg->desEndPos.len_offset_dist;
			desEndPos.datum_offset_dist=pJg->desEndPos.datum_offset_dist;
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetClassTypeId()==CLS_GROUPLINEANGLE&&pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{	//��ϽǸ�
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)pJg;
		CLDSGroupLineAngle *pCurGroupJg=(CLDSGroupLineAngle*)this;
		pCurGroupJg->jg_space=pGroupJg->jg_space;
		pCurGroupJg->group_style=pGroupJg->group_style;
		pCurGroupJg->ciMasterIndex=pGroupJg->ciMasterIndex;
		pCurGroupJg->ciMasterDirectIndex=pGroupJg->ciMasterDirectIndex;
		pCurGroupJg->m_bUpdateSonAngleOddment=pGroupJg->m_bUpdateSonAngleOddment;
	}
	else if(GetClassTypeId()==CLS_GROUPLINEANGLE&&pJg->GetClassTypeId()==CLS_LINEANGLE)
	{	//�������Ǹ���˨��Ϣ����ϽǸ�
		connectStart.CopyBasicBoltInfo(pJg->connectStart);
		connectEnd.CopyBasicBoltInfo(pJg->connectEnd);
	}
#endif
}

//��ȡ��˨���ڵ�ǰ�Ǹֵ��ľ��������ͬ��GetLsG���ӵ���ʵ�ʿռ����ֵ wjh-2016.1.8
double CLDSLineAngle::GetLsParamG(CLDSBolt *pBolt)
{
	CLDSPoint* pPoint=&pBolt->des_base_pos.datumPoint;
	double wing_offset_dist=0;
	if(pPoint->datum_pos_style==1)
	{	//�Ǹ����
		if( pPoint->des_para.RODEND.offset_wing==pBolt->des_base_pos.offset_wing&&
			pPoint->des_para.RODEND.hRod==handle)
		{
			if(pPoint->des_para.RODEND.wing_offset_style>=4)
				wing_offset_dist=pPoint->des_para.RODEND.wing_offset_dist;
			else if(pPoint->des_para.RODEND.offset_wing==0)
				wing_offset_dist=GetWingXG(pPoint->des_para.RODEND.wing_offset_style);
			else if(pPoint->des_para.RODEND.offset_wing==1)
				wing_offset_dist=GetWingYG(pPoint->des_para.RODEND.wing_offset_style);
		}
	}
	else if(pPoint->datum_pos_style==2)
	{	//�Ǹֽڵ�
		if( pPoint->des_para.RODNODE.offset_wing==pBolt->des_base_pos.offset_wing&&
			pPoint->des_para.RODNODE.hRod==handle)
		{
			if(pPoint->des_para.RODNODE.wing_offset_style>=4)
				wing_offset_dist=pPoint->des_para.RODNODE.wing_offset_dist;
			else if(pPoint->des_para.RODNODE.offset_wing==0)
				wing_offset_dist=GetWingXG(pPoint->des_para.RODNODE.wing_offset_style);
			else if(pPoint->des_para.RODNODE.offset_wing==1)
				wing_offset_dist=GetWingYG(pPoint->des_para.RODNODE.wing_offset_style);
		}
	}
	else if(pPoint->datum_pos_style==3)
	{	//�Ǹ����߽���
		if(pPoint->des_para.AXIS_INTERS.hDatum1==handle)
		{
			if(pPoint->des_para.AXIS_INTERS.wing_offset_style1>=4)
				wing_offset_dist=pPoint->des_para.AXIS_INTERS.wing_offset_dist1;
			else if(pBolt->des_base_pos.offset_wing==0)
				wing_offset_dist=GetWingXG(pPoint->des_para.AXIS_INTERS.wing_offset_style1);
			else if(pBolt->des_base_pos.offset_wing==1)
				wing_offset_dist=GetWingYG(pPoint->des_para.AXIS_INTERS.wing_offset_style1);
		}
		else if(pPoint->des_para.AXIS_INTERS.hDatum2==handle)
		{
			if(pPoint->des_para.AXIS_INTERS.wing_offset_style2>=4)
				wing_offset_dist=pPoint->des_para.AXIS_INTERS.wing_offset_dist2;
			else if(pBolt->des_base_pos.offset_wing==0)
				wing_offset_dist=GetWingXG(pPoint->des_para.AXIS_INTERS.wing_offset_style2);
			else if(pBolt->des_base_pos.offset_wing==1)
				wing_offset_dist=GetWingYG(pPoint->des_para.AXIS_INTERS.wing_offset_style2);
		}
	}
	return  wing_offset_dist+pBolt->des_base_pos.wing_offset_dist;
}
double CLDSLineAngle::GetLsG(CLsRef *pLsRef,double *depth/*=NULL*/,bool bInitUCS/*=true*/)
{
	return GetLsG(pLsRef->GetLsPtr(),depth,bInitUCS);
}
double CLDSLineAngle::GetLsG(CLDSBolt *pLs,double *depth/*=NULL*/,bool bInitUCS/*=true*/)
{
	f3dPoint perp,ls_vec,jg_vec,norm,base_pos;
	if(bInitUCS)
		getUCS(ucs);
	if( fabs(norm_x_wing*pLs->get_norm())
		>fabs(norm_y_wing*pLs->get_norm()))
	{	//X֫��˨
		GetBoltIntersPos(pLs,base_pos);
		//SnapPerp(&perp,Start(),End(),pLs->ucs.origin);
		//base_pos = pLs->ucs.origin;
		//project_point(base_pos,perp,norm_x_wing);
		double dd = DistOf3dPtLine(base_pos,Start(),End());
		//�ж���˨�����ߵ���һ��
		ls_vec=base_pos-Start();
		jg_vec=End()-Start();
		normalize(ls_vec);
		normalize(jg_vec);
		norm=jg_vec^ls_vec;
		if(norm*norm_x_wing>0)
			dd=-dd;

		if(depth)
		{
			base_pos = pLs->ucs.origin;
			coord_trans(base_pos,ucs,FALSE);
			*depth=-base_pos.y;
		}
		return dd;
	}
	else
	{	//Y֫��˨
		GetBoltIntersPos(pLs,base_pos);
		//SnapPerp(&perp,Start(),End(),pLs->ucs.origin);
		//base_pos = pLs->ucs.origin;
		//project_point(base_pos,perp,norm_y_wing);
		double dd = DistOf3dPtLine(base_pos,Start(),End());
		//�ж���˨�����ߵ���һ��
		ls_vec=base_pos-Start();
		jg_vec=End()-Start();
		normalize(ls_vec);
		normalize(jg_vec);
		norm=jg_vec^ls_vec;
		if(norm*norm_y_wing<0)
			dd=-dd;

		if(depth)
		{
			f3dPoint perp,bak_ls_pos;
			base_pos = pLs->ucs.origin;
			bak_ls_pos = base_pos;
			SnapPerp(&perp,Start(),End(),base_pos);
			project_point(base_pos,perp,norm_y_wing);
			*depth=DISTANCE(bak_ls_pos,base_pos);
			coord_trans(bak_ls_pos,ucs,FALSE);
			if(bak_ls_pos.x>0)
				*depth*=-1.0;
		}
		return dd;
	}
}
double CLDSLineAngle::GetLsG(f3dPoint ls_norm,f3dPoint ls_pos,double *depth/*=NULL*/)
{
	f3dPoint perp,ls_vec,jg_vec,norm,base_pos;
	getUCS(ucs);
	if( fabs(norm_x_wing*ls_norm)
		>fabs(norm_y_wing*ls_norm))
	{	//X֫��˨
		SnapPerp(&perp,Start(),End(),ls_pos);
		base_pos = ls_pos;
		project_point(base_pos,perp,norm_x_wing);
		double dd = DistOf3dPtLine(base_pos,Start(),End());
		//�ж���˨�����ߵ���һ��
		ls_vec=base_pos-Start();
		jg_vec=End()-Start();
		normalize(ls_vec);
		normalize(jg_vec);
		norm=jg_vec^ls_vec;
		if(norm*norm_x_wing>0)
			dd=-dd;
		if(depth)
		{
			base_pos = ls_pos;
			coord_trans(base_pos,ucs,FALSE);
			*depth=-base_pos.y;
		}
		return dd;
	}
	else
	{	//Y֫��˨
		SnapPerp(&perp,Start(),End(),ls_pos);
		base_pos = ls_pos;
		project_point(base_pos,perp,norm_y_wing);
		double dd = DistOf3dPtLine(base_pos,Start(),End());
		//�ж���˨�����ߵ���һ��
		ls_vec=base_pos-Start();
		jg_vec=End()-Start();
		normalize(ls_vec);
		normalize(jg_vec);
		norm=jg_vec^ls_vec;
		if(norm*norm_y_wing<0)
			dd=-dd;
		if(depth)
		{
			f3dPoint perp,bak_ls_pos;
			base_pos = ls_pos;
			bak_ls_pos = base_pos;
			SnapPerp(&perp,Start(),End(),base_pos);
			project_point(base_pos,perp,norm_y_wing);
			*depth=DISTANCE(bak_ls_pos,base_pos);
			coord_trans(bak_ls_pos,ucs,FALSE);
			if(bak_ls_pos.x>0)
				*depth*=-1.0;
		}
		return dd;
	}
}
//ͨ��CLDSPart::HOLE��������˨�ף���������˨������������ף�ֱ�ӿ��������ɵĹ��տף����ϡ�wjh-2017.11.2
UINT CLDSLineAngle::AddMiscellaneousHoles(IXhList<HOLE>* pListHoles)
{	//��GetHoleSet�ڵ���,����ӳ�������˨����������
	return 0;
}

//����ֵΪ������
UINT CLDSLineAngle::GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole/*=false*/)
{
	if(pListHoles==NULL)
		return 0;
	UINT count=0;
	GEPOINT lenStdVec=End()-Start();
	GEPOINT vxWingVec=GetWingVecX();
	GEPOINT vyWingVec=GetWingVecY();
	normalize(lenStdVec);
	for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		count++;
		HOLE* pHole=pListHoles->Append(HOLE(pLsRef),pLsRef->pBolt->handle);
		int ciWingX0Y1=0;
		if(GetBoltIntersPos(pLsRef->pBolt,pHole->xMcsPosition,&ciWingX0Y1))
		{
			pHole->vHoleAxisVec=ciWingX0Y1==0?vxWingNorm:vyWingNorm;
			pHole->wBoltD=pLsRef->pBolt->d;
			pHole->wWaistLen=pLsRef->waistLen;
			GEPOINT xStartOrg=baseline.Start()-lenStdVec*startOdd();
			GEPOINT bolt_pos,bolt_vec=pHole->xMcsPosition-xStartOrg;
			pHole->xLocalPosition.x=bolt_vec*lenStdVec;
			if(ciWingX0Y1==0)
				pHole->xLocalPosition.y=-bolt_vec*vxWingVec;
			else
				pHole->xLocalPosition.y=bolt_vec*vyWingVec;
			pHole->ciResidePlane=ciWingX0Y1;
			char cWorkWing='X'+ciWingX0Y1;
			if((cWorkWing=='X'&&pHole->xLocalPosition.y>0)||(cWorkWing=='Y'&&pHole->xLocalPosition.y<0))
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X��˨ƫ��0x%X�Ǹֵ�%C����֫",pLsRef->pBolt->handle,handle,cWorkWing);
		}
		else
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"error");
	}
	UINT count2=bOnlyBoltHole?0:AddMiscellaneousHoles(pListHoles);
	return count+count2;
}
//���ciWingX0Y1,0:��˨��X֫;1:��˨��Y֫
bool CLDSLineAngle::GetBoltLocalPos(CLDSBolt *pBolt,double *local_pos,bool bIncOddment,int* ciWingX0Y1/*=NULL*/)
{
	f3dPoint inters;
	int wingx0_y1=0;
	//bool blValidIntersPoint = GetBoltIntersPos(pBolt, inters, &wingx0_y1);
	//GEPOINT vWorkWingNormal = wingx0_y1 == 0 ? this->vxWingNorm : this->vyWingNorm;
	//if (!blValidIntersPoint&&fabs(pBolt->ucs.axis_z*vWorkWingNormal) < EPS_COS)
	//	return 0;
	if(!GetBoltIntersPos(pBolt,inters,&wingx0_y1))
		return false;
	f3dPoint len_vec=End()-Start();
	normalize(len_vec);
	f3dPoint start_pos=Start();
	if(bIncOddment)
		start_pos-=len_vec*startOdd();
	f3dPoint bolt_pos,bolt_vec=inters-start_pos;
	bolt_pos.x=bolt_vec*len_vec;
	if(wingx0_y1==0)
		bolt_pos.y=-bolt_vec*GetWingVecX();
	else
		bolt_pos.y=bolt_vec*GetWingVecY();
	char cWorkWing='X'+wingx0_y1;
	if((cWorkWing=='X'&&bolt_pos.y>0)||(cWorkWing=='Y'&&bolt_pos.y<0))
		Log2File()->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"0x%X��˨ƫ��0x%X�Ǹֵ�%C����֫",pBolt->handle,handle,cWorkWing);
	if(ciWingX0Y1)
		*ciWingX0Y1=wingx0_y1;
	if(local_pos)
		memcpy(local_pos,bolt_pos,24);
	return true;//blValidIntersPoint?1:-1;
}
bool CLDSLineAngle::GetBoltIntersPos(CLDSBolt *pBolt,double* inters_coords,int* wingx0_y1,int iIntersPlane)	//��ȡ��˨�����Ǹ���Ƥ�Ľ���λ�� wjh-2015.4.8
{
	GEPOINT inters;
	f3dPoint worknorm=pBolt->ucs.axis_z;
	if(pBolt->m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
		worknorm*=-1.0;
	double ddx=worknorm*norm_x_wing;
	double ddy=worknorm*norm_y_wing;
	if( fabs(ddx)>fabs(ddy))	//X֫��˨
	{
		if(fabs(ddx)<EPS2)
			return false;	//��ֹ��˨��Ǹֽ���ƽ��ʱ�������쳣 wjh-2019.7.29
		int status=Int3dlf(inters,pBolt->ucs.origin,pBolt->get_norm(),Start(),norm_x_wing);
		if(status!=1)
			return false;
		if(iIntersPlane==CLDSPart::BOLTBTM_PLANE&&ddx>0)
			inters-=norm_x_wing*size.thick;
		if(wingx0_y1!=NULL)
			*wingx0_y1=0;
	}
	else if(fabs(ddy)<EPS2)
		return false;	//��ֹ��˨��Ǹֽ���ƽ��ʱ�������쳣 wjh-2019.7.29
	else
	{
		int status=Int3dlf(inters,pBolt->ucs.origin,pBolt->get_norm(),Start(),norm_y_wing);
		if(status!=1)
			return false;
		if(iIntersPlane==CLDSPart::BOLTBTM_PLANE&&ddy>0)
			inters-=norm_y_wing*size.thick;
		if(wingx0_y1!=NULL)
			*wingx0_y1=1;
	}
	if(inters_coords)
		memcpy(inters_coords,&inters,24);
	/*double D, H = 0, H2 = 0;
	FindLsD_H(pBolt->d, &D, &H, &H2);
	double zReverseLen = !pBolt->IsFootNail() ? H2 : pBolt->L - pBolt->dfLe;
	GEPOINT vzLenVec = f3dPoint(inters) - pBolt->ucs.origin;
	double zLenPos = vzLenVec * pBolt->ucs.axis_z;
	if (zLenPos > pBolt->L || zLenPos < -zReverseLen)
		return false;*/
	return true;
}

void CLDSLineAngle::SetWingXLsPos(CLsRef *pLsRef,double z,double g,double depth)
{
	f3dPoint z_vec,wing_vec_x,ls_pos;
	z_vec = End()-Start();
	normalize(z_vec);
	wing_vec_x = GetWingVecX();
	ls_pos = Start()+z_vec*z+wing_vec_x*g+depth*norm_x_wing;
	(*pLsRef)->ucs.origin=ls_pos;
	(*pLsRef)->SetModified();
}

void CLDSLineAngle::SetWingYLsPos(CLsRef *pLsRef,double z,double g,double depth)
{
	f3dPoint z_vec,wing_vec_y,ls_pos;
	z_vec = End()-Start();
	normalize(z_vec);
	wing_vec_y = GetWingVecY();
	ls_pos = Start()+z_vec*z+wing_vec_y*g+depth*norm_y_wing;
	(*pLsRef)->ucs.origin=ls_pos;
	(*pLsRef)->SetModified();
}
bool CLDSLineAngle::UpdateHoleWall(HOLE_WALL* pHoleWall)
{
	if(GetClassTypeId()!=CLS_LINEANGLE)
		return false;
	int iCurSelItem=-1;
	for(int i=0;i<4;i++)
	{
		if(pHoleWall->items[i].hRelaPart==handle)
		{
			iCurSelItem=i;
			break;
		}
	}
	if(iCurSelItem==-1)
		return false;
	double worklen=0;
	GEPOINT worknorm=pHoleWall->pBolt->get_norm();
	if(pHoleWall->pBolt->m_dwFlag.IsHasFlag(CLDSBolt::REVERSE_NORM))
		worknorm*=-1.0;
	GEPOINT worklenvec=pHoleWall->vEnd-pHoleWall->vStart;
	if(!worklenvec.IsZero())
	{
		if(worklenvec*worknorm<0)
			worknorm=-worknorm;
		worklen=worknorm*worklenvec;
	}
	GEPOINT vIntersStart,vIntersEnd,vLinePick,vFacePick,vFaceNorm;
	vLinePick=pHoleWall->pBolt->ucs.origin;
	double justify=1.0;
	double ddx=worknorm*norm_x_wing;
	double ddy=worknorm*norm_y_wing;
	if(fabs(ddx)>fabs(ddy))	//X֫��˨
	{
		justify=ddx;
		vFacePick=Start();
		vFaceNorm=norm_x_wing;
	}
	else	//Y֫��˨
	{
		justify=ddy;
		vFacePick=Start();
		vFaceNorm=norm_y_wing;
	}
	Int3dlf(vIntersEnd,vLinePick,worknorm,vFacePick,vFaceNorm);
	if (justify>0)
		vIntersStart=vIntersEnd-worknorm*(GetThick()/justify);
	else //if(justify<0)
	{
		vIntersStart=vIntersEnd;
		vIntersEnd-=worknorm*(GetThick()/justify);
	}
	pHoleWall->items[iCurSelItem].ptS=vIntersStart;
	pHoleWall->items[iCurSelItem].ptE=vIntersEnd;
	if(worklen==0)
	{
		pHoleWall->vStart=vIntersStart;
		pHoleWall->vEnd=vIntersEnd;
	}
	else
	{
		GEPOINT vLenVecToStart=vIntersStart-pHoleWall->vStart;
		GEPOINT vLenVecToEnd=vIntersEnd-pHoleWall->vStart;
		if(vLenVecToStart*worknorm<0)
			pHoleWall->vStart=vIntersStart;
		if(vLenVecToEnd*worknorm>worklen)
			pHoleWall->vEnd=vIntersEnd;
	}
	return true;
}
int CLDSLineAngle::GatherHoleWalls(CHash64ListEx<HOLE_WALL>& hashHoles)
{
	if(GetClassTypeId()!=CLS_LINEANGLE||CLDSObject::sysPara->HoleWallDisplayMode()==0)
		return 0;
	int gather_count=0;
	HOLE_WALL* pHoleWall;
	CXhSimpleList<HOLE_WALL*> listHoles;
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		if( (CLDSObject::sysPara->HoleWallDisplayMode()==1&&!pLsRef->GetLsPtr()->m_bVirtualPart)||(
			!CLDSObject::sysPara->IsDisplayAllHole()&&!m_bDisplayHole&&!pLsRef->GetLsPtr()->m_bVirtualPart))
			continue;
		pHoleWall=hashHoles.Add(DUALKEY((*pLsRef)->handle,handle));
		pHoleWall->pBolt=pLsRef->GetLsPtr();
		for(int j=0;j<4;j++)
		{
			if(pHoleWall->items[j].hRelaPart==handle)
				break;
			else if(pHoleWall->items[j].hRelaPart==0)
			{
				pHoleWall->items[j].hRelaPart=handle;
				break;
			}
		}
		listHoles.AttachObject(pHoleWall);
		gather_count++;
	}
	if(gather_count==0)
		return gather_count;
	//����ױڵ�ʼ�˺��ն�����
	GEPOINT origin=ucs.origin;
	for(HOLE_WALL** ppHole=listHoles.EnumObjectFirst();ppHole;ppHole=listHoles.EnumObjectNext())
		UpdateHoleWall(*ppHole);
	return gather_count;
}
struct CONNBOLT{
	CLDSBolt* pBolt;
	bool bDualShear;//��ͷ����˫����������˨
	int ciWingX0Y1;
	double lenposx;	//���ȷ���
	CONNBOLT(){pBolt=NULL;bDualShear=false;lenposx=ciWingX0Y1=0;}
};
static BOOL IsInSameGroup(CONNBOLT& prebolt,CONNBOLT& curbolt,long hCurJg,bool bInMutualWing=true,bool* pbIn1WrapAngle=NULL)
{
	THICK_ITEM item;
	double STEP1=90,STEP2=225;
	if(fabs(prebolt.lenposx-curbolt.lenposx)<STEP1&&bInMutualWing)
	{	//�ں������˨�����ȫ��һ�µĹ�ͬ��������֫��,�����ٴ����������ӹ��� wjh-2017.8.24
		CParaThick *pParaThick=curbolt.pBolt->GetL0ThickPara();
		bool hasElsePart=false;
		for(item=pParaThick->GetFirstItem();item.val!=0&&!hasElsePart;item=pParaThick->GetNextItem())
		{
			if(item.IsHandle()&&abs(item.val)==hCurJg)
				continue;
			else if(item.IsHandle())
				hasElsePart=true;
		}
		return hasElsePart;
	}
	if(fabs(prebolt.lenposx-curbolt.lenposx)>STEP2)
		return FALSE;	//������������Ч���ֵ
	//�ж�����˨�Ƿ���ͬ������
	if(prebolt.pBolt==NULL || curbolt.pBolt==NULL)
		return FALSE;
	CHashSet<CLDSPart*> xPartHash;
	CParaThick *pPreThick=prebolt.pBolt->GetL0ThickPara();
	for(item=pPreThick->GetFirstItem();item.val!=0;item=pPreThick->GetNextItem())
	{
		if(!item.IsHandle() || abs(item.val)==hCurJg)
			continue;
		CLDSPart *pPart=prebolt.pBolt->BelongModel()->FromPartHandle(abs(item.val));
		if(pPart&&(pPart->GetClassTypeId()!=CLS_BOLT))
			xPartHash.SetValue(pPart->handle,pPart);
	}
	if(pbIn1WrapAngle)
		*pbIn1WrapAngle=false;
	CParaThick *pCurThick=curbolt.pBolt->GetL0ThickPara();
	for(THICK_ITEM item=pCurThick->GetFirstItem();item.val!=0;item=pCurThick->GetNextItem())
	{
		if(!item.IsHandle() || abs(item.val)==hCurJg)
			continue;	//�����ǵ�ǰ���ڽǸ�
		CLDSPart* pSharePart=xPartHash.GetValue(abs(item.val));
		if(pSharePart!=NULL)
		{
			if(pbIn1WrapAngle)
				*pbIn1WrapAngle=pSharePart->IsAngle();
			return TRUE;
		}
	}
	return FALSE;
}
static int _LocalCompareConnBolt(const CONNBOLT& bolt1,const CONNBOLT& bolt2)
{
	if(bolt1.lenposx-bolt2.lenposx>1)
		return 1;
	else if(bolt1.lenposx-bolt2.lenposx<-1)
		return -1;
	else if(bolt1.pBolt->d>bolt2.pBolt->d)
		return -1;
	else if(bolt1.pBolt->d<bolt2.pBolt->d)
		return 1;
	else
		return 0;
}
static bool IsInArr(long iVal,long* iarr,int size)
{
	for(int i=0;i<size;i++)
		if(iarr[i]==iVal)
			return true;
	return false;
}
struct INTER_ANGLE{
	BYTE ciResideWing;	//0x01:X֫��0x02��Y֫
	long hInterAngle;
	INTER_ANGLE(){hInterAngle=ciResideWing=0;}
	BYTE UpdateConnAngleWing(CONNBOLT* pConnBolt,long hExcludeRod)
	{
		for(THICK_ITEM item=pConnBolt->pBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pConnBolt->pBolt->GetNextL0ThickItem())
		{
			if(!item.IsFinalKey())
				continue;
			if(item.val==hExcludeRod)
				continue;
			if(hInterAngle==0&&pConnBolt->pBolt->BelongModel()->FromPartHandle(item.val,CLS_LINEANGLE)!=NULL)
			{
				hInterAngle=item.val;
				ciResideWing|=pConnBolt->ciWingX0Y1==0?0x01:0x02;
				break;
			}
			else if(hInterAngle==item.val)
			{
				ciResideWing|=pConnBolt->ciWingX0Y1==0?0x01:0x02;
				break;
			}
		}
		return ciResideWing;
	}
};
bool CLDSLineAngle::GetEffectiveEndBolts(ENDBOLTS* pEndBolts,bool bRetrieveStart/*=true*/)
{
	int i;
	int nM16=0,nM20=0,nM24=0;
	pEndBolts->Clear();
	if(size.idClassType==CLS_LINEANGLE)
	{	//���Ǹ�
		double length=GetLength();
		ARRAY_LIST<CONNBOLT> arrbolts;
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			GEPOINT xLocalPos;
			int ciWingX0Y1,cbWingX1Y2=0;
			if(pLsRef->GetLsPtr()->m_bVirtualPart)
				continue;
			if(!GetBoltLocalPos(pLsRef->GetLsPtr(),xLocalPos,true,&ciWingX0Y1))
				continue;
			CONNBOLT* pConnBolt=arrbolts.append();
			pConnBolt->pBolt=pLsRef->GetLsPtr();
			pConnBolt->ciWingX0Y1=ciWingX0Y1;
			pConnBolt->lenposx=xLocalPos.x;
		}
		CQuickSort<CONNBOLT>::QuickSort(arrbolts.m_pData,arrbolts.GetSize(),_LocalCompareConnBolt);
		if(arrbolts.GetSize()==0)
			return false;
		INTER_ANGLE xWrapAngle;
		if(bRetrieveStart)
		{	//��ȡʼ��������˨
			int STEP=125,iPrevBolt=0,iFirstValidBolt=0;
			for(i=0;i<arrbolts.GetSize();i++)
			{
				if(arrbolts[i].lenposx>0&&arrbolts[i].lenposx<length)
					break;
			}
			if((iPrevBolt=iFirstValidBolt=i)!=0)
				Log2File()->Log("0x%X�Ǹ�{%s}���ڳ����Ǹֳ��ȷ�Χ����˨0x%X",handle,Label,arrbolts[0].pBolt->handle);
			int cbMaskConnWing=(connectStart.m_iConnectWing==0)?0x03:0;
			if((Layer(1)=='Z'&&Layer(0)!='T')||connectStart.m_iConnectWing==0)
				cbMaskConnWing=0x03;
			if(arrbolts[iFirstValidBolt].ciWingX0Y1==0)
				cbMaskConnWing|=0x01;
			else
				cbMaskConnWing|=0x02;
			for(i=iFirstValidBolt;i<arrbolts.GetSize();i++)
			{
				CONNBOLT* pConnBolt=&arrbolts[i];
				if(i==iFirstValidBolt)
				{
					if(pConnBolt->lenposx>100)
					{
						pEndBolts->n16=pEndBolts->n20=pEndBolts->n24=pEndBolts->nX=0;
						pEndBolts->Avf=pEndBolts->Ahf=0;
						return false;
					}
					else if(pConnBolt->pBolt->get_d()==16)
					{
						pEndBolts->n16++;
						if(pEndBolts->n16>pEndBolts->n20&&pEndBolts->n16>pEndBolts->n24)
							StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
						pEndBolts->Avf=Pi*64;
						pEndBolts->Ahf=size.thick*16;
					}
					else if(pConnBolt->pBolt->get_d()==20)
					{
						pEndBolts->n20++;
						if(pEndBolts->n20>pEndBolts->n16&&pEndBolts->n20>pEndBolts->n24)
							StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
						pEndBolts->Avf=Pi*100;
						pEndBolts->Ahf=size.thick*20;
					}
					else if(pConnBolt->pBolt->get_d()==24)
					{
						pEndBolts->n24++;
						if(pEndBolts->n24>pEndBolts->n16&&pEndBolts->n24>pEndBolts->n20)
							StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
						pEndBolts->Avf=Pi*144;
						pEndBolts->Ahf=size.thick*24;
					}
					//��Ϊ��˨����ǿ�����㲻������˨����ǿ�Ȼ��ܱ����ӹ������Ͽױڳ�ѹǿ������, �ʳ�4.8����
					//һ���ͳһΪ6.8��,ֻ����˨ֱ��������������˨ǿ�ȵȼ����� wjh-2017.8.24
					if(strcmp(pConnBolt->pBolt->Grade(),"4.8")==0)
					{
						pEndBolts->Avf*=170;
						pEndBolts->Ahf*=420;
					}
					else //if(strcmp(pConnBolt->pBolt->Grade(),"6.8")==0)
					{
						pEndBolts->Avf*=240;
						pEndBolts->Ahf*=600;
					}
					/*else if(strcmp(pConnBolt->pBolt->Grade(),"8.8")==0)
					{
						pEndBolts->Avf*=300;
						pEndBolts->Ahf*=800;
					}*/
					if(xWrapAngle.ciResideWing!=0x03)	//ʶ���Ƿ��ͷ�����а���
						xWrapAngle.UpdateConnAngleWing(pConnBolt,handle);
					continue;
				}

				bool bInMutualWing=true,bIn1WrapAngle=false;
				if(pConnBolt->ciWingX0Y1!=arrbolts[iFirstValidBolt].ciWingX0Y1&&cbMaskConnWing!=0x03)
					bInMutualWing=false;	//��֫����ʱ�ǵ�ǰ����֫��˨
				if(!IsInSameGroup(arrbolts[iPrevBolt],arrbolts[i],handle,bInMutualWing,&bIn1WrapAngle))
				{
					if(fabs(arrbolts[i].lenposx-arrbolts[iPrevBolt].lenposx)>=125)
						break;
					else
						continue;
				}
				double Avf=0,Ahf=0;
				if(pConnBolt->pBolt->get_d()==16)
				{
					pEndBolts->n16++;
					if(pEndBolts->n16>pEndBolts->n20&&pEndBolts->n16>pEndBolts->n24)
						StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
					Avf=Pi*64;
					Ahf=size.thick*16;
				}
				else if(pConnBolt->pBolt->get_d()==20)
				{
					pEndBolts->n20++;
					if(pEndBolts->n20>pEndBolts->n16&&pEndBolts->n20>pEndBolts->n24)
						StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
					Avf=Pi*100;
					Ahf=size.thick*20;
				}
				else if(pConnBolt->pBolt->get_d()==24)
				{
					pEndBolts->n24++;
					if(pEndBolts->n24>pEndBolts->n16&&pEndBolts->n24>pEndBolts->n20)
						StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
					Avf=Pi*144;
					Ahf=size.thick*24;
				}
				//��Ϊ��˨����ǿ�����㲻������˨����ǿ�Ȼ��ܱ����ӹ������Ͽױڳ�ѹǿ������, �ʳ�4.8����
				//һ���ͳһΪ6.8��,ֻ����˨ֱ��������������˨ǿ�ȵȼ����� wjh-2017.8.24
				if(strcmp(pConnBolt->pBolt->Grade(),"4.8")==0)
				{
					Avf*=170;
					Ahf*=420;
				}
				else //if(strcmp(pConnBolt->pBolt->Grade(),"6.8")==0)
				{
					Avf*=240;
					Ahf*=600;
				}
				/*else if(strcmp(pConnBolt->pBolt->Grade(),"8.8")==0)
				{
					Avf*=300;
					Ahf*=800;
				}*/
				pEndBolts->Avf+=Avf;
				pEndBolts->Ahf+=Ahf;
				if(xWrapAngle.ciResideWing!=0x03)
					xWrapAngle.UpdateConnAngleWing(pConnBolt,handle);
				iPrevBolt=i;
			}
			if(xWrapAngle.ciResideWing==0x03)
				pEndBolts->Avf*=2;
		}
		else
		{	//��ȡ�ն�������˨
			int STEP=125,lastI=arrbolts.GetSize()-1;
			int iPrevBolt=lastI,iFirstValidBolt=lastI;
			for(i=lastI;i>=0;i--)
			{
				if(arrbolts[i].lenposx>0&&arrbolts[i].lenposx<length)
					break;
			}
			if((iPrevBolt=iFirstValidBolt=i)!=lastI)
				Log2File()->Log("0x%X�Ǹ�{%s}���ڳ����Ǹֳ��ȷ�Χ����˨0x%X",handle,Label,arrbolts[lastI].pBolt->handle);
			int cbMaskConnWing=(connectEnd.m_iConnectWing==0)?0x03:0;
			if((Layer(1)=='Z'&&Layer(0)!='T')||connectEnd.m_iConnectWing==0)
				cbMaskConnWing=0x03;
			if(arrbolts[iFirstValidBolt].ciWingX0Y1==0)
				cbMaskConnWing|=0x01;
			else
				cbMaskConnWing|=0x02;
			for(i=iFirstValidBolt;i>=0;i--)
			{
				CONNBOLT* pConnBolt=&arrbolts[i];
				if(i==iFirstValidBolt)
				{
					if(length-pConnBolt->lenposx>100)
					{
						pEndBolts->n16=pEndBolts->n20=pEndBolts->n24=pEndBolts->nX=0;
						pEndBolts->Avf=pEndBolts->Ahf=0;
						return false;
					}
					else if(pConnBolt->pBolt->get_d()==16)
					{
						pEndBolts->n16++;
						if(pEndBolts->n16>pEndBolts->n20&&pEndBolts->n16>pEndBolts->n24)
							StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
						pEndBolts->Avf=Pi*64;
						pEndBolts->Ahf=size.thick*16;
					}
					else if(pConnBolt->pBolt->get_d()==20)
					{
						pEndBolts->n20++;
						if(pEndBolts->n20>pEndBolts->n16&&pEndBolts->n20>pEndBolts->n24)
							StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
						pEndBolts->Avf=Pi*100;
						pEndBolts->Ahf=size.thick*20;
					}
					else if(pConnBolt->pBolt->get_d()==24)
					{
						pEndBolts->n24++;
						if(pEndBolts->n24>pEndBolts->n16&&pEndBolts->n24>pEndBolts->n20)
							StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
						pEndBolts->Avf=Pi*144;
						pEndBolts->Ahf=size.thick*24;
					}
					//��Ϊ��˨����ǿ�����㲻������˨����ǿ�Ȼ��ܱ����ӹ������Ͽױڳ�ѹǿ������, �ʳ�4.8����
					//һ���ͳһΪ6.8��,ֻ����˨ֱ��������������˨ǿ�ȵȼ����� wjh-2017.8.24
					if(strcmp(pConnBolt->pBolt->Grade(),"4.8")==0)
					{
						pEndBolts->Avf*=170;
						pEndBolts->Ahf*=420;
					}
					else //if(strcmp(pConnBolt->pBolt->Grade(),"6.8")==0)
					{
						pEndBolts->Avf*=240;
						pEndBolts->Ahf*=600;
					}
					/*else if(strcmp(pConnBolt->pBolt->Grade(),"8.8")==0)
					{
						pEndBolts->Avf*=300;
						pEndBolts->Ahf*=800;
					}*/
					if(xWrapAngle.ciResideWing!=0x03)	//ʶ���Ƿ��ͷ�����а���
						xWrapAngle.UpdateConnAngleWing(pConnBolt,handle);
					continue;
				}

				bool bInMutualWing=true,bIn1WrapAngle=false;
				if(pConnBolt->ciWingX0Y1!=arrbolts[iFirstValidBolt].ciWingX0Y1&&cbMaskConnWing!=0x03)
					bInMutualWing=false;	//��֫����ʱ�ǵ�ǰ����֫��˨
				if(!IsInSameGroup(arrbolts[iPrevBolt],arrbolts[i],handle,bInMutualWing,&bIn1WrapAngle))
				{
					if(fabs(arrbolts[i].lenposx-arrbolts[iPrevBolt].lenposx)>=125)
						break;
					else
						continue;
				}
				double Avf=0,Ahf=0;
				if(pConnBolt->pBolt->get_d()==16)
				{
					pEndBolts->n16++;
					if(pEndBolts->n16>pEndBolts->n20&&pEndBolts->n16>pEndBolts->n24)
						StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
					Avf=Pi*64;
					Ahf=size.thick*16;
				}
				else if(pConnBolt->pBolt->get_d()==20)
				{
					pEndBolts->n20++;
					if(pEndBolts->n20>pEndBolts->n16&&pEndBolts->n20>pEndBolts->n24)
						StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
					Avf=Pi*100;
					Ahf=size.thick*20;
				}
				else if(pConnBolt->pBolt->get_d()==24)
				{
					pEndBolts->n24++;
					if(pEndBolts->n24>pEndBolts->n16&&pEndBolts->n24>pEndBolts->n20)
						StrCopy(pEndBolts->grade,pConnBolt->pBolt->Grade(),5);
					Avf=Pi*144;
					Ahf=size.thick*24;
				}
				//��Ϊ��˨����ǿ�����㲻������˨����ǿ�Ȼ��ܱ����ӹ������Ͽױڳ�ѹǿ������, �ʳ�4.8����
				//һ���ͳһΪ6.8��,ֻ����˨ֱ��������������˨ǿ�ȵȼ����� wjh-2017.8.24
				if(strcmp(pConnBolt->pBolt->Grade(),"4.8")==0)
				{
					Avf*=170;
					Ahf*=420;
				}
				else //if(strcmp(pConnBolt->pBolt->Grade(),"6.8")==0)
				{
					Avf*=240;
					Ahf*=600;
				}
				/*else if(strcmp(pConnBolt->pBolt->Grade(),"8.8")==0)
				{
					Avf*=300;
					Ahf*=800;
				}*/
				pEndBolts->Avf+=Avf;
				pEndBolts->Ahf+=Ahf;
				if(xWrapAngle.ciResideWing!=0x03)	//ʶ���Ƿ��ͷ�����а���
					xWrapAngle.UpdateConnAngleWing(pConnBolt,handle);
				iPrevBolt=i;
			}
			if(xWrapAngle.ciResideWing==0x03)	//ʶ���Ƿ��ͷ�����а���
				pEndBolts->Avf*=2;
		}
	}
	else //if(size.idClassType==CLS_GROUPLINEANGLE)
	{	//��ϽǸ�
		CLDSLineAngle* arrangles[4]={NULL};
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)this;
		for(i=0;i<4;i++)
			arrangles[i]=(CLDSLineAngle*)m_pModel->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
		for(i=0;i<4&&arrangles[i]!=NULL;i++)
		{
			CLDSLinePart::ENDBOLTS endbolts;
			if(!arrangles[i]->GetEffectiveEndBolts(&endbolts,bRetrieveStart))
				return false;
			pEndBolts->n16+=endbolts.n16;
			pEndBolts->n20+=endbolts.n20;
			pEndBolts->n24+=endbolts.n24;
			pEndBolts->nX +=endbolts.nX;
			pEndBolts->Avf+=endbolts.Avf;
			pEndBolts->Ahf+=endbolts.Ahf;
			if(i==0)
				StrCopy(pEndBolts->grade,endbolts.grade,5);
		}
	}
	return true;
}
//<DEVELOP_PROCESS_MARK nameId="CLDSLineAngle::GetStartInitPos">
bool CLDSLineAngle::GetStartInitPos(f3dPoint &init_pos)	//��ȡʼ�˶�λ��ʼλ��(���ݲ�������false)
{	//��ȡʼ�˶�λ��ʼλ��
	double spec_g=0;	//��ʼֵ
	int origin_pos_style=desStartPos.origin_pos_style;
	if(desStartPos.spatialOperationStyle==15||desStartPos.spatialOperationStyle==18||desStartPos.spatialOperationStyle==19)
		origin_pos_style=0;	//���ִӾɰ��ļ��̳й���������ģ�Ϳ��ܴ���=1�Ŀ��ܣ���ʱ����λ��ʽ��Ϊ��������ʱ�������ǿ���԰��ڵ�λ�ô���wjh-2016.11.18
	int operStyle=desStartPos.datumWingOffset.operStyle;
	BYTE cDatumAngleWing=desStartPos.cDatumAngleWing;
	CSuperSmartPtr<CLDSLinePart>pDatumRod;
	if(desStartPos.origin_pos_style==0)
	{
		pDatumRod = GetStartDatumPart();
		if(IsLappedAnchorStart()&&pDatumRod.IsHasPtr()&&pDatumRod->IsAngle())
		{
			if(pDatumRod.LineAnglePointer()->m_cPosCtrlDatumLineType=='X')
			{
				origin_pos_style=2;
				operStyle=pDatumRod.LineAnglePointer()->m_cPosCtrlType;
				spec_g=pDatumRod.LineAnglePointer()->GetWingXG(0);
				cDatumAngleWing='X';
			}
			else if(pDatumRod.LineAnglePointer()->m_cPosCtrlDatumLineType=='Y')
			{
				origin_pos_style=2;
				operStyle=pDatumRod.LineAnglePointer()->m_cPosCtrlType;
				spec_g=pDatumRod.LineAnglePointer()->GetWingYG(0);
				cDatumAngleWing='Y';
			}
		}
		else if(desStartPos.spatialOperationStyle==16||desStartPos.spatialOperationStyle==17)
		{
			if(pStart&&pStart->m_cPosCalType==4)	//�����
				desStartPos.origin_pos_style=origin_pos_style=5;	//�����ǿ���Ը�Ϊ����ʶ���
		}
	}
	else if(desStartPos.origin_pos_style==2)
	{
		pDatumRod=GetStartDatumPart();
		if(pDatumRod.IsHasPtr()&&pDatumRod->IsAngle())
		{
			if(desStartPos.cDatumAngleWing=='X')	//X֫�����Ϲؼ���
			{	
				if(desStartPos.datumWingOffset.gStyle<4)
					spec_g=pDatumRod.LineAnglePointer()->GetWingXG(desStartPos.datumWingOffset.gStyle);
				else
					spec_g=desStartPos.datumWingOffset.offsetDist;
			}
			else if(desStartPos.cDatumAngleWing=='Y')	//Y֫�����Ϲؼ���
			{	
				if(desStartPos.datumWingOffset.gStyle<4)
					spec_g=pDatumRod.LineAnglePointer()->GetWingYG(desStartPos.datumWingOffset.gStyle);
				else
					spec_g=desStartPos.datumWingOffset.offsetDist;
			}
		}
		else
			return false;
	}

	if(origin_pos_style==0)
	{	//�ڵ�λ��
		if(pStart!=NULL)
			init_pos = pStart->Position(true);
		else
			return false;
	}
	else if(origin_pos_style==1)
	{
		desStartPos.datumPoint.UpdatePos(BelongModel());
		init_pos = desStartPos.datumPoint.Position();	//ָ����׼��λ��
		return true;
	}
	else if(origin_pos_style==2&&pStart)
	{	//�Ǹֹؼ��㡢
		if(cDatumAngleWing=='X')	//X֫�����Ϲؼ���
		{	
			//��������0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
			if(operStyle==0)
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosWingX(pStart,spec_g);
			else if(operStyle==1)	//X-Yƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,f3dPoint(0,0,1),NULL,spec_g);
			else if(operStyle==2)	//Y-Zƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,f3dPoint(1,0,0),NULL,spec_g);
			else if(operStyle==3)	//X-Zƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosWingXByFaceOffset(pStart,f3dPoint(0,1,0),NULL,spec_g);
			else
			{
				logto.Log("error @GetStartInitPos");
				return false;
			}
		}
		else if(cDatumAngleWing=='Y')	//Y֫�����Ϲؼ���
		{	
			//��������0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
			if(operStyle==0)
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosWingY(pStart,spec_g);
			else if(operStyle==1)	//X-Yƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,f3dPoint(0,0,1),NULL,spec_g);
			else if(operStyle==2)	//Y-Zƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,f3dPoint(1,0,0),NULL,spec_g);
			else if(operStyle==3)	//X-Zƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosWingYByFaceOffset(pStart,f3dPoint(0,1,0),NULL,spec_g);
			else
			{
				logto.Log("error @GetStartInitPos");
				return false;
			}
		}
		else
		{
			//��������0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
			if(desStartPos.datumWingOffset.operStyle==0)
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosBer(pStart);
			else if(desStartPos.datumWingOffset.operStyle==1)	//X-Yƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,f3dPoint(0,0,1),NULL);
			else if(desStartPos.datumWingOffset.operStyle==2)	//Y-Zƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,f3dPoint(1,0,0),NULL);
			else if(desStartPos.datumWingOffset.operStyle==3)	//X-Zƽ��ƽ��
				init_pos = pDatumRod.LineAnglePointer()->GetDatumPosBerByFaceOffset(pStart,f3dPoint(0,1,0),NULL);
			else
			{
				logto.Log("error @GetStartInitPos");
				return false;
			}
		}
	}
	else if(origin_pos_style==3)
	{	//��һ�˽ڵ�λ��
		if(pEnd!=NULL)
			init_pos = pEnd->Position(true);
		else
			return false;
	}
	else if(origin_pos_style==4)
		init_pos = End();	//��һ�����λ��
	else if(origin_pos_style==5&&pStart)
	{	//�����ǿ���Ը�Ϊ����ʶ���
		if(pStart->m_cPosCalType==CLDSNode::COORD_INTERSECTION)
			pStart->GetIntersZhun(&init_pos);	//�����
		else if(pStart->m_cPosCalType==CLDSNode::COORD_SCALE)
		{	//�����ȷֵ�
			bool inited=false;
			CLDSNode* pAttachNode1=BelongModel()->FromNodeHandle(pStart->arrRelationNode[0]);
			CLDSNode* pAttachNode2=BelongModel()->FromNodeHandle(pStart->arrRelationNode[1]);
			if(pAttachNode1!=NULL&&pAttachNode2!=NULL)
			{
				f3dPoint lineVec,lineStdVec=pAttachNode2->Position()-pAttachNode1->Position();
				normalize(lineStdVec);
				//�����жϽڵ㸸�˼��Ƿ����Ϊ��׼�˼�
				CLDSLinePart* pStartDatumRod=NULL,*pEndDatumRod=NULL;
				CLDSLinePart* pRod=BelongModel()->FromRodHandle(pAttachNode1->hFatherPart);
				if(pRod&&pRod->pStart!=NULL&&pRod->pEnd!=NULL)
				{
					lineVec=pRod->pEnd->Position()-pRod->pStart->Position();
					if(lineVec.IsEqual((lineVec*lineStdVec)*lineStdVec))
						pStartDatumRod=pRod;	//�繲��(lineVec*lineStdVec)*lineStdVecӦ��lineVecһ��
				}
				pRod=BelongModel()->FromRodHandle(pAttachNode2->hFatherPart);
				if(pRod&&pRod->pStart!=NULL&&pRod->pEnd!=NULL)
				{
					lineVec=pRod->pEnd->Position()-pRod->pStart->Position();
					if(lineVec.IsEqual((lineVec*lineStdVec)*lineStdVec))
						pEndDatumRod=pRod;	//�繲��(lineVec*lineStdVec)*lineStdVecӦ��lineVecһ��
				}
				if(pStartDatumRod==NULL||pEndDatumRod==NULL)
				{
					CHandleReplacer filter(0,0);
					BOOL pushed=BelongModel()->PushPartStack();
					for(pRod=BelongModel()->EnumRodFirst();pRod;pRod=BelongModel()->EnumRodNext())
					{
						if( pRod->pStart==NULL||pRod->pEnd==NULL||
							pRod->handle==pAttachNode1->hFatherPart||pRod->handle==pAttachNode2->hFatherPart)
							continue;	//���ǺϷ��˼������������Ĵ���
						GEPOINT lineStart=pRod->pStart->Position();
						GEPOINT lineEnd=pRod->pEnd->Position();
						lineVec=lineEnd-lineStart;
						if(!lineVec.IsEqual((lineVec*lineStdVec)*lineStdVec))
							continue;	//�繲��(lineVec*lineStdVec)*lineStdVecӦ��lineVecһ��
						f3dPoint vertDistVec=f3dPoint(lineStart)-pAttachNode1->Position();
						if(!vertDistVec.IsEqual((vertDistVec*lineStdVec)*lineStdVec))
							continue;	//ʼ�˵㲻�ڵȷֶ�������
						filter.AppendSectRod(lineStart,lineEnd,pRod->handle);
					}
					if(pStartDatumRod==NULL)
					{
						if(filter.SelectPerfectPartHandle(pAttachNode1->Position())>0)
							pStartDatumRod=BelongModel()->FromRodHandle(filter.hNew);
					}
					if(pEndDatumRod==NULL)
					{
						if(filter.SelectPerfectPartHandle(pAttachNode2->Position())>0)
							pEndDatumRod=BelongModel()->FromRodHandle(filter.hNew);
					}
					BelongModel()->PopPartStack(pushed);
				}
				if(pStartDatumRod!=NULL&&pEndDatumRod!=NULL)
				{
					GEPOINT datumStart,datumEnd;
					if(pStartDatumRod->IsAngle())
					{
						if(desStartPos.cFaceOffsetDatumLine=='X')
							datumStart=((CLDSLineAngle*)pStartDatumRod)->GetDatumPosWingX(pAttachNode1);
						else if(desStartPos.cFaceOffsetDatumLine=='Y')
							datumStart=((CLDSLineAngle*)pStartDatumRod)->GetDatumPosWingY(pAttachNode1);
						else
							datumStart=((CLDSLineAngle*)pStartDatumRod)->GetDatumPosBer(pAttachNode1);
					}
					else
						pStartDatumRod->GetDatumPos(pAttachNode1);
					if(pEndDatumRod->IsAngle())
					{
						if(desStartPos.cFaceOffsetDatumLine=='X')
							datumEnd=((CLDSLineAngle*)pEndDatumRod)->GetDatumPosWingX(pAttachNode2);
						else if(desStartPos.cFaceOffsetDatumLine=='Y')
							datumEnd=((CLDSLineAngle*)pEndDatumRod)->GetDatumPosWingY(pAttachNode2);
						else
							datumEnd=((CLDSLineAngle*)pEndDatumRod)->GetDatumPosBer(pAttachNode2);
					}
					else
						pEndDatumRod->GetDatumPos(pAttachNode2);
					init_pos=datumStart+(datumEnd-datumStart)*pStart->attach_scale;
					inited=true;
				}
			}
			if(!inited)
				init_pos = pStart->Position(true);
		}
		//else if(pStart->m_cPosCalType==CLDSNode::COORD_OFFSET)
		//{	//ƫ�Ƶ�
		//}
		else
			init_pos = pStart->Position(true);
	}
	else
		return false;
	return true;
}
//</DEVELOP_PROCESS_MARK>
f3dPoint CLDSLineAngle::GetStartInitPos()
{
	f3dPoint init_pos;
	if(GetStartInitPos(init_pos))
		return init_pos;
	else
		return Start();
}

bool CLDSLineAngle::GetEndInitPos(f3dPoint &init_pos)	//��ȡ�ն˶�λ��ʼλ��(���ݲ�������false)
{	//��ȡ�ն˶�λ��ʼλ��
	double spec_g=0;	//��ʼֵ
	int origin_pos_style=desEndPos.origin_pos_style;
	if(desEndPos.spatialOperationStyle==15||desEndPos.spatialOperationStyle==18||desEndPos.spatialOperationStyle==19)
		origin_pos_style=0;	//���ִӾɰ��ļ��̳й���������ģ�Ϳ��ܴ���=1�Ŀ��ܣ���ʱ����λ��ʽ��Ϊ��������ʱ�������ǿ���԰��ڵ�λ�ô���wjh-2016.11.18
	int operStyle=desEndPos.datumWingOffset.operStyle;
	BYTE cDatumAngleWing=desEndPos.cDatumAngleWing;
	CSuperSmartPtr<CLDSLinePart>pDatumRod;
	if(desEndPos.origin_pos_style==0)
	{
		pDatumRod = GetEndDatumPart();
		if(IsLappedAnchorEnd()&&pDatumRod.IsHasPtr()&&pDatumRod->IsAngle())
		{
			if(pDatumRod.LineAnglePointer()->m_cPosCtrlDatumLineType=='X')
			{
				origin_pos_style=2;
				operStyle=pDatumRod.LineAnglePointer()->m_cPosCtrlType;
				spec_g=pDatumRod.LineAnglePointer()->GetWingXG(0);
				cDatumAngleWing='X';
			}
			else if(pDatumRod.LineAnglePointer()->m_cPosCtrlDatumLineType=='Y')
			{
				origin_pos_style=2;
				operStyle=pDatumRod.LineAnglePointer()->m_cPosCtrlType;
				spec_g=pDatumRod.LineAnglePointer()->GetWingYG(0);
				cDatumAngleWing='Y';
			}
		}
		else if(desEndPos.spatialOperationStyle==16||desEndPos.spatialOperationStyle==17)
		{
			if(pEnd&&pEnd->m_cPosCalType==4)	//�����
				desEndPos.origin_pos_style=origin_pos_style=5;	//�����ǿ���Ը�Ϊ����ʶ���
		}
	}
	else if(desEndPos.origin_pos_style==2)
	{
		pDatumRod=GetEndDatumPart();
		if(pDatumRod.IsHasPtr()&&pDatumRod->IsAngle())
		{
			if(desEndPos.cDatumAngleWing=='X')	//X֫�����Ϲؼ���
			{	
				if(desEndPos.datumWingOffset.gStyle<4)
					spec_g=pDatumRod.LineAnglePointer()->GetWingXG(desEndPos.datumWingOffset.gStyle);
				else
					spec_g=desEndPos.datumWingOffset.offsetDist;
			}
			else if(desEndPos.cDatumAngleWing=='Y')	//Y֫�����Ϲؼ���
			{	
				if(desEndPos.datumWingOffset.gStyle<4)
					spec_g=pDatumRod.LineAnglePointer()->GetWingYG(desEndPos.datumWingOffset.gStyle);
				else
					spec_g=desEndPos.datumWingOffset.offsetDist;
			}
		}
		else
			return false;
	}
	if(origin_pos_style==0)
	{	//�ڵ�λ��
		if(pEnd!=NULL)
			init_pos = pEnd->Position(true);
		else
			return false;
	}
	else if(origin_pos_style==1)
	{
		desEndPos.datumPoint.UpdatePos(BelongModel());
		init_pos = desEndPos.datumPoint.Position();	//ָ����׼��λ��
	}
	else if(origin_pos_style==2&&pEnd)
	{	//�Ǹֹؼ���
		CLDSLineAngle *pDatumAngle=pDatumRod.LineAnglePointer();
		if(cDatumAngleWing=='X')	//X֫�����Ϲؼ���
		{	
			//��������0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
			if(operStyle==0)
				init_pos = pDatumAngle->GetDatumPosWingX(pEnd,spec_g);
			else if(operStyle==1)	//X-Yƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosWingXByFaceOffset(pEnd,f3dPoint(0,0,1),NULL,spec_g);
			else if(operStyle==2)	//Y-Zƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosWingXByFaceOffset(pEnd,f3dPoint(1,0,0),NULL,spec_g);
			else if(operStyle==3)	//X-Zƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosWingXByFaceOffset(pEnd,f3dPoint(0,1,0),NULL,spec_g);
			else
			{
				logto.Log("error @GetEndInitPos");
				return false;
			}
		}
		else if(cDatumAngleWing=='Y')	//Y֫�����Ϲؼ���
		{	
			//��������0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
			if(operStyle==0)
				init_pos = pDatumAngle->GetDatumPosWingY(pEnd,spec_g);
			else if(operStyle==1)	//X-Yƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosWingYByFaceOffset(pEnd,f3dPoint(0,0,1),NULL,spec_g);
			else if(operStyle==2)	//Y-Zƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosWingYByFaceOffset(pEnd,f3dPoint(1,0,0),NULL,spec_g);
			else if(operStyle==3)	//X-Zƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosWingYByFaceOffset(pEnd,f3dPoint(0,1,0),NULL,spec_g);
			else
			{
				logto.Log("error @GetEndInitPos");
				return false;
			}
		}
		else
		{
			//��������0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
			if(desEndPos.datumWingOffset.operStyle==0)
				init_pos = pDatumAngle->GetDatumPosBer(pEnd);
			else if(desEndPos.datumWingOffset.operStyle==1)	//X-Yƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosBerByFaceOffset(pEnd,f3dPoint(0,0,1),NULL);
			else if(desEndPos.datumWingOffset.operStyle==2)	//Y-Zƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosBerByFaceOffset(pEnd,f3dPoint(1,0,0),NULL);
			else if(desEndPos.datumWingOffset.operStyle==3)	//X-Zƽ��ƽ��
				init_pos = pDatumAngle->GetDatumPosBerByFaceOffset(pEnd,f3dPoint(0,1,0),NULL);
			else
			{
				logto.Log("error @GetEndInitPos");
				return false;
			}
		}
	}
	else if(origin_pos_style==3)
	{	//��һ�˽ڵ�λ��
		if(pStart!=NULL)
			init_pos = pStart->Position(true);
		else
			return false;
	}
	else if(origin_pos_style==4)
		init_pos = Start();	//��һ�����λ��
	else if(origin_pos_style==5&&pEnd)
	{	//�����ǿ���Ը�Ϊ����ʶ���
		if(pEnd->m_cPosCalType==CLDSNode::COORD_INTERSECTION)
			pEnd->GetIntersZhun(&init_pos);	//�����
		else if(pEnd->m_cPosCalType==CLDSNode::COORD_SCALE)
		{	//�����ȷֵ�
			bool inited=false;
			CLDSNode* pAttachNode1=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[0]);
			CLDSNode* pAttachNode2=BelongModel()->FromNodeHandle(pEnd->arrRelationNode[1]);
			if(pAttachNode1!=NULL&&pAttachNode2!=NULL)
			{
				f3dPoint lineVec,lineStdVec=pAttachNode2->Position()-pAttachNode1->Position();
				normalize(lineStdVec);
				//�����жϽڵ㸸�˼��Ƿ����Ϊ��׼�˼�
				CLDSLinePart* pStartDatumRod=NULL,*pEndDatumRod=NULL;
				CLDSLinePart* pRod=BelongModel()->FromRodHandle(pAttachNode1->hFatherPart);
				if(pRod&&pRod->pStart!=NULL&&pRod->pEnd!=NULL)
				{
					lineVec=pRod->pEnd->Position()-pRod->pStart->Position();
					if(lineVec.IsEqual((lineVec*lineStdVec)*lineStdVec))
						pStartDatumRod=pRod;	//�繲��(lineVec*lineStdVec)*lineStdVecӦ��lineVecһ��
				}
				pRod=BelongModel()->FromRodHandle(pAttachNode2->hFatherPart);
				if(pRod&&pRod->pStart!=NULL&&pRod->pEnd!=NULL)
				{
					lineVec=pRod->pEnd->Position()-pRod->pStart->Position();
					if(lineVec.IsEqual((lineVec*lineStdVec)*lineStdVec))
						pEndDatumRod=pRod;	//�繲��(lineVec*lineStdVec)*lineStdVecӦ��lineVecһ��
				}
				if(pStartDatumRod==NULL||pEndDatumRod==NULL)
				{
					CHandleReplacer filter(0,0);
					BOOL pushed=BelongModel()->PushPartStack();
					for(pRod=BelongModel()->EnumRodFirst();pRod;pRod=BelongModel()->EnumRodNext())
					{
						if( pRod->pStart==NULL||pRod->pEnd==NULL||
							pRod->handle==pAttachNode1->hFatherPart||pRod->handle==pAttachNode2->hFatherPart)
							continue;	//���ǺϷ��˼������������Ĵ���
						GEPOINT lineStart=pRod->pStart->Position();
						GEPOINT lineEnd=pRod->pEnd->Position();
						lineVec=lineEnd-lineStart;
						if(!lineVec.IsEqual((lineVec*lineStdVec)*lineStdVec))
							continue;	//�繲��(lineVec*lineStdVec)*lineStdVecӦ��lineVecһ��
						f3dPoint vertDistVec=f3dPoint(lineStart)-pAttachNode1->Position();
						if(!vertDistVec.IsEqual((vertDistVec*lineStdVec)*lineStdVec))
							continue;	//ʼ�˵㲻�ڵȷֶ�������
						filter.AppendSectRod(lineStart,lineEnd,pRod->handle);
					}
					if(pStartDatumRod==NULL)
					{
						if(filter.SelectPerfectPartHandle(pAttachNode1->Position())>0)
							pStartDatumRod=BelongModel()->FromRodHandle(filter.hNew);
					}
					if(pEndDatumRod==NULL)
					{
						if(filter.SelectPerfectPartHandle(pAttachNode2->Position())>0)
							pEndDatumRod=BelongModel()->FromRodHandle(filter.hNew);
					}
					BelongModel()->PopPartStack(pushed);
				}
				if(pStartDatumRod!=NULL&&pEndDatumRod!=NULL)
				{
					GEPOINT datumStart,datumEnd;
					if(pStartDatumRod->IsAngle())
					{
						if(desStartPos.cFaceOffsetDatumLine=='X')
							datumStart=((CLDSLineAngle*)pStartDatumRod)->GetDatumPosWingX(pAttachNode1);
						else if(desStartPos.cFaceOffsetDatumLine=='Y')
							datumStart=((CLDSLineAngle*)pStartDatumRod)->GetDatumPosWingY(pAttachNode1);
						else
							datumStart=((CLDSLineAngle*)pStartDatumRod)->GetDatumPosBer(pAttachNode1);
					}
					else
						pStartDatumRod->GetDatumPos(pAttachNode1);
					if(pEndDatumRod->IsAngle())
					{
						if(desStartPos.cFaceOffsetDatumLine=='X')
							datumEnd=((CLDSLineAngle*)pEndDatumRod)->GetDatumPosWingX(pAttachNode2);
						else if(desStartPos.cFaceOffsetDatumLine=='Y')
							datumEnd=((CLDSLineAngle*)pEndDatumRod)->GetDatumPosWingY(pAttachNode2);
						else
							datumEnd=((CLDSLineAngle*)pEndDatumRod)->GetDatumPosBer(pAttachNode2);
					}
					else
						pEndDatumRod->GetDatumPos(pAttachNode2);
					init_pos=datumStart+(datumEnd-datumStart)*pEnd->attach_scale;
					inited=true;
				}
			}
			if(!inited)
				init_pos = pEnd->Position(true);
		}
		//else if(pEnd->m_cPosCalType==CLDSNode::COORD_OFFSET)
		//{	//ƫ�Ƶ�
		//}
		else
			init_pos = pEnd->Position(true);
	}
	else
		return false;
	return true;
}
f3dPoint CLDSLineAngle::GetEndInitPos()
{
	f3dPoint init_pos;
	if(GetEndInitPos(init_pos))
		return init_pos;
	else
		return End();
}

f3dPoint CLDSLineAngle::GetStartDatumPos()
{
	f3dPoint datum_point;
	if(BelongModel()==NULL)
		return Start();
	if(desStartPos.jgber_cal_style==1)	//��ʾָ������
		return Start();
	else if(desStartPos.spatialOperationStyle==0)
	{
		desStartPos.datumPoint.UpdatePos(BelongModel());
		return desStartPos.datumPoint.Position();
	}
	else if(desStartPos.spatialOperationStyle==16)	//����ֱ��ͶӰ
		return Start();
	else if(desStartPos.spatialOperationStyle==17)	//����ֱ��ƽ��
	{
		Int3dlf(datum_point,f3dLine(Start(),End()),pStart->Position(true),desStartPos.face_offset_norm);
		return datum_point;
	}
	else if(desStartPos.spatialOperationStyle==18||desStartPos.spatialOperationStyle==19)
		return GetStartInitPos();
	else //if(desStartPos.jgber_cal_style==0)
	{
		CSuperSmartPtr<CLDSLinePart>pDatumRod;
		if(desStartPos.datum_jg_h>0x20)
			pDatumRod=(CLDSLinePart*)BelongModel()->FromPartHandle(desStartPos.datum_jg_h,CLS_LINEPART);
		if(pDatumRod.IsNULL())
			pDatumRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEPART);
		f3dPoint len_offset_vec;
		if(pDatumRod.IsHasPtr())
		{
			len_offset_vec=pDatumRod->End()-pDatumRod->Start();
			normalize(len_offset_vec);
			len_offset_vec*=desStartPos.len_offset_dist;
		}
		if(desStartPos.spatialOperationStyle==0)		//�ڵ㼴����
		{
			if(desStartPos.origin_pos_style==0)
				return pStart->Position(true);
			else if(desStartPos.origin_pos_style==1)	//ֱ��ָ����������
				return desStartPos.datumPoint.Position();
			else
				logto.Log("error");
		}
		if(pDatumRod.IsNULL())
			return f3dPoint(0,0,0);
		else if(pDatumRod->IsAngle())
		{
			CSmartPtr<CLDSLineAngle>pDatumJg=pDatumRod.LineAnglePointer();
			if(desStartPos.spatialOperationStyle==1||desStartPos.spatialOperationStyle==7)
				datum_point=pDatumJg->GetDatumPosBer(pStart)+len_offset_vec;
			else if(desStartPos.spatialOperationStyle==2||desStartPos.spatialOperationStyle==8||desStartPos.spatialOperationStyle==13)
				return pDatumJg->GetDatumPosWingX(pStart)+len_offset_vec;	//��X֫����ͶӰ
			else if(desStartPos.spatialOperationStyle==3||desStartPos.spatialOperationStyle==9||desStartPos.spatialOperationStyle==14)
				return pDatumJg->GetDatumPosWingY(pStart)+len_offset_vec;	//��Y֫����ͶӰ
			else if(desStartPos.spatialOperationStyle==4||desStartPos.spatialOperationStyle==10)//.offset_style==1)
			{	//��������ƽ��
				if(desStartPos.origin_pos_style>0)//bByOtherEndOffset)
				{
					logto.Log("error");
					f3dPoint pt = GetEndDatumPos();
					datum_point = pDatumJg->GetDatumPosBerByFaceOffset(pStart,desStartPos.face_offset_norm,&pt)+len_offset_vec;
				}
				else
					datum_point = pDatumJg->GetDatumPosBerByFaceOffset(pStart,desStartPos.face_offset_norm)+len_offset_vec;
			}
			else if(desStartPos.spatialOperationStyle==5||desStartPos.spatialOperationStyle==11)		//X֫
			{	//����X֫����ƽ��
				if(desStartPos.origin_pos_style>0)//bByOtherEndOffset)
				{
					logto.Log("error");
					f3dPoint pt = GetEndDatumPos();
					return pDatumJg->GetDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm,&pt)+len_offset_vec;
				}
				else
					return pDatumJg->GetDatumPosWingXByFaceOffset(pStart,desStartPos.face_offset_norm)+len_offset_vec;
			}
			else if(desStartPos.spatialOperationStyle==6||desStartPos.spatialOperationStyle==12)
			{	//����Y֫����ƽ��
				if(desStartPos.origin_pos_style>0)//bByOtherEndOffset)
				{
					logto.Log("error");
					f3dPoint pt = GetEndDatumPos();
					return pDatumJg->GetDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm,&pt)+len_offset_vec;
				}
				else
					return pDatumJg->GetDatumPosWingYByFaceOffset(pStart,desStartPos.face_offset_norm)+len_offset_vec;
			}
			if((desStartPos.spatialOperationStyle==1||desStartPos.spatialOperationStyle==4)&&
				pDatumJg.IsHasPtr()&&pDatumJg->IsReservedHeJiao())
			{	//����ͶӰ���߻�ƽ������ʱ���б���֫��ȵ�����Ͻ���Ҫ���⴦��
				f3dPoint vec=End()-Start();
				f3dPoint work_norm=vec^len_offset_vec;
				normalize(work_norm);
				if(fabs(work_norm*pDatumJg->get_norm_x_wing())>fabs(work_norm*pDatumJg->get_norm_y_wing()))
					datum_point+=CalWingOffsetVec('X',0);
				else
					datum_point+=CalWingOffsetVec('Y',0);
			}
		}
		else if(pDatumRod->GetClassTypeId()==CLS_LINETUBE)
			SnapPerp(&datum_point,pDatumRod->Start(),pDatumRod->End(),pStart->Position(true));
		return datum_point;		//��������ͶӰ
	}
}
f3dPoint CLDSLineAngle::GetEndDatumPos()
{
	f3dPoint datum_point;
	if(BelongModel()==NULL)
		return End();
	if(desEndPos.jgber_cal_style==1)
	{	//��ʾָ�����꣬ȥ��ԭ������ʱҲֱ�ӷ��ص��趨����Ϊ�Ȼ�������ƶ���˨ʱ�����⣩ WJH-2004.11.19
		return End();
	}
	else if(desEndPos.spatialOperationStyle==0)
	{
		desEndPos.datumPoint.UpdatePos(BelongModel());
		return desEndPos.datumPoint.Position();
	}
	else if(desEndPos.spatialOperationStyle==18||desEndPos.spatialOperationStyle==19)
		return GetEndInitPos();
	else if(desEndPos.spatialOperationStyle==16)	//����ֱ��ͶӰ
		return End();
	else if(desEndPos.spatialOperationStyle==17)	//����ֱ��ƽ��
	{
		Int3dlf(datum_point,f3dLine(Start(),End()),pEnd->Position(true),desEndPos.face_offset_norm);
		return datum_point;
	}
	else //if(desEndPos.jgber_cal_style==0)
	{
		CSuperSmartPtr<CLDSLinePart>pDatumRod;
		if(desEndPos.datum_jg_h>0x20)
			pDatumRod=(CLDSLinePart*)BelongModel()->FromPartHandle(desEndPos.datum_jg_h,CLS_LINEPART);
		if(pDatumRod.IsNULL())
			pDatumRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEPART);
		f3dPoint len_offset_vec;
		if(pDatumRod.IsHasPtr())
		{
			len_offset_vec=pDatumRod->End()-pDatumRod->Start();
			normalize(len_offset_vec);
			len_offset_vec*=desEndPos.len_offset_dist;
		}
		if(desEndPos.spatialOperationStyle==0)
		{
			if(desEndPos.origin_pos_style==0)
				return pEnd->Position(true);	//�ڵ㼴����
			else if(desEndPos.origin_pos_style==1)	//ֱ��ָ����������
				return desEndPos.datumPoint.Position();
			else
				logto.Log("error");
		}
		else if(pDatumRod.IsNULL())
			return f3dPoint(0,0,0);
		else if(pDatumRod->IsAngle())
		{
			CSmartPtr<CLDSLineAngle>pDatumJg=pDatumRod.LineAnglePointer();
			if(desEndPos.spatialOperationStyle==1||desEndPos.spatialOperationStyle==7)
				datum_point = pDatumJg->GetDatumPosBer(pEnd)+len_offset_vec;		//������ͶӰ
			else if(desEndPos.spatialOperationStyle==2||desEndPos.spatialOperationStyle==8||desEndPos.spatialOperationStyle==13)
				return pDatumJg->GetDatumPosWingX(pEnd)+len_offset_vec;	//����X֫����ͶӰ
			else if(desEndPos.spatialOperationStyle==3||desEndPos.spatialOperationStyle==9||desEndPos.spatialOperationStyle==14)
				return pDatumJg->GetDatumPosWingY(pEnd)+len_offset_vec;	//����Y֫����ͶӰ
			else if(desEndPos.spatialOperationStyle==4||desEndPos.spatialOperationStyle==10)
			{	//��������ƽ��
				if(desEndPos.origin_pos_style>0)//.bByOtherEndOffset)
				{
					logto.Log("error");
					f3dPoint pt = GetStartDatumPos();
					datum_point = pDatumJg->GetDatumPosBerByFaceOffset(pEnd,desEndPos.face_offset_norm,&pt)+len_offset_vec;
				}
				else
					datum_point = pDatumJg->GetDatumPosBerByFaceOffset(pEnd,desEndPos.face_offset_norm)+len_offset_vec;
			}
			else if(desEndPos.spatialOperationStyle==5||desEndPos.spatialOperationStyle==11)
			{	//����X֫����ƽ��
				if(desEndPos.origin_pos_style>0)//bByOtherEndOffset)
				{
					logto.Log("error");
					f3dPoint pt = GetStartDatumPos();
					return pDatumJg->GetDatumPosWingXByFaceOffset(pEnd,desEndPos.face_offset_norm,&pt)+len_offset_vec;
				}
				else
					return pDatumJg->GetDatumPosWingXByFaceOffset(pEnd,desEndPos.face_offset_norm)+len_offset_vec;
			}
			else if(desEndPos.spatialOperationStyle==6||desEndPos.spatialOperationStyle==12)
			{	//����Y֫����ƽ��
				if(desEndPos.origin_pos_style>0)//bByOtherEndOffset==1)
				{
					logto.Log("error");
					f3dPoint pt = GetStartDatumPos();
					return pDatumJg->GetDatumPosWingYByFaceOffset(pEnd,desEndPos.face_offset_norm,&pt)+len_offset_vec;
				}
				else
					return pDatumJg->GetDatumPosWingYByFaceOffset(pEnd,desEndPos.face_offset_norm)+len_offset_vec;
			}
			if((desEndPos.spatialOperationStyle==1||desEndPos.spatialOperationStyle==4)&&
				pDatumJg.IsHasPtr()&&pDatumJg->IsReservedHeJiao())
			{	//�б���֫��ȵ�����Ͻ�
				f3dPoint vec=End()-Start();
				f3dPoint work_norm=vec^len_offset_vec;
				normalize(work_norm);
				if(fabs(work_norm*pDatumJg->get_norm_x_wing())>fabs(work_norm*pDatumJg->get_norm_y_wing()))
					datum_point+=CalWingOffsetVec('X',0);
				else
					datum_point+=CalWingOffsetVec('Y',0);
			}
		}
		else if(pDatumRod->GetClassTypeId()==CLS_LINETUBE)
			SnapPerp(&datum_point,pDatumRod->Start(),pDatumRod->End(),pEnd->Position(true));
		return datum_point;
	}
}
f3dPoint CLDSLineAngle::GetEndDatumPosEx()	//����ǻ�����Ϊ��׼���򷵻ػ�����λ��
{
	//try{
		if(desEndPos.spatialOperationStyle<=6||desEndPos.spatialOperationStyle>=15||pEnd==NULL)
			return GetEndDatumPos();
		else// if(pEnd->hFatherPart!=handle)
		{	//��ӽǸ�
			f3dPoint end_ber,end_datum;
			end_ber=End();
			f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
			//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
			f3dPoint datum_vec,cur_vec,work_norm;
			CSmartPtr<CLDSLineAngle>pDatumJg;
			if(desEndPos.datum_jg_h>0x20)
				pDatumJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(desEndPos.datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumJg.IsNULL())
				pDatumJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumJg.IsNULL())
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Both the datum angle which angle's end positions and the father line part of end node are 0x0��";
#else
				throw "�Ǹ��ն˶�λ��׼�Ǹּ��ն˽ڵ㸸�˼���Ϊ0x0��";
#endif
			//��������ֱ�Ӹ�Ϊ�Ǹ���㣬��û�в���ֻ�Ǿ�����ǰ�������е���� WJH-2005.03.08
			datum_vec = pDatumJg->End()-pDatumJg->Start();
			cur_vec = End()-Start();
			//datum_vec = pDatumJg->pEnd->GetInverseNodePos()-pDatumJg->pStart->GetInverseNodePos();
			//cur_vec = pEnd->GetInverseNodePos()-pStart->GetInverseNodePos();
			normalize(datum_vec);
			normalize(cur_vec);
			if(fabs(datum_vec*cur_vec)>=0.9)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
			{
				if(desEndPos.IsInDatumJgWingX())
					work_norm=pDatumJg->get_norm_x_wing();
				else if(desEndPos.IsInDatumJgWingY())
					work_norm=pDatumJg->get_norm_y_wing();
				else
					work_norm=datum_vec^cur_vec;
			}
			else
				work_norm = datum_vec^cur_vec;
			normalize(work_norm);
			if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
			{
				offset_vec_x = get_norm_x_wing();
				offset_vec_y = -GetWingVecX();
			}
			else
			{
				offset_vec_x = -GetWingVecY();
				offset_vec_y = get_norm_y_wing();
			}
		
			end_datum = end_ber-offset_vec_x*GetEndOffsetByWingX();
			end_datum = end_datum-offset_vec_y*GetEndOffsetByWingY();
			return end_datum;
		}
	/*}
	catch(CPtrException &e)
	{
		AfxMessageBox(e.sError);
		return f3dPoint(0,0,0);
	}*/
}
f3dPoint CLDSLineAngle::GetStartDatumPosEx()
{
	//try{
	if(desStartPos.spatialOperationStyle<=6||desStartPos.spatialOperationStyle>=15||pStart==NULL)
			return GetStartDatumPos();
		else// if(pStart->hFatherPart!=handle)
		{	//��ӽǸ�
			f3dPoint start_ber,start_datum;
			start_ber=Start();
			f3dPoint offset_vec_x,offset_vec_y;	//�ֱ��ʾ��X/Y֫���ߡ�ƫ�Ʒ���
			//�ص�ǰ����֫�ķ��߼�֫�������ƫ��
			f3dPoint datum_vec,cur_vec,work_norm;;
			CSmartPtr<CLDSLineAngle>pDatumJg;
			if(desStartPos.datum_jg_h>0x20)
				pDatumJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(desStartPos.datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumJg.IsNULL())
				pDatumJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pDatumJg.IsNULL())
#ifdef AFX_TARG_ENU_ENGLISH
				throw "Both the datum angle which angle's end positions and the father line part of start node are 0x0��";
#else
				throw "�Ǹ�ʼ�˶�λ��׼�Ǹּ�ʼ�˽ڵ㸸�˼���Ϊ0x0��";
#endif
			datum_vec = pDatumJg->pEnd->GetInverseNodePos()-pDatumJg->pStart->GetInverseNodePos();
			cur_vec = pEnd->GetInverseNodePos()-pStart->GetInverseNodePos();
			normalize(datum_vec);
			normalize(cur_vec);
			if(fabs(datum_vec*cur_vec)>=0.9)	//�ӽ��ڹ��ߣ������Դ�Ϊ�о�
			{
				if(desStartPos.IsInDatumJgWingX())
					work_norm=pDatumJg->get_norm_x_wing();
				else if(desStartPos.IsInDatumJgWingY())
					work_norm=pDatumJg->get_norm_y_wing();
				else
					work_norm=datum_vec^cur_vec;
			}
			else
				work_norm = datum_vec^cur_vec;
			normalize(work_norm);
			if(fabs(work_norm*get_norm_x_wing())>fabs(work_norm*get_norm_y_wing()))
			{
				offset_vec_x = get_norm_x_wing();
				offset_vec_y = -GetWingVecX();
			}
			else
			{
				offset_vec_x = -GetWingVecY();
				offset_vec_y = get_norm_y_wing();
			}
		
			start_datum = start_ber-offset_vec_x*GetStartOffsetByWingX();
			start_datum = start_datum-offset_vec_y*GetStartOffsetByWingY();
			return start_datum;
		}
	/*}
	catch(CPtrException &e)
	{
		AfxMessageBox(e.sError);
		return f3dPoint(0,0,0);
	}*/
}

/*
����ֵ:
 0��ʾ�Ϸ�;
-1��˨�˾��С;
-2��˨���Ʊ߾��С;
-3��˨�нǱ߾�̫С 
*/
int CLDSLineAngle::ValidLsList(char sError[200],
							 BOOL bRollingSpace/*=TRUE*/,BOOL bCutAngleSpace/*=TRUE*/,
							 BOOL bLsEndSpace/*=TRUE*/,double epsilon/*=2*/)
{
	UCS_STRU ucs;
	getUCS(ucs);
	f3dPoint pos,norm;
	LSSPACE_STRU LsSpace;
	//try{
		double x_wing_wide=GetWidth();
		double y_wing_wide=GetWidthWingY();
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			pos = (*pLsRef)->ucs.origin;
			norm= (*pLsRef)->get_norm();
			GetLsSpace(LsSpace,(*pLsRef)->get_d());
			if(LsSpace.RollingEdgeSpace>0.5*GetWidth())
				LsSpace.RollingEdgeSpace=ftoi(0.5*GetWidth());
			//���ﻹû�д����Ͻǵ����⣬�д��Ľ� WJH-2003.12.30
			if(fabs(norm_y_wing*norm)<fabs(norm_x_wing*norm))
			{	//��˨λ��X֫��
				Int3dlf(pos,pos,norm,ucs.origin,norm_x_wing);
				coord_trans(pos,ucs,FALSE);
				double g=GetLsG(pLsRef);
				if(bLsEndSpace&&pos.z<-start_oddment+LsSpace.EndSpace-epsilon && !bHuoQuStart)	//�˾಻����
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on start of 0X%X anlge {%s} don't meet the requirement of end space range",handle,(char*)sPartNo);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}��ʼ����˨������˾෶ΧҪ��",handle,(char*)sPartNo);
#endif
					return -1;
				}
				else if(bLsEndSpace&&pos.z>DISTANCE(Start(),End())+end_oddment-LsSpace.EndSpace+epsilon && !bHuoQuEnd)	//�˾಻����
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on end of 0X%X angle {%s} don't meet the requirement of end space range",handle,(char*)sPartNo);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}���ն���˨������˾෶ΧҪ��",handle,(char*)sPartNo);
#endif
					return -1;
				}
				else if(bRollingSpace&&(g<LsSpace.RollingEdgeSpace-epsilon||g>x_wing_wide-LsSpace.RollingEdgeSpace+epsilon))
				{	//����g<size.thick�ǲ��Ե�,�����߾���<���Ʊ߾�Ҳ�����ǲ������ wjh-2017.11.24
					double fRealRollingEdgeSpace=min(g,x_wing_wide-g);
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on X leg of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of rolling space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.RollingEdgeSpace,fRealRollingEdgeSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫����˨���������Ʊ߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.RollingEdgeSpace,fRealRollingEdgeSpace);
#endif
					return -2;
				}
				if(bCutAngleSpace&&(cut_angle[0][0]>0&&cut_angle[0][1]>0)&&DistOf3dPtLine(pos,
					f3dPoint(x_wing_wide-cut_angle[0][1],0,-start_oddment),
					f3dPoint(x_wing_wide,0,-start_oddment+cut_angle[0][0]))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(x_wing_wide-cut_angle[0][1],0,-start_oddment),f3dPoint(x_wing_wide,0,-start_oddment+cut_angle[0][0]));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on X leg's start of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫��ʼ����˨�������нǱ߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&(cut_angle[2][0]>0&&cut_angle[2][1]>0)&&DistOf3dPtLine(pos,
					f3dPoint(x_wing_wide-cut_angle[2][1],0,DISTANCE(Start(),End())+end_oddment),
					f3dPoint(x_wing_wide,0,DISTANCE(Start(),End())+end_oddment-cut_angle[2][0]))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(x_wing_wide-cut_angle[2][1],0,DISTANCE(Start(),End())+end_oddment),f3dPoint(x_wing_wide,0,DISTANCE(Start(),End())+end_oddment-cut_angle[2][0]));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on X leg's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫���ն���˨�������нǱ߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&cut_wing[0]==1&&	//X֫
					DistOf3dPtLine(pos,
					f3dPoint(0,0,-start_oddment+cut_wing_para[0][0]),
					f3dPoint(x_wing_wide,0,-start_oddment+cut_wing_para[0][0]+cut_wing_para[0][2]))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,0,-start_oddment+cut_wing_para[0][0]),
						f3dPoint(x_wing_wide,0,-start_oddment+cut_wing_para[0][0]+cut_wing_para[0][2]));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on X leg's start of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫��ʼ����˨��������֫�߾�Ҫ��(��׼:%d,ʵ��%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&cut_wing[0]==2&&	//Y֫
					DistOf3dPtLine(pos,
					f3dPoint(0,0,-start_oddment+cut_wing_para[0][0]),
					f3dPoint(cut_wing_para[0][1],0,-start_oddment))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,0,-start_oddment+cut_wing_para[0][0]),f3dPoint(cut_wing_para[0][1],0,-start_oddment));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on Y start's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,size_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫��ʼ����˨��������֫�߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,size_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&cut_wing[1]==1&&	//X֫
					DistOf3dPtLine(pos,
					f3dPoint(0,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]),
					f3dPoint(size_wide,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]-cut_wing_para[1][2]))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]),
						f3dPoint(size_wide,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]-cut_wing_para[1][2]));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on X leg's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,size_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫���ն���˨��������֫�߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,size_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&cut_wing[1]==2&&	//Y֫
					DistOf3dPtLine(pos,
					f3dPoint(0,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]),
					f3dPoint(cut_wing_para[1][1],0,DISTANCE(Start(),End())+end_oddment))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]),
						f3dPoint(cut_wing_para[1][1],0,DISTANCE(Start(),End())+end_oddment));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on Y leg's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫���ն���˨��������֫�߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
			}
			else
			{	//��˨λ��Y֫��
				Int3dlf(pos,pos,norm,ucs.origin,norm_y_wing);
				coord_trans(pos,ucs,FALSE);
				double g=GetLsG(pLsRef);
				if(bLsEndSpace&&pos.z<-start_oddment+LsSpace.EndSpace-epsilon && !bHuoQuStart)	//�˾಻����
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on start of 0X%X angle {%s} don't meet the requirement of end space range",handle,(char*)sPartNo);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}��ʼ����˨������˾෶ΧҪ��",handle,(char*)sPartNo);
#endif
					return -1;
				}
				else if(bLsEndSpace&&pos.z>DISTANCE(Start(),End())+end_oddment-LsSpace.EndSpace+epsilon && !bHuoQuEnd)	//�˾಻����
				{
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on end of 0X%X angle {%s} don't meet the requirement of end space range",handle,(char*)sPartNo);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}���ն���˨������˾෶ΧҪ��",handle,(char*)sPartNo);
#endif
					return -1;
				}
				else if(bRollingSpace&&(g<LsSpace.RollingEdgeSpace-epsilon||g>y_wing_wide-LsSpace.RollingEdgeSpace+epsilon))
				{	//����g<size.thick�ǲ��Ե�,�����߾���<���Ʊ߾�Ҳ�����ǲ������ wjh-2017.11.24
					double fRealRollingEdgeSpace=min(g,y_wing_wide-g);
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on Y leg of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of rolling space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,y_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.RollingEdgeSpace,fRealRollingEdgeSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫����˨���������Ʊ߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,y_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.RollingEdgeSpace,fRealRollingEdgeSpace);
#endif
					return -2;
				}
				if((bCutAngleSpace&&cut_angle[1][0]>0&&cut_angle[1][1]>0)&&DistOf3dPtLine(pos,
					f3dPoint(0,x_wing_wide-cut_angle[1][1],-start_oddment),
					f3dPoint(0,x_wing_wide,-start_oddment+cut_angle[1][0]))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAnbleSpace=DistOf3dPtLine(pos,f3dPoint(0,x_wing_wide-cut_angle[1][1],-start_oddment),f3dPoint(0,x_wing_wide,-start_oddment+cut_angle[1][0]));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on Y leg's start of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAnbleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫��ʼ����˨�������нǱ߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAnbleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&(cut_angle[3][0]>0&&cut_angle[3][1]>0)&&DistOf3dPtLine(pos,
					f3dPoint(0,x_wing_wide-cut_angle[3][1],DISTANCE(Start(),End())+end_oddment),
					f3dPoint(0,x_wing_wide,DISTANCE(Start(),End())+end_oddment-cut_angle[3][0]))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,x_wing_wide-cut_angle[3][1],DISTANCE(Start(),End())+end_oddment),
						f3dPoint(0,x_wing_wide,DISTANCE(Start(),End())+end_oddment-cut_angle[3][0]));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on Y leg's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫���ն���˨�������нǱ߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&cut_wing[0]==2&&	//Y֫
					DistOf3dPtLine(pos,
					f3dPoint(0,0,-start_oddment+cut_wing_para[0][0]),
					f3dPoint(0,x_wing_wide,-start_oddment+cut_wing_para[0][0]+cut_wing_para[0][2]))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,0,-start_oddment+cut_wing_para[0][0]),
						f3dPoint(0,x_wing_wide,-start_oddment+cut_wing_para[0][0]+cut_wing_para[0][2]));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on Y leg's start of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫��ʼ����˨��������֫�߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&cut_wing[0]==1&&	//X֫
					DistOf3dPtLine(pos,
					f3dPoint(0,0,-start_oddment+cut_wing_para[0][0]),
					f3dPoint(0,cut_wing_para[0][1],-start_oddment))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,0,-start_oddment+cut_wing_para[0][0]),f3dPoint(0,cut_wing_para[0][1],-start_oddment));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on X leg's start of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫��ʼ����˨��������֫�߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&cut_wing[1]==2&&	//Y֫
					DistOf3dPtLine(pos,
					f3dPoint(0,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]),
					f3dPoint(0,x_wing_wide,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]-cut_wing_para[1][2]))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]),
						f3dPoint(0,x_wing_wide,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]-cut_wing_para[1][2]));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on Y leg's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫���ն���˨��������֫�߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
				else if(bCutAngleSpace&&cut_wing[1]==1&&	//X֫
					DistOf3dPtLine(pos,
					f3dPoint(0,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]),
					f3dPoint(0,cut_wing_para[1][1],DISTANCE(Start(),End())+end_oddment))<LsSpace.CutAngleSpace-epsilon)
				{
					double fRealCutAngleSpace=DistOf3dPtLine(pos,f3dPoint(0,0,DISTANCE(Start(),End())+end_oddment-cut_wing_para[1][0]),
						f3dPoint(0,cut_wing_para[1][1],DISTANCE(Start(),End())+end_oddment));
#ifdef AFX_TARG_ENU_ENGLISH
					sprintf(sError,"Bolts on X leg's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) don't meet the requirement of cut leg space(standard:%d,actual:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#else
					sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫���ն���˨��������֫�߾�Ҫ��(��׼:%d,ʵ��:%0.1f)",
						handle,(char*)sPartNo,x_wing_wide,size_thick,(*pLsRef)->get_d(),LsSpace.CutAngleSpace,fRealCutAngleSpace);
#endif
					return -3;
				}
			}
		}
		return 0;
	/*}
	catch(CPtrException &e)
	{
		AfxMessageBox(e.sError);
		return FALSE;
	}*/
}
BOOL CLDSLineAngle::ValidLsPushFlat(char sError[MAX_TEMP_CHAR_200+1],double epsilon/*=2*/)
{
	UCS_STRU ucs;
	getUCS(ucs);
	f3dPoint pos,norm;
	LSSPACE_STRU LsSpace;
	double length=DISTANCE(Start(),End());
	double x_wing_wide=GetWidth();
	double y_wing_wide=GetWidthWingY();
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
	{
		pos = (*pLsRef)->ucs.origin;
		norm= (*pLsRef)->get_norm();
		GetLsSpace(LsSpace,(*pLsRef)->get_d());
		if(LsSpace.RollingEdgeSpace>0.5*GetWidth())
			LsSpace.RollingEdgeSpace=ftoi(0.5*GetWidth());
		//���ﻹû�д����Ͻǵ����⣬�д��Ľ� WJH-2003.12.30
		if(fabs(norm_y_wing*norm)<fabs(norm_x_wing*norm))
		{	//��˨λ��X֫��
			Int3dlf(pos,pos,norm,ucs.origin,norm_x_wing);
			coord_trans(pos,ucs,FALSE);
			if(start_push_flat_x1_y2==1&&(pos.z<start_push_flat_length+epsilon-start_oddment))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"The bolt hole on X leg's start of 0X%X angle {%s}(L%.0fX%.0f:M%d) is conflict with pushing flat",handle,(char*)sPartNo,x_wing_wide,size.thick,(*pLsRef)->get_d());
#else
				sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫��ʼ����˨��λ��ѹ���г�ͻ",handle,(char*)sPartNo,x_wing_wide,size.thick,(*pLsRef)->get_d());
#endif
				return FALSE;
			}
			if(end_push_flat_x1_y2==1&&(pos.z>length+end_oddment-end_push_flat_length-epsilon))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"The bolt hole on X leg's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) is conflict with pushing flat",handle,(char*)sPartNo,x_wing_wide,size.thick,(*pLsRef)->get_d());
#else
				sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)X֫���ն���˨��λ��ѹ���г�ͻ",handle,(char*)sPartNo,x_wing_wide,size.thick,(*pLsRef)->get_d());
#endif
				return FALSE;
			}
		}
		else
		{	//��˨λ��Y֫��
			Int3dlf(pos,pos,norm,ucs.origin,norm_y_wing);
			coord_trans(pos,ucs,FALSE);
			if(start_push_flat_x1_y2==2&&(pos.z<start_push_flat_length+epsilon-start_oddment))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"The bolt hole on Y leg's start of 0X%X angle {%s}(L%.0fX%.0f:M%d) is conflict with pushing flat",handle,(char*)sPartNo,x_wing_wide,size.thick,(*pLsRef)->get_d());
#else
				sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫��ʼ����˨��λ��ѹ���г�ͻ",handle,(char*)sPartNo,x_wing_wide,size.thick,(*pLsRef)->get_d());
#endif
				return FALSE;
			}
			if(end_push_flat_x1_y2==2&&(pos.z>length+end_oddment-end_push_flat_length-epsilon))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"The bolt hole on Y leg's end of 0X%X angle {%s}(L%.0fX%.0f:M%d) is conflict with pushing flat",handle,(char*)sPartNo,x_wing_wide,size.thick,(*pLsRef)->get_d());
#else
				sprintf(sError,"0X%X�Ǹ�{%s}(L%.0fX%.0f:M%d)Y֫���ն���˨��λ��ѹ���г�ͻ",handle,(char*)sPartNo,x_wing_wide,size.thick,(*pLsRef)->get_d());
#endif
				return FALSE;
			}
		}
	}
	return TRUE;
}

void CLDSLineAngle::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	long h,method=0;
	buffer.ReadInteger(&h);
	pStart=BelongModel()->FromNodeHandle(h);
	buffer.ReadInteger(&h);
	pEnd=BelongModel()->FromNodeHandle(h);
	buffer.ReadString(layer_tag,4);
	buffer.ReadInteger(&_hPartWeldParent);
	pLsRefList->FromBuffer(buffer,m_pModel,version,doc_type);

	if( version==0||
		(doc_type==1&&version>=4020005)||	//TMA(V4.2.0.5)
		(doc_type==2&&version>=1020005)||	//LMA(V1.2.0.5)
		(doc_type==3&&version>=2000005)||	//TSA(V2.0.0.5)
		(doc_type==4&&version>=1020005)||	//LDS(V1.2.0.5)
		(doc_type==5&&version>=1020005))	//TDA(V1.2.0.5)
	{
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		buffer.ReadDword(&dwPermission);
		buffer.ReadDword(&dwStartPermission);
		buffer.ReadDword(&dwEndPermission);
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadString(sNotes,51);
		if(version==0||
			(doc_type==1&&version>=5000010)||
			(doc_type==2&&version>=2000005)||
			(doc_type==4&&version>=1030005)||
			(doc_type==5&&version>=1030005))
		{
			buffer.ReadBooleanThin(&_bSyncShadowPartNo);
			if(!_bSyncShadowPartNo)
			{
				buffer.ReadInteger(&_iSeg.iSeg);
				buffer.ReadString(sPartNo,16);
			}
		}
		if (version==0||(
			(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
			(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
			(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
			(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
			(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
		{
			buffer.ReadBooleanThin(&_blOddByBoltCanAcrossMiddleSect);
			buffer.ReadBooleanThin(&_blScatterBoltLocaTolerance);
		}
		else
			_blOddByBoltCanAcrossMiddleSect=_blScatterBoltLocaTolerance=false;
		buffer.ReadDouble(&norm_x_wing.x);
		buffer.ReadDouble(&norm_x_wing.y);
		buffer.ReadDouble(&norm_x_wing.z);
		buffer.ReadDouble(&norm_y_wing.x);
		buffer.ReadDouble(&norm_y_wing.y);
		buffer.ReadDouble(&norm_y_wing.z);
		buffer.ReadDouble(&nominal_length);
		buffer.ReadByte(&m_cHeJiaoReservedWidthType);
		buffer.ReadDouble(&m_fHeJiaoReservedWidth);
		buffer.ReadInteger(&kaihe_base_wing_x0_y1);
		buffer.ReadInteger(&m_bEnableTeG);
		buffer.ReadInteger(&xWingXZhunJu.g);
		buffer.ReadInteger(&xWingXZhunJu.g1);
		buffer.ReadInteger(&xWingXZhunJu.g2);
		buffer.ReadInteger(&xWingXZhunJu.g3);
		buffer.ReadInteger(&xWingYZhunJu.g);
		buffer.ReadInteger(&xWingYZhunJu.g1);
		buffer.ReadInteger(&xWingYZhunJu.g2);
		buffer.ReadInteger(&xWingYZhunJu.g3);
		if( version==0||
			(doc_type==1&&version>=4010100)||	//TMA(V4.1.1.0)
			(doc_type==2&&version>=1010100)||	//LMA(V1.1.1.0)
			(doc_type==3&&version>=2000100)||	//TSA(V2.0.1.0)
			(doc_type==4&&version>=1020100)||	//LDS(V1.2.1.0)
			(doc_type==5&&version>=1020100))	//TDA(V1.2.1.0)
			buffer.ReadDouble(&_fCutAngleCoefR);	//Ӧ�ü���,����������Ժ�汾�ѷ����ݲ�֪��δ���
		else
			_fCutAngleCoefR=0;
		
		if( version==0||
			(doc_type==1&&version>=4010102)||	//TMA(V4.1.1.2)
			(doc_type==2&&version>=1010102&&version!=1020009)||	//LMA(V1.1.1.2)V1.2.0.9���ڸ������Ҫ���ڵ����汾
			(doc_type==3&&version>=2000100)||	//TSA(V2.0.1.0)
			(doc_type==4&&version>=1020100)||	//TDA(V1.2.1.1)
			(doc_type==5&&version>=1020100))	//TMA(V1.2.1.1)
			buffer.ReadWord(&_nCutAngleSpaceOdd);	//�Ǹ��н�ʱ����ȥ����Բ���뾶ϵ��֮�����ͷ��϶
		else
			_nCutAngleSpaceOdd=0;
		
		buffer.ReadInteger(&group_father_jg_h);
		buffer.ReadInteger(&m_bUserSpecStartCutAngle);
		buffer.ReadInteger(&m_bUserSpecEndCutAngle);
		buffer.Read(cut_wing_para,6*sizeof(double));
		buffer.ReadInteger(&cut_wing[0]);
		buffer.ReadInteger(&cut_wing[1]);
		buffer.Read(cut_angle,8*sizeof(double));
		f3dPoint start,end;
		buffer.ReadDouble(&start.x);
		buffer.ReadDouble(&start.y);
		buffer.ReadDouble(&start.z);
		buffer.ReadDouble(&end.x);	
		buffer.ReadDouble(&end.y);	
		buffer.ReadDouble(&end.z);	
		baseline.SetStart(start);
		baseline.SetEnd(end);
		m_xStartBeforeLift=start;	
		m_xEndBeforeLift  =end;		
#ifdef __COMMON_PART_INC_
		buffer.ReadInteger(&m_hArcLift);
		buffer.ReadPoint(m_xStartBeforeLift);
		buffer.ReadPoint(m_xEndBeforeLift);
#endif
		//ʼ����ͷ�������
		buffer.ReadDouble(&start_oddment);
		buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desStartOdd.m_hRefPart1);
		buffer.ReadInteger(&desStartOdd.m_hRefPart2);
		buffer.ReadDouble(&desStartOdd.m_fCollideDist);
		//�洢�Ǹ�ʼ�˶���(�̶�)����ͷ
		buffer.ReadByte(&extraStartOdd.ber.style);
		if(extraStartOdd.ber.style==0)
			buffer.ReadDouble(&extraStartOdd.ber.odd);
		else
			extraStartOdd.ber.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		buffer.ReadByte(&extraStartOdd.wing_x.style);
		if(extraStartOdd.wing_x.style==0)
			buffer.ReadDouble(&extraStartOdd.wing_x.odd);
		else
			extraStartOdd.wing_x.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		buffer.ReadByte(&extraStartOdd.wing_y.style);
		if(extraStartOdd.wing_y.style==0)
			buffer.ReadDouble(&extraStartOdd.wing_y.odd);
		else
			extraStartOdd.wing_y.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		//�ն���ͷ�������
		buffer.ReadDouble(&end_oddment);   
		buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desEndOdd.m_hRefPart1);
		buffer.ReadInteger(&desEndOdd.m_hRefPart2);
		buffer.ReadDouble(&desEndOdd.m_fCollideDist);
		//�洢�Ǹ��ն˶���(�̶�)����ͷ
		buffer.ReadByte(&extraEndOdd.ber.style);
		if(extraEndOdd.ber.style==0)
			buffer.ReadDouble(&extraEndOdd.ber.odd);
		else
			extraEndOdd.ber.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		buffer.ReadByte(&extraEndOdd.wing_x.style);
		if(extraEndOdd.wing_x.style==0)
			buffer.ReadDouble(&extraEndOdd.wing_x.odd);
		else
			extraEndOdd.wing_x.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		buffer.ReadByte(&extraEndOdd.wing_y.style);
		if(extraEndOdd.wing_y.style==0)
			buffer.ReadDouble(&extraEndOdd.wing_y.odd);
		else
			extraEndOdd.wing_y.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		connectStart.FromBuffer(buffer,version,doc_type);
		connectEnd.FromBuffer(buffer,version,doc_type);
		buffer.ReadByte(&m_cPosCtrlDatumLineType);
		buffer.ReadByte(&m_cPosCtrlType);
		buffer.ReadInteger(&m_bLockStartPos);
#ifdef __PART_DESIGN_INC_
		if(doc_type==3||doc_type==4)
		{
			buffer.ReadInteger(&m_bAutoCalHoleOutCoef);	
			buffer.ReadDouble(&hole_out);			
			buffer.ReadInteger(&start_force_type);
			buffer.ReadInteger(&end_force_type);
			buffer.ReadInteger(&start_joint_type);
			buffer.ReadInteger(&end_joint_type);
			//buffer.ReadInteger(&CalLenCoef.iSelRFlag);	//(������ʵ������)
			buffer.ReadInteger(&CalLenCoef.iTypeNo);
			buffer.ReadInteger(&CalLenCoef.hHorizPole);
			buffer.ReadInteger(&CalLenCoef.hRefPole);
			buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
			buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
			buffer.ReadDouble(&CalLenCoef.minR.coef);
			buffer.ReadInteger(&CalLenCoef.paraR.hStartNode);
			buffer.ReadInteger(&CalLenCoef.paraR.hEndNode);
			buffer.ReadDouble(&CalLenCoef.paraR.coef);
			buffer.ReadWord(&m_iElemType);			
			//buffer.ReadDword(m_uStatMatNo);		//ͳ�����(��ĸ������������)
			//buffer.ReadDouble(bear_load_coef);	//������֧����֧�Ÿ˼�������(������ʵ������)
			buffer.ReadDouble(&m_fWeightRaiseCoef);	//�����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
			buffer.ReadInteger(&m_bNeedSelSize);	
			buffer.ReadInteger(&m_bNeedSelMat);	
			buffer.ReadInteger(&method);					
			sel_mat_method=AUX_POLE_SEL_METHOD(method);
		}
#endif
		int n=0;
		buffer.ReadInteger(&n);
		for(int j=0;j<n;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&0x08)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//����
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
}
void CLDSLineAngle::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(pStart)
		buffer.WriteInteger(pStart->handle);
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);
	buffer.WriteString(layer_tag);
	buffer.WriteInteger(m_hPartWeldParent);
	pLsRefList->ToBuffer(buffer,version,doc_type);

	buffer.Write(cfgword.flag.word,24);
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteString(sNotes);
	if(version==0||
		(doc_type==1&&version>=5000010)||
		(doc_type==2&&version>=2000005)||
		(doc_type==4&&version>=1030005)||
		(doc_type==5&&version>=1030005))
	{
		buffer.WriteBooleanThin(_bSyncShadowPartNo);
		if(!_bSyncShadowPartNo)
		{
			buffer.WriteInteger(_iSeg.iSeg);
			buffer.WriteString(sPartNo);
		}
	}
	if (version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
		(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
		(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
		(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
		(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
	{
		buffer.WriteBooleanThin(_blOddByBoltCanAcrossMiddleSect);
		buffer.WriteBooleanThin(_blScatterBoltLocaTolerance);
	}
	buffer.WriteDouble(norm_x_wing.x);
	buffer.WriteDouble(norm_x_wing.y);	
	buffer.WriteDouble(norm_x_wing.z);	
	buffer.WriteDouble(norm_y_wing.x);
	buffer.WriteDouble(norm_y_wing.y);
	buffer.WriteDouble(norm_y_wing.z);	
	buffer.WriteDouble(nominal_length);
	buffer.WriteByte(m_cHeJiaoReservedWidthType);
	buffer.WriteDouble(m_fHeJiaoReservedWidth);
	buffer.WriteInteger(kaihe_base_wing_x0_y1);

	buffer.WriteInteger(m_bEnableTeG);
	buffer.WriteInteger(xWingXZhunJu.g);
	buffer.WriteInteger(xWingXZhunJu.g1);
	buffer.WriteInteger(xWingXZhunJu.g2);
	buffer.WriteInteger(xWingXZhunJu.g3);
	buffer.WriteInteger(xWingYZhunJu.g);
	buffer.WriteInteger(xWingYZhunJu.g1);
	buffer.WriteInteger(xWingYZhunJu.g2);
	buffer.WriteInteger(xWingYZhunJu.g3);
	buffer.WriteDouble(_fCutAngleCoefR);	//Ӧ�ü���,����������Ժ�汾�ѷ����ݲ�֪��δ���
	buffer.WriteWord(_nCutAngleSpaceOdd);	//�Ǹ��н�ʱ����ȥ����Բ���뾶ϵ��֮�����ͷ��϶
	buffer.WriteInteger(group_father_jg_h);
	buffer.WriteInteger(m_bUserSpecStartCutAngle);
	buffer.WriteInteger(m_bUserSpecEndCutAngle);
	buffer.Write(cut_wing_para,6*sizeof(double));
	buffer.WriteInteger(cut_wing[0]);
	buffer.WriteInteger(cut_wing[1]);
	buffer.Write(cut_angle,8*sizeof(double));
	buffer.WriteDouble(baseline.Start().x);
	buffer.WriteDouble(baseline.Start().y);
	buffer.WriteDouble(baseline.Start().z);
	buffer.WriteDouble(baseline.End().x);
	buffer.WriteDouble(baseline.End().y);
	buffer.WriteDouble(baseline.End().z);
#ifdef __COMMON_PART_INC_
	buffer.WriteInteger(m_hArcLift);
	buffer.WritePoint(m_xStartBeforeLift);
	buffer.WritePoint(m_xEndBeforeLift);
#endif
	//ʼ����ͷ�������
	buffer.WriteDouble(start_oddment);
	buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
	buffer.WriteInteger(desStartOdd.m_hRefPart1);
	buffer.WriteInteger(desStartOdd.m_hRefPart2);
	buffer.WriteDouble(desStartOdd.m_fCollideDist);
	//�洢�Ǹ�ʼ�˶���(�̶�)����ͷ
	buffer.WriteByte(extraStartOdd.ber.style);
	if(extraStartOdd.ber.style==0)
		buffer.WriteDouble(extraStartOdd.ber.odd);
	else
		extraStartOdd.ber.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(extraStartOdd.wing_x.style);
	if(extraStartOdd.wing_x.style==0)
		buffer.WriteDouble(extraStartOdd.wing_x.odd);
	else
		extraStartOdd.wing_x.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(extraStartOdd.wing_y.style);
	if(extraStartOdd.wing_y.style==0)
		buffer.WriteDouble(extraStartOdd.wing_y.odd);
	else
		extraStartOdd.wing_y.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	//�ն���ͷ�������
	buffer.WriteDouble(end_oddment);   // �Ǹ�ʼ�ն�����ͷ.
	buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
	buffer.WriteInteger(desEndOdd.m_hRefPart1);
	buffer.WriteInteger(desEndOdd.m_hRefPart2);
	buffer.WriteDouble(desEndOdd.m_fCollideDist);
	//�洢�Ǹ��ն˶���(�̶�)����ͷ
	buffer.WriteByte(extraEndOdd.ber.style);
	if(extraEndOdd.ber.style==0)
		buffer.WriteDouble(extraEndOdd.ber.odd);
	else
		extraEndOdd.ber.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(extraEndOdd.wing_x.style);
	if(extraEndOdd.wing_x.style==0)
		buffer.WriteDouble(extraEndOdd.wing_x.odd);
	else
		extraEndOdd.wing_x.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(extraEndOdd.wing_y.style);
	if(extraEndOdd.wing_y.style==0)
		buffer.WriteDouble(extraEndOdd.wing_y.odd);
	else
		extraEndOdd.wing_y.extraDatumPoint.ToBuffer(buffer,version,doc_type);

	connectStart.ToBuffer(buffer,version,doc_type);   			
	connectEnd.ToBuffer(buffer,version,doc_type);				
	buffer.WriteByte(m_cPosCtrlDatumLineType);
	buffer.WriteByte(m_cPosCtrlType);
	buffer.WriteInteger(m_bLockStartPos);
#ifdef __PART_DESIGN_INC_
	buffer.WriteInteger(m_bAutoCalHoleOutCoef);	
	buffer.WriteDouble(hole_out);			
	buffer.WriteInteger(start_force_type);
	buffer.WriteInteger(end_force_type);
	buffer.WriteInteger(start_joint_type);
	buffer.WriteInteger(end_joint_type);
	//buffer.WriteInteger(CalLenCoef.iSelRFlag);	//(������ʵ������)
	buffer.WriteInteger(CalLenCoef.iTypeNo);
	buffer.WriteInteger(CalLenCoef.hHorizPole);
	buffer.WriteInteger(CalLenCoef.hRefPole);
	buffer.WriteInteger(CalLenCoef.minR.hStartNode);
	buffer.WriteInteger(CalLenCoef.minR.hEndNode);
	buffer.WriteDouble(CalLenCoef.minR.coef);
	buffer.WriteInteger(CalLenCoef.paraR.hStartNode);
	buffer.WriteInteger(CalLenCoef.paraR.hEndNode);
	buffer.WriteDouble(CalLenCoef.paraR.coef);
	buffer.WriteWord(m_iElemType);					
	//buffer.WriteDword(m_uStatMatNo);		//ͳ�����(��ĸ������������)
	//buffer.WriteDouble(bear_load_coef);	//������֧����֧�Ÿ˼�������(������ʵ������)
	buffer.WriteDouble(m_fWeightRaiseCoef);	//�����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
	buffer.WriteInteger(m_bNeedSelSize);	
	buffer.WriteInteger(m_bNeedSelMat);		
	buffer.WriteInteger(sel_mat_method);	
#endif
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		if(pMirObj->mirInfo.axis_flag&0x10)	//����
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);
	}
}
void CLDSLineAngle::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropFromBuffer(buffer,version,doc_type);
		return;
	}
	DWORD dw;
	bool bPrevDeflateVersion=false;	//�ļ�����֮ǰ�İ汾
	if( (doc_type==1&&version>0&&version<4000027)||	//TMA
		(doc_type==2&&version>0&&version<1000082)||	//LMA
		(doc_type==4&&version>0&&version<1000009))	//LDS
		bPrevDeflateVersion=true;
	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
	{
		mpcl.FromBuffer(&buffer,version,doc_type);
		buffer.ReadDword(&dwLayStyle);
	}
	buffer.ReadInteger(&m_uStatMatCoef);
	buffer.ReadInteger(&_hPartWeldParent);
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=5000000)||	//LMA V2.0.0.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		buffer.ReadByte(&_cFuncType);
	if(version==0||doc_type==4||doc_type==5||	//LDS&TDA
		(doc_type==1&&version>=4000021)||		//TMA
		(doc_type==2&&version>=1000076))		//LMA
	{
		if(bPrevDeflateVersion)
		{
			buffer.ReadInteger(&dw);
			m_bUserSpecColor=(dw!=0);
		}
		else
			buffer.ReadBooleanThin(&m_bUserSpecColor);
		if(m_bUserSpecColor)
			buffer.ReadDword(&crMaterial);
	}
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)||defined(__LDS_)
	if(doc_type==3||doc_type==4)
		buffer.ReadInteger(&_uStatMatNo);
#endif
	long i,n,h;
	buffer.ReadInteger(&h);
	//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
	//	SuperiorTower()->hashObjs.GetValueAt(h,(CLDSObject*&)pStart);
	//else
		pStart=BelongModel()->FromNodeHandle(h);	// �����
	buffer.ReadInteger(&h);
	//if(SuperiorTower()->hashObjs.GetHashTableSize()>0)
	//	SuperiorTower()->hashObjs.GetValueAt(h,(CLDSObject*&)pEnd);
	//else
		pEnd  =BelongModel()->FromNodeHandle(h);	// �յ���
	buffer.ReadDouble(&norm_x_wing.x);	// ������֫���߷���.
	buffer.ReadDouble(&norm_x_wing.y);	// ������֫���߷���.
	buffer.ReadDouble(&norm_x_wing.z);	// ������֫���߷���.
	buffer.ReadDouble(&norm_y_wing.x);	// ��һ֫���߷���.
	buffer.ReadDouble(&norm_y_wing.y);	// ��һ֫���߷���.
	buffer.ReadDouble(&norm_y_wing.z);	// ��һ֫���߷���.
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);
		m_bVirtualPart=(dw!=0);	//	�����Ǹ�
	}
	else
		buffer.ReadBooleanThin(&_bVirtualPart);
	buffer.ReadDouble(&nominal_length);
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000110)||	//TMA(V4.0.1.10)
		(doc_type==2&&version>=1000110)||	//LMA(V1.0.1.10)
		(doc_type==4&&version>=1000310))	//LDS(V1.0.3.10)
	{
		buffer.ReadByte(&m_cHeJiaoReservedWidthType);
		buffer.ReadDouble(&m_fHeJiaoReservedWidth);
	}
	buffer.ReadInteger(&kaihe_base_wing_x0_y1);
	buffer.ReadInteger(&huoqu_base_wing_x0_y1_both2);
	buffer.ReadInteger(&huoqu_start1_end2);
	buffer.ReadInteger(&bHuoQuStart);
	buffer.ReadInteger(&bHuoQuEnd);
	buffer.ReadDouble(&pre_huoqu_start_xyz.x);
	buffer.ReadDouble(&pre_huoqu_start_xyz.y);
	buffer.ReadDouble(&pre_huoqu_start_xyz.z);
	buffer.ReadDouble(&pre_huoqu_end_xyz.x);
	buffer.ReadDouble(&pre_huoqu_end_xyz.y);
	buffer.ReadDouble(&pre_huoqu_end_xyz.z);
	//�Ǹ�������������
	buffer.ReadInteger(&m_bEnableTeG);
	buffer.ReadInteger(&xWingXZhunJu.g);
	buffer.ReadInteger(&xWingXZhunJu.g1);
	buffer.ReadInteger(&xWingXZhunJu.g2);
	buffer.ReadInteger(&xWingXZhunJu.g3);
	buffer.ReadInteger(&xWingYZhunJu.g);
	buffer.ReadInteger(&xWingYZhunJu.g1);
	buffer.ReadInteger(&xWingYZhunJu.g2);
	buffer.ReadInteger(&xWingYZhunJu.g3);
	
	//buffer.ReadInteger(&handle);					// ���Ǹ־��	
	if(doc_type==4&&(version==0||version>=1000204))	//LDS
	{
		buffer.ReadInteger(&size.idClassType);
		buffer.ReadByte(&size.cSubClassType);
	}
	buffer.ReadDouble(&size.wide);
	buffer.ReadDouble(&size.thick);
	if( version==0||
		(doc_type==1&&version>=4000113)||	//TMA(V4.0.1.13)
		(doc_type==2&&version>=1000113)||	//LMA(V1.0.1.13)
		(doc_type==3&&version>=1070313)||	//TSA(V1.7.3.13)
		(doc_type==4&&version>=1000313)||	//LDS(V1.0.3.13)
		(doc_type==5&&version>=1000003))	//TDA(V1.0.0.03)
	{	//��ȡ���ȱ߽Ǹ�Y֫��֫�� wht 12-04-26
		buffer.ReadDouble(&size.height);	
	}
	buffer.ReadByte(&_material);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	buffer.ReadInteger(&m_bReplPart);
	buffer.ReadByte(&org_material);
	buffer.ReadDouble(&org_wing_wide);
	buffer.ReadDouble(&org_wing_thick);
	_start_push_flat_x1_y2=(BYTE)buffer.ReadInteger();
	buffer.ReadDouble(&_start_push_flat_length);
	_end_push_flat_x1_y2=(BYTE)buffer.ReadInteger();
	buffer.ReadDouble(&_end_push_flat_length);
	//
	if( version==0||
		(doc_type==1&&version>=4020004)||	//TMA(V4.2.0.4)
		(doc_type==2&&version>=1020004)||	//LMA(V1.2.0.4)
		(doc_type==3&&version>=1080104)||	//TSA(V1.8.1.4)
		(doc_type==4&&version>=1020004)||	//LDS(V1.2.0.4)
		(doc_type==5&&version>=1020004))	//TDA(V1.2.0.4)
	{	//��ȡ�м�ѹ����ز��� wht 12-10-09
		_mid_push_flat_x1_y2=(BYTE)buffer.ReadInteger();
		if(mid_push_flat_x1_y2>0)
		{
			buffer.ReadDouble(&_mid_push_flat_length);
			buffer.ReadInteger(&mid_push_flat_node_handle);
			if(	version==0||
				(doc_type==1&&version>=5010300)||	//TMA V5.1.3.0
				(doc_type==2&&version>=2000600)||	//LMA V2.0.6.0
				(doc_type==4&&version>=1030600)||	//LDS V1.3.6.0
				(doc_type==5&&version>=1030600))	//TDA V1.3.6.0
				buffer.ReadDouble(&_mid_push_flat_up_offset);
			else
				_mid_push_flat_up_offset=0;
		}
	}

	if( version==0||
		(doc_type==1&&version>=4010100)||	//TMA(V4.1.1.0)
		(doc_type==2&&version>=1010100)||	//LMA(V1.1.1.0)
		(doc_type==3&&version>=2000100)||	//TSA(V2.0.1.0)
		(doc_type==4&&version>=1020100)||	//LDS(V1.2.1.0)
		(doc_type==5&&version>=1020100))	//TDA(V1.2.1.0)
		buffer.ReadDouble(&_fCutAngleCoefR);
	else
		_fCutAngleCoefR=0;
	
	if( version==0||
		(doc_type==1&&version>=4010102)||	//TMA(V4.1.1.2)
		(doc_type==2&&version>=1010102&&version!=1020009)||	//LMA(V1.1.1.2)
		(doc_type==3&&version>=2000100)||	//TSA(V2.0.1.0)
		(doc_type==4&&version>=1020100)||	//LDS(V1.2.1.1)
		(doc_type==5&&version>=1020100))	//TDA(V1.2.1.1)
		buffer.ReadWord(&_nCutAngleSpaceOdd);	//�Ǹ��н�ʱ����ȥ����Բ���뾶ϵ��֮�����ͷ��϶
	else
		_nCutAngleSpaceOdd=0;
	_blScatterBoltLocaTolerance=false;
	if (version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
		(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
		(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
		(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
		(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
	{
		buffer.ReadBooleanThin(&_blOddByBoltCanAcrossMiddleSect);
		buffer.ReadBooleanThin(&_blScatterBoltLocaTolerance);
	}
	else if(doc_type==1||doc_type==2)	//�ɸ�ʽ�ļ���ʱ�����ǵ��ɷ�����Աϰ��Ӧ������˨�������ͷʱ����Ǹ��ж���
		_blOddByBoltCanAcrossMiddleSect=true;
	else
		_blOddByBoltCanAcrossMiddleSect=false;
	//��ȡ�Ǹֵ���Բ������
	if(	version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0	��V5.2.1ʱ©�� wjh-2019.8.13
		(doc_type==2&&version>=2010100)||	//LMA V2,1,1,0
		(doc_type==3&&version>=2010400)||	//TSA V2,1,4,0
		(doc_type==4&&version>=1030900)||	//LDS V1,3,9,0
		(doc_type==5&&version>=1030900)))	//TDA V1,3,9,0
	{
		buffer.ReadDouble(&_dfFilletROfSX);
		buffer.ReadDouble(&_dfFilletROfSY);
		buffer.ReadDouble(&_dfFilletROfEX);
		buffer.ReadDouble(&_dfFilletROfEY);
		//��ȡ���
		BYTE ciIndex,cnCount=0;
		buffer.ReadByte(&cnCount);
		xarrOpenCuts.Clear();
		for(ciIndex=0;ciIndex<cnCount;ciIndex++)
		{
			OPENING_CUT cut;
			buffer.ReadDouble(&cut.dfLateralBtmXorY);
			buffer.ReadDouble(&cut.dfLenPosFrom);
			buffer.ReadDouble(&cut.dfLenPosTo);
			buffer.ReadDouble(&cut.dfTipDeltaS);
			buffer.ReadDouble(&cut.dfTipDeltaE);
			//if(doc_type == 4 && version >= 1030901)	//LDS V1,3,9,0 2019��4��17��֮ǰ�汾�޴�����
				buffer.ReadWord(&cut.siCornerFilletR);
			xarrOpenCuts.Append(cut);
		}
		//д��ֲ����Ͻ�
		buffer.ReadByte(&cnCount);
		xarrKaiheSects.Clear();
		for(ciIndex=0;ciIndex<cnCount;ciIndex++)
		{
			KAIHE_SECTION sect;
			buffer.ReadByte(&sect.ciOperWing);
			buffer.ReadDouble(&sect.dfFromX);
			buffer.ReadDouble(&sect.dfToX);
			buffer.ReadDouble(&sect.dfReservedSizeY);
			buffer.ReadDouble(&sect.dfOpenAngle);
			xarrKaiheSects.Append(sect);
		}
	}

	buffer.ReadInteger(&group_father_jg_h);
	buffer.ReadInteger(&_iSeg.iSeg);
	buffer.ReadString(sPartNo);	//ԭʼ�������
	buffer.ReadString(layer(),4);					// ���Ǹ�����ͼ��.
	// --------- 1.2. �н� -----------
	if(version<=0||doc_type==5
		||(doc_type==1&&version>=4000104)	//TMA
		||(doc_type==2&&version>=1000104)	//LMA
		||(doc_type==4&&version>=1000304))	//LDS
	{
		buffer.ReadInteger(&m_bUserSpecStartCutAngle);
		buffer.ReadInteger(&m_bUserSpecEndCutAngle);
	}
	/*cut_wing_para[0]��ʾʼ����֫cut_wing_para[1]��ʾ�ն���֫
	...[][0]��ʾX����Ƥ��ȡ����
	...[][1]��ʾY����Ƥ��ȡ����
	...[][2]��ʾZ����Ƥ��ȡ����
	*/
	buffer.Read(cut_wing_para,6*sizeof(double));
	//������֫����֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
	buffer.ReadInteger(&cut_wing[0]);
	buffer.ReadInteger(&cut_wing[1]);
// --------- 1.3. ��֫ -----------
	/*
	cut_angle[0]��ʾ���X֫
	cut_angle[1]��ʾ���Y֫
	cut_angle[2]��ʾ�յ�X֫
	cut_angle[3]��ʾ�յ�Y֫
	...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
	*/
	buffer.Read(cut_angle,8*sizeof(double));
	// --------- 1.4. ���������-----------
	_bCutRoot=(buffer.ReadInteger()!=0);
	_bCutBer=(buffer.ReadInteger()!=0);
	buffer.ReadInteger(&m_bWeldPart);
	buffer.ReadWord(&m_nCutBerR);
	
	f3dPoint start,end;
	buffer.ReadDouble(&start.x);// ʵ���������
	buffer.ReadDouble(&start.y);// ʵ���������
	buffer.ReadDouble(&start.z);// ʵ���������
	buffer.ReadDouble(&end.x);	// ʵ�������յ�
	buffer.ReadDouble(&end.y);	// ʵ�������յ�
	buffer.ReadDouble(&end.z);	// ʵ�������յ�
	baseline.SetStart(start);
	baseline.SetEnd(end);
	m_xStartBeforeLift=start;	//��Ĭ��ֵ
	m_xEndBeforeLift  =end;		//��Ĭ��ֵ
#ifdef __COMMON_PART_INC_
	if(  version==0||
		(doc_type==1&&version>=4020001)||	//TMA V4.2.0.1
		(doc_type==2&&version>=1020001)||	//LMA V1.2.0.1
		//(doc_type==3&&version>=1080001)||	//TSA V1.8.0.1
		(doc_type==4&&version>=1020001)||	//LDS V1.2.0.1
		(doc_type==5&&version>=1020001))	//TDA V1.2.0.1
	{
		buffer.ReadInteger(&m_hArcLift);
		buffer.ReadPoint(m_xStartBeforeLift);
		buffer.ReadPoint(m_xEndBeforeLift);
	}
#endif
	//ʼ����ͷ�������
	buffer.ReadDouble(&start_oddment);
	buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
	buffer.ReadInteger(&desStartOdd.m_hRefPart1);
	buffer.ReadInteger(&desStartOdd.m_hRefPart2);
	buffer.ReadDouble(&desStartOdd.m_fCollideDist);
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000100)||	//TMA V4.0.1.0
		(doc_type==2&&version>=1000100)||	//LMA V1.0.1.0
		(doc_type==4&&version>=1000300))	//LDS V1.0.3.0
	{
		//�洢�Ǹ�ʼ�˶���(�̶�)����ͷ
		buffer.ReadByte(&extraStartOdd.ber.style);
		if(extraStartOdd.ber.style==0)
			buffer.ReadDouble(&extraStartOdd.ber.odd);
		else
		{
			if(   (doc_type==1&&(version>0&&version<4000107)) //TMA V4.0.1.0
				||(doc_type==2&&(version>0&&version<1000107)) //LMA V1.0.1.0
				||(doc_type==4&&(version>0&&version<1000307)))//LDS V1.0.3.0
				extraStartOdd.ber.extraDatumPoint.FromBuffer(buffer,version,doc_type);	//extraStartOdd.ber.extraStartPoint
			extraStartOdd.ber.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		}
		buffer.ReadByte(&extraStartOdd.wing_x.style);
		if(extraStartOdd.wing_x.style==0)
			buffer.ReadDouble(&extraStartOdd.wing_x.odd);
		else
		{
			if(   (doc_type==1&&(version>0&&version<4000107)) //TMA V4.0.1.0
				||(doc_type==2&&(version>0&&version<1000107)) //LMA V1.0.1.0
				||(doc_type==4&&(version>0&&version<1000307)))//LDS V1.0.3.0
				extraStartOdd.wing_x.extraDatumPoint.FromBuffer(buffer,version,doc_type);	//extraStartOdd.wing_x.extraStartPoint
			extraStartOdd.wing_x.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		}
		buffer.ReadByte(&extraStartOdd.wing_y.style);
		if(extraStartOdd.wing_y.style==0)
			buffer.ReadDouble(&extraStartOdd.wing_y.odd);
		else
		{
			if(   (doc_type==1&&(version>0&&version<4000107)) //TMA V4.0.1.0
				||(doc_type==2&&(version>0&&version<1000107)) //LMA V1.0.1.0
				||(doc_type==4&&(version>0&&version<1000307)))//LDS V1.0.3.0
				extraStartOdd.wing_y.extraDatumPoint.FromBuffer(buffer,version,doc_type);	//extraStartOdd.wing_y.extraStartPoint
			extraStartOdd.wing_y.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		}
	}
	//else	//ԭ������Ĭ��ֵΪ2�������ǵ���λ�ٶȼ������жϵ���ȷ�ʣ�����Ĭ��ֵΪ0���ʲ������´�����
	//	extraStartOdd.ber.style=0;
	//�ն���ͷ�������
	buffer.ReadDouble(&end_oddment);   // �Ǹ�ʼ�ն�����ͷ.
	buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
	buffer.ReadInteger(&desEndOdd.m_hRefPart1);
	buffer.ReadInteger(&desEndOdd.m_hRefPart2);
	buffer.ReadDouble(&desEndOdd.m_fCollideDist);
	if( version==0||doc_type==5||
		(doc_type==1&&version>=4000100)||	//TMA V4.0.1.0
		(doc_type==2&&version>=1000100)||	//LMA V1.0.1.0
		(doc_type==4&&version>=1000300))	//LDS V1.0.3.0
	{
		//�洢�Ǹ��ն˶���(�̶�)����ͷ
		buffer.ReadByte(&extraEndOdd.ber.style);
		if(extraEndOdd.ber.style==0)
			buffer.ReadDouble(&extraEndOdd.ber.odd);
		else
		{
			if(   (doc_type==1&&(version>0&&version<4000107)) //TMA V4.0.1.0
				||(doc_type==2&&(version>0&&version<1000107)) //LMA V1.0.1.0
				||(doc_type==4&&(version>0&&version<1000307)))//LDS V1.0.3.0
				extraEndOdd.ber.extraDatumPoint.FromBuffer(buffer,version,doc_type);	//extraEndOdd.ber.extraStartPoint
			extraEndOdd.ber.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		}
		buffer.ReadByte(&extraEndOdd.wing_x.style);
		if(extraEndOdd.wing_x.style==0)
			buffer.ReadDouble(&extraEndOdd.wing_x.odd);
		else
		{
			if(   (doc_type==1&&(version>0&&version<4000107)) //TMA V4.0.1.0
				||(doc_type==2&&(version>0&&version<1000107)) //LMA V1.0.1.0
				||(doc_type==4&&(version>0&&version<1000307)))//LDS V1.0.3.0
				extraEndOdd.wing_x.extraDatumPoint.FromBuffer(buffer,version,doc_type);	//extraEndOdd.wing_x.extraStartPoint
			extraEndOdd.wing_x.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		}
		buffer.ReadByte(&extraEndOdd.wing_y.style);
		if(extraEndOdd.wing_y.style==0)
			buffer.ReadDouble(&extraEndOdd.wing_y.odd);
		else
		{
			if(   (doc_type==1&&(version>0&&version<4000107)) //TMA V4.0.1.0
				||(doc_type==2&&(version>0&&version<1000107)) //LMA V1.0.1.0
				||(doc_type==4&&(version>0&&version<1000307)))//LDS V1.0.3.0
				extraEndOdd.wing_y.extraDatumPoint.FromBuffer(buffer,version,doc_type);	//extraEndOdd.wing_y.extraStartPoint
			extraEndOdd.wing_y.extraDatumPoint.FromBuffer(buffer,version,doc_type);
		}
	}
	//else	//ԭ������Ĭ��ֵΪ2�������ǵ���λ�ٶȼ������жϵ���ȷ�ʣ�����Ĭ��ֵΪ0���ʲ������´�����
	//	extraEndOdd.ber.style=0;
	connectStart.FromBuffer(buffer,version,doc_type);   			//(��ˮƽ�Ǹֵ��϶˻�ˮƽ�Ǹֵ�)��ʼ����˨��Ϣ
	connectEnd.FromBuffer(buffer,version,doc_type);				//(��ˮƽ�Ǹֵ��¶˻�ˮƽ�Ǹֵ�)ĩ��  ��˨��Ϣ
	//��ʱ�Դ���,����Ӧ��CConnectInfo�м�¼����֫����,�����Ȳ�����Ĭ��Ϊ˫֫���� wjh-2014.12.16
	//if(layer_tag[0]!='T'&&layer_tag[1]=='Z')
	//	connectStart.m_iConnectWing=connectEnd.m_iConnectWing=0;
	//else
	//	connectStart.m_iConnectWing=connectEnd.m_iConnectWing=1;
	if( version==0||
		(doc_type==1&&version>=4020101)||	//TMA
		(doc_type==2&&version>=1020101)||	//LMA
		(doc_type==5&&version>=1020101)||	//TDA
		(doc_type==4&&version>=1020101))	//LDS
	{
		buffer.ReadInteger(&connectStart.idAngleJoint);
		buffer.ReadInteger(&connectEnd.idAngleJoint);
	}
	pLsRefList->FromBuffer(buffer,m_pModel,version,doc_type);
	buffer.ReadInteger(&feature);		//����(������Ϊ���Ǹֱ�־�û���ʱ�Զ��������)
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);//��ʾ��˨�׼��и���
		m_bDisplayHole=(dw!=0);
	}
	else
		buffer.ReadBooleanThin(&m_bDisplayHole);

	//�����Ϣ
	buffer.ReadInteger(&des_norm_x.bByOtherWing);
	buffer.ReadInteger(&des_norm_x.bSpecific);
	buffer.ReadDouble(&des_norm_x.near_norm.x);	// 
	buffer.ReadDouble(&des_norm_x.near_norm.y);	// 
	buffer.ReadDouble(&des_norm_x.near_norm.z);	// 
	des_norm_x.spec_norm.FromBuffer(buffer,version,doc_type);
	if(des_norm_x.spec_norm.norm_style==0)
		des_norm_x.spec_norm.vector=norm_x_wing;
	buffer.ReadInteger(&des_norm_x.hViceJg);
	buffer.ReadInteger(&des_norm_y.bByOtherWing);
	buffer.ReadInteger(&des_norm_y.bSpecific);
	buffer.ReadDouble(&des_norm_y.near_norm.x);	// 
	buffer.ReadDouble(&des_norm_y.near_norm.y);	// 
	buffer.ReadDouble(&des_norm_y.near_norm.z);	// 
	des_norm_y.spec_norm.FromBuffer(buffer,version,doc_type);
	if(des_norm_y.spec_norm.norm_style==0)
		des_norm_y.spec_norm.vector=norm_y_wing;
	buffer.ReadInteger(&des_norm_y.hViceJg);
	if( version==0||
		(doc_type==1&&version>=4010001)||	//TMA
		(doc_type==2&&version>=1010001)||	//LMA
		(doc_type==4&&version>=1010001)||	//LDS
		(doc_type==5&&version>=1010001))	//TDA
	{
		buffer.ReadByte(&m_cPosCtrlDatumLineType);
		buffer.ReadByte(&m_cPosCtrlType);
		desStartPos.FromBuffer(buffer,version,doc_type);
		buffer.ReadInteger(&m_bLockStartPos);
		desEndPos.FromBuffer(buffer,version,doc_type);
		buffer.ReadInteger(&m_bLockEndPos);
	}
	else
	{
		if(pOldDesStartPos==NULL)
			pOldDesStartPos=new CJgBerPosPara();
		//��������Ϊ���¹���ԭ����޸ĵ� WJH-2005.03.03
		pOldDesStartPos->FromBuffer(buffer,version,doc_type);
		/*buffer.ReadInteger(&pOldDesStartPos->jgber_cal_style);		//�Ǹ������㷽ʽ
		buffer.ReadInteger(&pOldDesStartPos->datum_pos_style);		//������㷽ʽ
		buffer.ReadInteger(&pOldDesStartPos->datum_to_ber_style);	//���ݻ���������ķ�ʽ
		pOldDesStartPos->datum_point.FromBuffer(buffer,version);
		buffer.ReadWord(&pOldDesStartPos->work_wing_auto0_x1_y2_both3);
		buffer.ReadInteger(&pOldDesStartPos->prj_type);
		if( version==0||doc_type==5||
		(doc_type==1&&version>=4000022)||	//TMA
		(doc_type==2&&version>=1000077)||	//LMA
		(doc_type==4&&version>=1000002))	//LDS
		buffer.ReadDouble(&pOldDesStartPos->len_offset_dist);
		buffer.ReadDouble(&pOldDesStartPos->datum_offset_dist);
		buffer.ReadDouble(&pOldDesStartPos->offset_X_dist);
		buffer.ReadInteger(&pOldDesStartPos->offset_X_dist_style);
		buffer.ReadDouble(&pOldDesStartPos->offset_Y_dist);
		buffer.ReadInteger(&pOldDesStartPos->offset_Y_dist_style);
		buffer.ReadInteger(&pOldDesStartPos->datum_jg_h);
		buffer.ReadInteger(&pOldDesStartPos->start_jg_h);
		buffer.ReadInteger(&pOldDesStartPos->end_jg_h);
		buffer.ReadInteger(&pOldDesStartPos->iStartJgBerStyle);
		buffer.ReadInteger(&pOldDesStartPos->iEndJgBerStyle);
		buffer.ReadInteger(&pOldDesStartPos->huoqu_jg_h);			// ������׼�Ǹ־��
		buffer.ReadByte(&pOldDesStartPos->cFaceOffsetDatumLine);
		buffer.ReadDouble(&pOldDesStartPos->face_offset_norm.x);	// ƽ���淨��
		buffer.ReadDouble(&pOldDesStartPos->face_offset_norm.y);	// ƽ���淨��
		buffer.ReadDouble(&pOldDesStartPos->face_offset_norm.z);	// ƽ���淨��
		buffer.ReadDouble(&pOldDesStartPos->huoqu_dist);			//�����߼�϶
		buffer.ReadDouble(&pOldDesStartPos->offset_zhun);			//ƫ��׼�ߵ�ƫ����
		buffer.ReadInteger(&pOldDesStartPos->bByOtherEndOffset);	//������һ�˽���ƽ�Ƽ���
		buffer.ReadDouble(&pOldDesStartPos->fStartJgLengOffsetX);
		buffer.ReadDouble(&pOldDesStartPos->fStartJgLengOffsetY);
		buffer.ReadDouble(&pOldDesStartPos->fEndJgLengOffsetX);
		buffer.ReadDouble(&pOldDesStartPos->fEndJgLengOffsetY);
		if( version==0||doc_type==4||doc_type==5||
		(doc_type==1&&version>=4000020)||	//TMA
		(doc_type==2&&version>=1000075))	//LMA
		buffer.ReadDouble(&pOldDesStartPos->fEccentricDist);*/
		buffer.ReadInteger(&m_bLockStartPos);
		if(pOldDesEndPos==NULL)
			pOldDesEndPos=new CJgBerPosPara();
		pOldDesEndPos->FromBuffer(buffer,version,doc_type);
		//��������Ϊ���¹���ԭ����޸ĵ� WJH-2005.03.03
		/*buffer.ReadInteger(&pOldDesEndPos->jgber_cal_style);		//�Ǹ������㷽ʽ
		buffer.ReadInteger(&pOldDesEndPos->datum_pos_style);		//������㷽ʽ
		buffer.ReadInteger(&pOldDesEndPos->datum_to_ber_style);	//���ݻ���������ķ�ʽ
		pOldDesEndPos->datum_point.FromBuffer(buffer,version);
		buffer.ReadWord(&pOldDesEndPos->work_wing_auto0_x1_y2_both3);
		buffer.ReadInteger(&pOldDesEndPos->prj_type);
		if( version==0||doc_type==5||
		(doc_type==1&&version>=4000022)||	//TMA
		(doc_type==2&&version>=1000077)||	//LMA
		(doc_type==4&&version>=1000002))	//LDS
		buffer.ReadDouble(&pOldDesEndPos->len_offset_dist);
		buffer.ReadDouble(&pOldDesEndPos->datum_offset_dist);
		buffer.ReadDouble(&pOldDesEndPos->offset_X_dist);
		buffer.ReadInteger(&pOldDesEndPos->offset_X_dist_style);
		buffer.ReadDouble(&pOldDesEndPos->offset_Y_dist);
		buffer.ReadInteger(&pOldDesEndPos->offset_Y_dist_style);
		buffer.ReadInteger(&pOldDesEndPos->datum_jg_h);
		buffer.ReadInteger(&pOldDesEndPos->start_jg_h);
		buffer.ReadInteger(&pOldDesEndPos->end_jg_h);
		buffer.ReadInteger(&pOldDesEndPos->iStartJgBerStyle);
		buffer.ReadInteger(&pOldDesEndPos->iEndJgBerStyle);
		buffer.ReadInteger(&pOldDesEndPos->huoqu_jg_h);		// ������׼�Ǹ־��
		buffer.ReadByte(&pOldDesEndPos->cFaceOffsetDatumLine);
		buffer.ReadDouble(&pOldDesEndPos->face_offset_norm.x);	// ƽ���淨��
		buffer.ReadDouble(&pOldDesEndPos->face_offset_norm.y);	// ƽ���淨��
		buffer.ReadDouble(&pOldDesEndPos->face_offset_norm.z);	// ƽ���淨��
		buffer.ReadDouble(&pOldDesEndPos->huoqu_dist);			//�����߼�϶
		buffer.ReadDouble(&pOldDesEndPos->offset_zhun);		//ƫ��׼�ߵ�ƫ����
		buffer.ReadInteger(&pOldDesEndPos->bByOtherEndOffset);	//������һ�˽���ƽ�Ƽ���
		buffer.ReadDouble(&pOldDesEndPos->fStartJgLengOffsetX);
		buffer.ReadDouble(&pOldDesEndPos->fStartJgLengOffsetY);
		buffer.ReadDouble(&pOldDesEndPos->fEndJgLengOffsetX);
		buffer.ReadDouble(&pOldDesEndPos->fEndJgLengOffsetY);
		if( version==0||doc_type==4||doc_type==5||
		(doc_type==1&&version>=4000020)||	//TMA
		(doc_type==2&&version>=1000075))	//LMA
		buffer.ReadDouble(&pOldDesEndPos->fEccentricDist);*/
		buffer.ReadInteger(&m_bLockEndPos);
	}
	buffer.ReadDouble(&huoqu_r);
	buffer.ReadInteger(&huoqu_handle);
	buffer.ReadString(sNotes,51);
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)
	if(doc_type==3||doc_type==4)
	{
		buffer.ReadInteger(&m_bAutoCalHoleOutCoef);	//�Զ�������˨������������Ϊָ����˨������
		buffer.ReadDouble(&hole_out);			//�˼��������˨������
		buffer.ReadInteger(&start_force_type);
		buffer.ReadInteger(&end_force_type);
		buffer.ReadInteger(&start_joint_type);
		buffer.ReadInteger(&end_joint_type);
		buffer.ReadInteger(&CalLenCoef.iSelRFlag);
		buffer.ReadInteger(&CalLenCoef.iTypeNo);
		if(version>0&&version<1000012)
		{
			if(CalLenCoef.iTypeNo<4)
				CalLenCoef.iTypeNo=0;
			else if(CalLenCoef.iTypeNo<8)
				CalLenCoef.iTypeNo-=3;
			else 
				CalLenCoef.iTypeNo=5;	//��ָ�����㳤��ͳһ��Ϊƽ������С��ͬʱָ��һ������
		}
		buffer.ReadInteger(&CalLenCoef.hHorizPole);
		buffer.ReadInteger(&CalLenCoef.hRefPole);
		if(version==0||(doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
		{
			buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
			buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
			buffer.ReadDouble(&CalLenCoef.minR.coef);
			buffer.ReadInteger(&CalLenCoef.paraR.hStartNode);
			buffer.ReadInteger(&CalLenCoef.paraR.hEndNode);
			buffer.ReadDouble(&CalLenCoef.paraR.coef);
		}
		else
		{
			CalLenCoef.minR.hStartNode=CalLenCoef.minR.hEndNode=CalLenCoef.paraR.hStartNode=CalLenCoef.paraR.hEndNode=0;
			buffer.ReadDouble(&CalLenCoef.minR.coef);
			buffer.ReadDouble(&CalLenCoef.paraR.coef);
		}
		buffer.ReadWord(&m_iElemType);			//0��ʾ�Զ��ж�;1��ʾ���������;2��ʾ��������Ԫ;3��ʾ������Ԫ
		buffer.ReadDword(&_uStatMatNo);		//ͳ�����
		buffer.ReadInteger(&m_bNeedSelSize);	//��Ҫѡ����
		buffer.ReadInteger(&m_bNeedSelMat);		//��Ҫѡ�����
		if(version>=1000003&&version<1000203)
		{
			BOOL bAnalysisInc;
			buffer.ReadInteger(&bAnalysisInc);
		}
		int method;
		buffer.ReadInteger(&method);		//����ѡ�ķ�ʽ
		sel_mat_method=AUX_POLE_SEL_METHOD(method);
		buffer.ReadDouble(&bear_load_coef);		//������֧����֧�Ÿ˼�������
		buffer.ReadDouble(&m_fWeightRaiseCoef);	//�����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
		if(	version==0|| (doc_type==4&&version>=1030800))	//LDS V1.3.8.0
			buffer.ReadByte(&m_cbRodFlag);
	}
#endif
	//m_bReplPart=FALSE;	//��ʱΪ�˷�ֹ��Դ�ļ������Ժ���Ҫȥ��
	//m_bCutBer=m_bCutRoot=m_bWeldPart=FALSE;	//��ʱΪ�˷�ֹ��Դ�ļ������Ժ���Ҫȥ��
	//���������б�
	buffer.ReadInteger(&n);
	RELATIVE_OBJECT *pRelaObj=NULL;
	relativeObjs.Empty();
	for(i=0;i<n;i++)
	{
		pRelaObj=relativeObjs.Add(buffer.ReadInteger());
		if(bPrevDeflateVersion)
		{
			buffer.ReadDword(&dw);
			pRelaObj->mirInfo.axis_flag=(BYTE)dw;
			buffer.ReadInteger(&dw);
			pRelaObj->mirInfo.array_num=(BYTE)dw;
			buffer.ReadPoint(pRelaObj->mirInfo.origin);
			double ff;
			buffer.ReadDouble(&ff);
			pRelaObj->mirInfo.rotate_angle=(short)ff;
		}
		else
		{
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
	}
	_bFootNail=(buffer.ReadInteger()!=0);

	if( (doc_type==1&&version>0&&version<=4000112)||	//TMA
		(doc_type==2&&version>0&&version<=1000112)||	//LMA
		(doc_type==5&&version>0&&version<=1000002)||	//TDA
		(doc_type==4&&version>0&&version<=1000312))	//LDS
	{
		desStartPos.FromJgBerPosPara(pOldDesStartPos);
		if(desStartPos.jgber_cal_style==1)	//ֱ��ָ���Ǹ��������
			desStartPos.datumPoint.SetPosition(Start());
		desEndPos.FromJgBerPosPara(pOldDesEndPos);
		if(desEndPos.jgber_cal_style==1)	//ֱ��ָ���Ǹ��������
			desEndPos.datumPoint.SetPosition(End());
	}
	if( (doc_type==1&&version>0&&version<5010300)||	//TMA V5.1.3.0
		(doc_type==2&&version>0&&version<2000600)||	//LMA V2.0.6.0
		(doc_type==4&&version>0&&version<1030600)||	//LDS V1.3.6.0
		(doc_type==5&&version>0&&version<1030600))		//TDA V1.3.6.0
	{	//�°������ýǸ��޶˽ڵ��Ҵ���ָ������ʱ��������֫����ƫ�ơ�wjh-2016.5.19
		if(pStart==NULL&&desStartPos.spatialOperationStyle==0)
		{
			desStartPos.wing_x_offset.gStyle=desStartPos.wing_y_offset.gStyle=4;
			desStartPos.wing_x_offset.offsetDist=desStartPos.wing_y_offset.offsetDist=0;
		}
		if(pEnd==NULL&&desEndPos.spatialOperationStyle==0)
		{
			desEndPos.wing_x_offset.gStyle=desEndPos.wing_y_offset.gStyle=4;
			desEndPos.wing_x_offset.offsetDist=desEndPos.wing_y_offset.offsetDist=0;
		}
	}
	SetModified();
}
void CLDSLineAngle::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
	{
		IndependentShadowPropToBuffer(buffer,version,doc_type);
		return;
	}
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	if( version==0||
	   (version>=5000003&&doc_type==1)||(version>=2000003&&doc_type==2)||
	   (version>=1030003&&doc_type==4)||(version>=1030003&&doc_type==5))
	{
		mpcl.ToBuffer(&buffer,version,doc_type);
		buffer.WriteDword(dwLayStyle);
	}
	buffer.WriteInteger(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	if(	version==0||
		(doc_type==1&&version>=5000000)||	//TMA V5.0.0.0
		(doc_type==2&&version>=5000000)||	//LMA V2.0.0.0
		(doc_type==3&&version>=2010000)||	//TSA V2.1.0.0
		(doc_type==4&&version>=1030000)||	//LDS V1.3.0.0
		(doc_type==5&&version>=1030000))	//TDA V1.3.0.0
		buffer.WriteByte(m_cFuncType);
	buffer.WriteBooleanThin(m_bUserSpecColor);	//�û�ָ��������ɫ
	if(m_bUserSpecColor)
		buffer.WriteDword(crMaterial);		//������ɫ
#ifdef __PART_DESIGN_INC_
	if(doc_type==3||doc_type==4)
		buffer.WriteInteger(m_uStatMatNo);
#endif
	if(pStart)
		buffer.WriteInteger(pStart->handle);	// �����
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);	// �յ���
	else
		buffer.WriteInteger(0);
	buffer.WriteDouble(norm_x_wing.x);	// ������֫���߷���.
	buffer.WriteDouble(norm_x_wing.y);	// ������֫���߷���.
	buffer.WriteDouble(norm_x_wing.z);	// ������֫���߷���.
	buffer.WriteDouble(norm_y_wing.x);	// ��һ֫���߷���.
	buffer.WriteDouble(norm_y_wing.y);	// ��һ֫���߷���.
	buffer.WriteDouble(norm_y_wing.z);	// ��һ֫���߷���.
	buffer.WriteBooleanThin(_bVirtualPart);	//	�����Ǹ�
	buffer.WriteDouble(nominal_length);
	buffer.WriteByte(m_cHeJiaoReservedWidthType);
	buffer.WriteDouble(m_fHeJiaoReservedWidth);
	buffer.WriteInteger(kaihe_base_wing_x0_y1);
	buffer.WriteInteger(huoqu_base_wing_x0_y1_both2);
	buffer.WriteInteger(huoqu_start1_end2);
	buffer.WriteInteger(bHuoQuStart);
	buffer.WriteInteger(bHuoQuEnd);
	buffer.WriteDouble(pre_huoqu_start_xyz.x);
	buffer.WriteDouble(pre_huoqu_start_xyz.y);
	buffer.WriteDouble(pre_huoqu_start_xyz.z);
	buffer.WriteDouble(pre_huoqu_end_xyz.x);
	buffer.WriteDouble(pre_huoqu_end_xyz.y);
	buffer.WriteDouble(pre_huoqu_end_xyz.z);

	//�Ǹ�������������
	buffer.WriteInteger(m_bEnableTeG);
	buffer.WriteInteger(xWingXZhunJu.g);
	buffer.WriteInteger(xWingXZhunJu.g1);
	buffer.WriteInteger(xWingXZhunJu.g2);
	buffer.WriteInteger(xWingXZhunJu.g3);
	buffer.WriteInteger(xWingYZhunJu.g);
	buffer.WriteInteger(xWingYZhunJu.g1);
	buffer.WriteInteger(xWingYZhunJu.g2);
	buffer.WriteInteger(xWingYZhunJu.g3);
	
	//buffer.WriteInteger(handle);					// ���Ǹ־��	
#if defined(__LDS_)||defined(__LDS_FILE_)
	if(doc_type==3||doc_type==4)
	{
		buffer.WriteInteger(size_idClassType);
		buffer.WriteByte(size_cSubClassType);
	}
#endif
	buffer.WriteDouble(size_wide);
	buffer.WriteDouble(size_thick);
	buffer.WriteDouble(size_height);	//���治�ȱ߽Ǹ�Y֫��֫�� wht 12-04-26
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteInteger(m_bReplPart);
	buffer.WriteByte(org_material);
	buffer.WriteDouble(org_wing_wide);
	buffer.WriteDouble(org_wing_thick);
	buffer.WriteInteger(start_push_flat_x1_y2);
	buffer.WriteDouble(start_push_flat_length);
	buffer.WriteInteger(end_push_flat_x1_y2);
	buffer.WriteDouble(end_push_flat_length);
	//�����м�ѹ����ز��� wht 12-10-09
	if( version==0||
		(doc_type==1&&version>=4020004)||	//TMA(V4.2.0.4)
		(doc_type==2&&version>=1020004)||	//LMA(V1.2.0.4)
		(doc_type==3&&version>=1080104)||	//TSA(V1.8.1.4)
		(doc_type==4&&version>=1020004)||	//LDS(V1.2.0.4)
		(doc_type==5&&version>=1020004))	//TDA(V1.2.0.4)
	{
		buffer.WriteInteger(mid_push_flat_x1_y2);
		if(mid_push_flat_x1_y2>0)
		{
			buffer.WriteDouble(_mid_push_flat_length);
			buffer.WriteInteger(mid_push_flat_node_handle);
			if(	version==0||
				(doc_type==1&&version>=5010300)||	//TMA V5.1.3.0
				(doc_type==2&&version>=2000600)||	//LMA V2.0.6.0
				(doc_type==4&&version>=1030600)||	//LDS V1.3.6.0
				(doc_type==5&&version>=1030600))	//TDA V1.3.6.0
				buffer.WriteDouble(_mid_push_flat_up_offset);
		}
	}

	if( version==0||
		(doc_type==1&&version>=4010100)||	//TMA(V4.1.1.0)
		(doc_type==2&&version>=1010100)||	//LMA(V1.1.1.0)
		(doc_type==4&&version>=1020100)||	//LDS(V1.2.1.0)
		(doc_type==5&&version>=1020100))	//TDA(V1.2.1.0)
		buffer.WriteDouble(_fCutAngleCoefR);	//Ӧ�ü���,����������Ժ�汾�ѷ����ݲ�֪��δ���
	if( version==0||
		(doc_type==1&&version>=4010102)||	//TMA(V4.1.1.2)
		(doc_type==2&&version>=1010102)||	//LMA(V1.1.1.2)
		(doc_type==3&&version>=2000100)||	//TSA(V2.0.1.0)
		(doc_type==4&&version>=1020100)||	//LDS(V1.2.1.0)
		(doc_type==5&&version>=1020100))	//TDA(V1.2.1.0)
		buffer.WriteWord(_nCutAngleSpaceOdd);	//�Ǹ��н�ʱ����ȥ����Բ���뾶ϵ��֮�����ͷ��϶

	if (version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
		(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
		(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
		(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
		(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
	{
		buffer.WriteBooleanThin(_blOddByBoltCanAcrossMiddleSect);
		buffer.WriteBooleanThin(_blScatterBoltLocaTolerance);
	}
	//����Ǹֵ���Բ������
	if(	version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2.0
		(doc_type==2&&version>=2010100)||	//LMA V2,1,1,0
		(doc_type==3&&version>=2010400)||	//TSA V2,1,4,0
		(doc_type==4&&version>=1030900)||	//LDS V1,3,9,0
		(doc_type==5&&version>=1030900)))	//TDA V1,3,9,0
	{
		buffer.WriteDouble(_dfFilletROfSX);
		buffer.WriteDouble(_dfFilletROfSY);
		buffer.WriteDouble(_dfFilletROfEX);
		buffer.WriteDouble(_dfFilletROfEY);
		//д����
		BYTE ciIndex,cnCount=(BYTE)xarrOpenCuts.Count;
		buffer.WriteByte(cnCount);
		for(ciIndex=0;ciIndex<cnCount;ciIndex++)
		{
			OPENING_CUT* pCut=xarrOpenCuts.GetAt(ciIndex);
			buffer.WriteDouble(pCut->dfLateralBtmXorY);
			buffer.WriteDouble(pCut->dfLenPosFrom);
			buffer.WriteDouble(pCut->dfLenPosTo);
			buffer.WriteDouble(pCut->dfTipDeltaS);
			buffer.WriteDouble(pCut->dfTipDeltaE);
			buffer.WriteWord(pCut->siCornerFilletR);
		}
		//д��ֲ����Ͻ�
		cnCount=(BYTE)xarrKaiheSects.Count;
		buffer.WriteByte(cnCount);
		for(ciIndex=0;ciIndex<cnCount;ciIndex++)
		{
			KAIHE_SECTION* pSect=xarrKaiheSects.GetAt(ciIndex);
			buffer.WriteByte(pSect->ciOperWing);
			buffer.WriteDouble(pSect->dfFromX);
			buffer.WriteDouble(pSect->dfToX);
			buffer.WriteDouble(pSect->dfReservedSizeY);
			buffer.WriteDouble(pSect->dfOpenAngle);
		}
	}
	buffer.WriteInteger(group_father_jg_h);
	//if(iSeg<0)
	//	iSeg=0;
	buffer.WriteInteger(iSeg);
	buffer.WriteString(sPartNo);	//ԭʼ�������
	buffer.WriteString(layer());					// ���Ǹ�����ͼ��.
	// --------- 1.2. �н� -----------
	buffer.WriteInteger(m_bUserSpecStartCutAngle);
	buffer.WriteInteger(m_bUserSpecEndCutAngle);
	/*cut_wing_para[0]��ʾʼ����֫cut_wing_para[1]��ʾ�ն���֫
	...[][0]��ʾX����Ƥ��ȡ����
	...[][1]��ʾY����Ƥ��ȡ����
	...[][2]��ʾZ����Ƥ��ȡ����
	*/
	buffer.Write(cut_wing_para,6*sizeof(double));
	//������֫����֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
	buffer.WriteInteger(cut_wing[0]);
	buffer.WriteInteger(cut_wing[1]);
// --------- 1.3. ��֫ -----------
	/*
	cut_angle[0]��ʾ���X֫
	cut_angle[1]��ʾ���Y֫
	cut_angle[2]��ʾ�յ�X֫
	cut_angle[3]��ʾ�յ�Y֫
	...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
	*/
	buffer.Write(cut_angle,8*sizeof(double));
	// --------- 1.4. ���������-----------
	buffer.WriteInteger(m_bCutRoot);
	buffer.WriteInteger(m_bCutBer);
	buffer.WriteInteger(m_bWeldPart);
	buffer.WriteWord(m_nCutBerR);
	
	buffer.WriteDouble(baseline.Start().x);// ʵ���������
	buffer.WriteDouble(baseline.Start().y);// ʵ���������
	buffer.WriteDouble(baseline.Start().z);// ʵ���������
	buffer.WriteDouble(baseline.End().x);	// ʵ�������յ�
	buffer.WriteDouble(baseline.End().y);	// ʵ�������յ�
	buffer.WriteDouble(baseline.End().z);	// ʵ�������յ�
#ifdef __COMMON_PART_INC_
	if(  version==0||
		(doc_type==1&&version>=4020001)||	//TMA V4.2.0.1
		(doc_type==2&&version>=1020001)||	//LMA V1.2.0.1
		//(doc_type==3&&version>=1080001)||	//TSA V1.8.0.1
		(doc_type==4&&version>=1020001)||	//LDS V1.2.0.1
		(doc_type==5&&version>=1020001))	//TDA V1.2.0.1
	{
	buffer.WriteInteger(m_hArcLift);
	buffer.WritePoint(m_xStartBeforeLift);
	buffer.WritePoint(m_xEndBeforeLift);
	}
#endif
	//ʼ����ͷ�������
	buffer.WriteDouble(start_oddment);
	buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
	buffer.WriteInteger(desStartOdd.m_hRefPart1);
	buffer.WriteInteger(desStartOdd.m_hRefPart2);
	buffer.WriteDouble(desStartOdd.m_fCollideDist);
	//�洢�Ǹ�ʼ�˶���(�̶�)����ͷ
	buffer.WriteByte(extraStartOdd.ber.style);
	if(extraStartOdd.ber.style==0)
		buffer.WriteDouble(extraStartOdd.ber.odd);
	else
		extraStartOdd.ber.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(extraStartOdd.wing_x.style);
	if(extraStartOdd.wing_x.style==0)
		buffer.WriteDouble(extraStartOdd.wing_x.odd);
	else
		extraStartOdd.wing_x.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(extraStartOdd.wing_y.style);
	if(extraStartOdd.wing_y.style==0)
		buffer.WriteDouble(extraStartOdd.wing_y.odd);
	else
		extraStartOdd.wing_y.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	//�ն���ͷ�������
	buffer.WriteDouble(end_oddment);   // �Ǹ�ʼ�ն�����ͷ.
	buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
	buffer.WriteInteger(desEndOdd.m_hRefPart1);
	buffer.WriteInteger(desEndOdd.m_hRefPart2);
	buffer.WriteDouble(desEndOdd.m_fCollideDist);
	//�洢�Ǹ��ն˶���(�̶�)����ͷ
	buffer.WriteByte(extraEndOdd.ber.style);
	if(extraEndOdd.ber.style==0)
		buffer.WriteDouble(extraEndOdd.ber.odd);
	else
		extraEndOdd.ber.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(extraEndOdd.wing_x.style);
	if(extraEndOdd.wing_x.style==0)
		buffer.WriteDouble(extraEndOdd.wing_x.odd);
	else
		extraEndOdd.wing_x.extraDatumPoint.ToBuffer(buffer,version,doc_type);
	buffer.WriteByte(extraEndOdd.wing_y.style);
	if(extraEndOdd.wing_y.style==0)
		buffer.WriteDouble(extraEndOdd.wing_y.odd);
	else
		extraEndOdd.wing_y.extraDatumPoint.ToBuffer(buffer,version,doc_type);

	connectStart.ToBuffer(buffer,version,doc_type);   			//(��ˮƽ�Ǹֵ��϶˻�ˮƽ�Ǹֵ�)��ʼ����˨��Ϣ
	connectEnd.ToBuffer(buffer,version,doc_type);				//(��ˮƽ�Ǹֵ��¶˻�ˮƽ�Ǹֵ�)ĩ��  ��˨��Ϣ
	if( version==0||
		(doc_type==1&&version>=4020101)||	//TMA
		(doc_type==2&&version>=1020101)||	//LMA
		(doc_type==5&&version>=1020101)||	//TDA
		(doc_type==4&&version>=1020101))	//LDS
	{
		buffer.WriteInteger(connectStart.idAngleJoint);
		buffer.WriteInteger(connectEnd.idAngleJoint);
	}
	pLsRefList->ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(feature);		//����(������Ϊ���Ǹֱ�־�û���ʱ�Զ��������)
	buffer.WriteBooleanThin(m_bDisplayHole);//����ʾ��˨��

	//�����Ϣ
	buffer.WriteInteger(des_norm_x.bByOtherWing);
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
		buffer.WriteInteger(des_norm_x.bSpecific);
	else
		buffer.WriteInteger(des_norm_x.IsSpecNormActual());
	buffer.WriteDouble(des_norm_x.near_norm.x);	// 
	buffer.WriteDouble(des_norm_x.near_norm.y);	// 
	buffer.WriteDouble(des_norm_x.near_norm.z);	// 
	des_norm_x.spec_norm.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(des_norm_x.hViceJg);
	buffer.WriteInteger(des_norm_y.bByOtherWing);
	buffer.WriteInteger(des_norm_y.bSpecific);
	buffer.WriteDouble(des_norm_y.near_norm.x);	// 
	buffer.WriteDouble(des_norm_y.near_norm.y);	// 
	buffer.WriteDouble(des_norm_y.near_norm.z);	// 
	des_norm_y.spec_norm.ToBuffer(buffer,version,doc_type);
	buffer.WriteInteger(des_norm_y.hViceJg);
	buffer.WriteByte(m_cPosCtrlDatumLineType);
	buffer.WriteByte(m_cPosCtrlType);
	BYTE cDatumAngleWing=desEndPos.cDatumAngleWing;	//���汾�洢ʱ���ܻ�Ķ�cDatumAngleWing���ԣ��洢��Ϻ���븴ԭ��wjh-2016.11.28
	if((version>0&&(doc_type==1&&version<5000000)||(doc_type==2&&version<2000000))&&
		desEndPos.spatialOperationStyle>0&&desEndPos.spatialOperationStyle<15&&desEndPos.IsFaceOffset())
	{	//TMA V4.1�д���BUG,������desEndPos.cDatumAngleWing����'X'��'Y'�����ն�ƽ��λ�ü������GetEndInitPos wjh-2016.4.26
		CLDSLinePart* pEndDatumRod=GetEndDatumPart();
		if( pEndDatumRod&&pEndDatumRod->IsAngle()&&desEndPos.cDatumAngleWing==0&&(
			((CLDSLineAngle*)pEndDatumRod)->m_cPosCtrlDatumLineType=='X'||((CLDSLineAngle*)pEndDatumRod)->m_cPosCtrlDatumLineType=='Y'))
			desEndPos.cDatumAngleWing=((CLDSLineAngle*)pEndDatumRod)->m_cPosCtrlDatumLineType;	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	}
	if(  version==0||
		(doc_type==1&&version>=4020001)||	//TMA V4.2.0.1
		(doc_type==2&&version>=1020001)||	//LMA V1.2.0.1
		//(doc_type==3&&version>=1080001)||	//TSA V1.8.0.1
		(doc_type==4&&version>=1020001)||	//LDS V1.2.0.1
		(doc_type==5&&version>=1020001))	//TDA V1.2.0.1
	{
		BOOL lockStartTemp=m_bLockStartPos,lockEndTemp=m_bLockEndPos;
		if( version>0&&desStartPos.spatialOperationStyle==15&&
			(doc_type==1&&version<5010200)||	//TMA(V5,1,2,0)
			(doc_type==2&&version<2000502)||	//LMA(V2,0,5,2)
			(doc_type==4&&version<1030502)||	//LDS(V1,3,5,2)
			(doc_type==5&&version<1030502))		//TDA(V1,3,5,2)
		{	//����TMAV5.1.2.0�濪ʼ��һ֫ƽ��һ֫ͶӰʱ�ļ����㷨�����仯����ת�Ͱ汾ʱ������������ wjh-2016.4.28
			if(desStartPos.ctrlWing.operStyle==0&&desStartPos.otherWing.operStyle!=0)
				lockStartTemp=TRUE;
			else if(desStartPos.ctrlWing.operStyle!=0&&desStartPos.otherWing.operStyle==0)
				lockStartTemp=TRUE;
		}
		if( version>0&&desEndPos.spatialOperationStyle==15&&
			(doc_type==1&&version<5010200)||	//TMA(V5,1,2,0)
			(doc_type==2&&version<2000502)||	//LMA(V2,0,5,2)
			(doc_type==4&&version<1030502)||	//LDS(V1,3,5,2)
			(doc_type==5&&version<1030502))		//TDA(V1,3,5,2)
		{	//����TMAV5.1.2.0�濪ʼ��һ֫ƽ��һ֫ͶӰʱ�ļ����㷨�����仯����ת�Ͱ汾ʱ������������ wjh-2016.4.28
			if(desEndPos.ctrlWing.operStyle==0&&desEndPos.otherWing.operStyle!=0)
				lockEndTemp=TRUE;
			else if(desEndPos.ctrlWing.operStyle!=0&&desEndPos.otherWing.operStyle==0)
				lockEndTemp=TRUE;
		}
		desStartPos.ToBuffer(buffer,version,doc_type);
		buffer.WriteInteger(lockStartTemp);
		desEndPos.ToBuffer(buffer,version,doc_type);
		desEndPos.cDatumAngleWing=cDatumAngleWing;	//��ԭcDatumAngleWing
		buffer.WriteInteger(lockEndTemp);
	}
	else
	{	//Ԥ����洢Ϊ�Ͱ汾ʱ�������������꣬����λ���ܵ�
		BOOL bLockStartPos=m_bLockStartPos,bLockEndPos=m_bLockEndPos;
		if( (doc_type==1&&version>0&&version<5010300)||	//TMA V5.1.3.0
			(doc_type==2&&version>0&&version<2000600)||	//LMA V2.0.6.0
			(doc_type==4&&version>0&&version<1030600)||	//LDS V1.3.6.0
			(doc_type==5&&version>0&&version<1030600))		//TDA V1.3.6.0
		{	//�ɰ汾��֧�ֽǸ��޶˽ڵ��Ҵ���ָ������ʱ��������֫����ƫ�ơ�wjh-2016.9.7
			if( pStart==NULL&&desStartPos.spatialOperationStyle==0&&(
				desStartPos.wing_x_offset.gStyle!=4    ||desStartPos.wing_y_offset.gStyle!=4||
				desStartPos.wing_x_offset.offsetDist!=0||desStartPos.wing_y_offset.offsetDist!=0))
				bLockStartPos=TRUE;	//�ɰ汾��֧��
			if( pEnd==NULL&&desEndPos.spatialOperationStyle==0&&(
				desEndPos.wing_x_offset.gStyle!=4    ||desEndPos.wing_y_offset.gStyle!=4||
				desEndPos.wing_x_offset.offsetDist!=0||desEndPos.wing_y_offset.offsetDist!=0))
				bLockEndPos=TRUE;	//�ɰ汾��֧��
		}
		desStartPos.ToBuffer(buffer,version,doc_type);
		if(m_hArcLift>0x20||desStartPos.len_offset_dist!=0)
			buffer.WriteInteger(TRUE);//m_bLockStartPos);
		else
			buffer.WriteInteger(bLockStartPos);
		desEndPos.ToBuffer(buffer,version,doc_type);
		if(m_hArcLift>0x20||desEndPos.len_offset_dist!=0)
			buffer.WriteInteger(TRUE);//m_bLockEndPos);
		else
			buffer.WriteInteger(bLockEndPos);
	}
	buffer.WriteDouble(huoqu_r);
	buffer.WriteInteger(huoqu_handle);
	buffer.WriteString(sNotes);
#if defined(__LDS_)||defined(__LDS_FILE_)
	if(doc_type==3||doc_type==4)
	{
	buffer.WriteInteger(m_bAutoCalHoleOutCoef);	//�Զ�������˨������������Ϊָ����˨������
	buffer.WriteDouble(hole_out);			//�˼��������˨������
	buffer.WriteInteger(start_force_type);
	buffer.WriteInteger(end_force_type);
	buffer.WriteInteger(start_joint_type);
	buffer.WriteInteger(end_joint_type);
	buffer.WriteInteger(CalLenCoef.iSelRFlag);
	buffer.WriteInteger(CalLenCoef.iTypeNo);
	buffer.WriteInteger(CalLenCoef.hHorizPole);
	buffer.WriteInteger(CalLenCoef.hRefPole);
	buffer.WriteInteger(CalLenCoef.minR.hStartNode);
	buffer.WriteInteger(CalLenCoef.minR.hEndNode);
	buffer.WriteDouble(CalLenCoef.minR.coef);
	buffer.WriteInteger(CalLenCoef.paraR.hStartNode);
	buffer.WriteInteger(CalLenCoef.paraR.hEndNode);
	buffer.WriteDouble(CalLenCoef.paraR.coef);
	buffer.WriteWord(m_iElemType);			//0��ʾ�Զ��ж�;1��ʾ���������;2��ʾ��������Ԫ;3��ʾ������Ԫ
	buffer.WriteDword(m_uStatMatNo);		//ͳ�����
	buffer.WriteInteger(m_bNeedSelSize);	//��Ҫѡ����
	buffer.WriteInteger(m_bNeedSelMat);		//��Ҫѡ�����
	buffer.WriteInteger(sel_mat_method);	//����ѡ�ķ�ʽ
	buffer.WriteDouble(bear_load_coef);		//������֧����֧�Ÿ˼�������
	buffer.WriteDouble(m_fWeightRaiseCoef);	//�����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
	if(	version==0|| (doc_type==4&&version>=1030800))	//LDS V1.3.8.0
		buffer.WriteByte(m_cbRodFlag);
	}
#endif
	//���������б�
	RELATIVE_OBJECT *pRelaObj=NULL;
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pRelaObj->hObj);
		buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
		bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
		buffer.WriteBooleanThin(bSpecOrigin);
		if(bSpecOrigin)
			buffer.WritePoint(pRelaObj->mirInfo.origin);
		if(pRelaObj->mirInfo.axis_flag&0x08)
		{
			buffer.WriteByte(pRelaObj->mirInfo.array_num);
			buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of son view is wrong!");
#else
		CLDSObject::Log2File()->Log("����ͼ��¼�����ִ���!");
#endif

	buffer.WriteInteger(m_bFootNail);
}
void CLDSLineAngle::FromCdfBuffer(CBuffer &buffer,long version/*=0*/,bool bReadForCheck/*=false*/)
{
	CLDSLinePart::FromCdfBuffer(buffer,version,bReadForCheck);
	buffer.ReadDword(&dwLayStyle);
	buffer.ReadPoint(norm_x_wing);	// ������֫���߷���.
	buffer.ReadPoint(norm_y_wing);	// ��һ֫���߷���.
	buffer.ReadInteger(&group_father_jg_h);
	GEPOINT posStart,posEnd;
	buffer.ReadPoint(posStart);
	buffer.ReadPoint(posEnd);
	baseline.SetStart(posStart);//ʵ���������
	baseline.SetEnd(posEnd);	//ʵ�������յ�
	buffer.ReadInteger(&connectStart.idAngleJoint);
	buffer.ReadInteger(&connectEnd.idAngleJoint);

	//�����Ϣ
	buffer.ReadInteger(&des_norm_x.bByOtherWing);
	buffer.ReadInteger(&des_norm_x.bSpecific);
	buffer.ReadPoint(des_norm_x.near_norm);	// 
	des_norm_x.spec_norm.FromBuffer(buffer);//,version,doc_type);
	buffer.ReadInteger(&des_norm_x.hViceJg);

	buffer.ReadInteger(&des_norm_y.bByOtherWing);
	buffer.ReadInteger(&des_norm_y.bSpecific);
	buffer.ReadPoint(des_norm_y.near_norm);	// 
	des_norm_y.spec_norm.FromBuffer(buffer);//,version,doc_type);
	buffer.ReadInteger(&des_norm_y.hViceJg);
	buffer.ReadByte(&m_cPosCtrlDatumLineType);
	buffer.ReadByte(&m_cPosCtrlType);
	desStartPos.FromBuffer(buffer);//,version,doc_type);
	desEndPos.FromBuffer(buffer);//,version,doc_type);
}
void CLDSLineAngle::ToCdfBuffer(CBuffer &buffer,long version/*=0*/)
{
	CLDSLinePart::ToCdfBuffer(buffer,version);
	buffer.WriteDword(dwLayStyle);
	buffer.WriteDouble(norm_x_wing.x);	// ������֫���߷���.
	buffer.WriteDouble(norm_x_wing.y);	// ������֫���߷���.
	buffer.WriteDouble(norm_x_wing.z);	// ������֫���߷���.
	buffer.WriteDouble(norm_y_wing.x);	// ��һ֫���߷���.
	buffer.WriteDouble(norm_y_wing.y);	// ��һ֫���߷���.
	buffer.WriteDouble(norm_y_wing.z);	// ��һ֫���߷���.
	buffer.WriteInteger(group_father_jg_h);
	buffer.WriteDouble(baseline.Start().x);// ʵ���������
	buffer.WriteDouble(baseline.Start().y);// ʵ���������
	buffer.WriteDouble(baseline.Start().z);// ʵ���������
	buffer.WriteDouble(baseline.End().x);	// ʵ�������յ�
	buffer.WriteDouble(baseline.End().y);	// ʵ�������յ�
	buffer.WriteDouble(baseline.End().z);	// ʵ�������յ�
	//connectStart.ToBuffer(buffer,version,doc_type);   			//(��ˮƽ�Ǹֵ��϶˻�ˮƽ�Ǹֵ�)��ʼ����˨��Ϣ
	//connectEnd.ToBuffer(buffer,version,doc_type);				//(��ˮƽ�Ǹֵ��¶˻�ˮƽ�Ǹֵ�)ĩ��  ��˨��Ϣ
	buffer.WriteInteger(connectStart.idAngleJoint);
	buffer.WriteInteger(connectEnd.idAngleJoint);

	//�����Ϣ
	buffer.WriteInteger(des_norm_x.bByOtherWing);
	buffer.WriteInteger(des_norm_x.bSpecific);
	buffer.WriteDouble(des_norm_x.near_norm.x);	// 
	buffer.WriteDouble(des_norm_x.near_norm.y);	// 
	buffer.WriteDouble(des_norm_x.near_norm.z);	// 
	des_norm_x.spec_norm.ToBuffer(buffer);//,version,doc_type);
	buffer.WriteInteger(des_norm_x.hViceJg);
	buffer.WriteInteger(des_norm_y.bByOtherWing);
	buffer.WriteInteger(des_norm_y.bSpecific);
	buffer.WriteDouble(des_norm_y.near_norm.x);	// 
	buffer.WriteDouble(des_norm_y.near_norm.y);	// 
	buffer.WriteDouble(des_norm_y.near_norm.z);	// 
	des_norm_y.spec_norm.ToBuffer(buffer);//,version,doc_type);
	buffer.WriteInteger(des_norm_y.hViceJg);
	buffer.WriteByte(m_cPosCtrlDatumLineType);
	buffer.WriteByte(m_cPosCtrlType);
	desStartPos.ToBuffer(buffer);//,version,doc_type);
	desEndPos.ToBuffer(buffer);//,version,doc_type);
}

void CLDSLineAngle::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//�ѵ��ù�)
		return;
	if(!_console->AddToObjSetBuf(this))
		return;
	if(!bAddRelaObj)
		return;
	bAddRelaObj=bAddSubLevelRelaObj;
	if(pStart)
		pStart->AddToConsoleObjBuf(bAddRelaObj);
	if(pEnd)
		pEnd->AddToConsoleObjBuf(bAddRelaObj);
	if(desStartPos.spatialOperationStyle==0&&desStartPos.datumPoint.datum_pos_style==5)
	{
		CLDSLine* pLine=BelongModel()->FromLineHandle(desStartPos.datumPoint.des_para.DATUMLINE.hDatumLine);
		if(pLine)
			pLine->AddToConsoleObjBuf(bAddRelaObj,false);
	}
	else if(desStartPos.spatialOperationStyle==0&&desStartPos.datumPoint.datum_pos_style==6)
	{
		CLDSPlane* pPlane=BelongModel()->FromPlaneHandle(desStartPos.datumPoint.des_para.DATUMPLANE.hDatumPlane);
		if(pPlane)
			pPlane->AddToConsoleObjBuf(bAddRelaObj,false);
	}
	else if(IsLappedAnchorStart()&&desStartPos.datum_jg_h>0x20)
	{
		CLDSLinePart* pRod=BelongModel()->FromRodHandle(desStartPos.datum_jg_h);
		if(pRod!=NULL)
			pRod->AddToConsoleObjBuf(bAddRelaObj);
	}
	if(desEndPos.spatialOperationStyle==0&&desEndPos.datumPoint.datum_pos_style==5)
	{
		CLDSLine* pLine=BelongModel()->FromLineHandle(desEndPos.datumPoint.des_para.DATUMLINE.hDatumLine);
		if(pLine)
			pLine->AddToConsoleObjBuf(bAddRelaObj,false);
	}
	else if(desEndPos.spatialOperationStyle==0&&desEndPos.datumPoint.datum_pos_style==6)
	{
		CLDSPlane* pPlane=BelongModel()->FromPlaneHandle(desEndPos.datumPoint.des_para.DATUMPLANE.hDatumPlane);
		if(pPlane)
			pPlane->AddToConsoleObjBuf(bAddRelaObj,false);
	}
	else if(IsLappedAnchorEnd()&&desEndPos.datum_jg_h>0x20)
	{
		CLDSLinePart* pRod=BelongModel()->FromRodHandle(desEndPos.datum_jg_h);
		if(pRod!=NULL)
			pRod->AddToConsoleObjBuf(bAddRelaObj);
	}
	if(des_norm_x.IsByViceRodActual()&&des_norm_x.hViceJg>0x20)
	{
		CLDSLinePart* pRod=BelongModel()->FromRodHandle(des_norm_x.hViceJg);
		if(pRod!=NULL)
			pRod->AddToConsoleObjBuf(bAddRelaObj);
	}
	if(des_norm_y.IsByViceRodActual()&&des_norm_y.hViceJg>0x20)
	{
		CLDSLinePart* pRod=BelongModel()->FromRodHandle(des_norm_y.hViceJg);
		if(pRod!=NULL)
			pRod->AddToConsoleObjBuf(bAddRelaObj);
	}
	if(GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)this;
		for(int i=0;i<4;i++)
		{
			if(pGroupAngle->son_jg_h[i]<=0x20)
				continue;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pAngle!=NULL)
				pAngle->AddToConsoleObjBuf(bAddRelaObj);
		}
	}
	else if(group_father_jg_h>0)
	{
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(group_father_jg_h,CLS_GROUPLINEANGLE);
		if(pGroupAngle)
			pGroupAngle->AddToConsoleObjBuf(bAddRelaObj);
	}
	for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		pLsRef->GetLsPtr()->AddToConsoleObjBuf(bAddRelaObj);
}

//��Эͬ����ͨ���ܵ���ȡ����
void CLDSLineAngle::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer))
	{
		IndependentShadowPropFromBuffer(buffer);
		return;
	}
	long h=0;
	int i=0,n=0;
	buffer.ReadDword(&dwPermission);
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	mpcl.FromBuffer(&buffer);
	buffer.ReadDword(&dwLayStyle);
	//�Ǹֻ�����Ϣ
	if(dwPermission&dwUpdateWord)
	{
		buffer.ReadString(layer(),4);
		buffer.Read(cfgword.flag.word,24);
		buffer.ReadInteger(&m_uStatMatCoef);
		buffer.ReadInteger(&_hPartWeldParent);
		buffer.ReadByte(&_cFuncType);
		buffer.ReadBooleanThin(&m_bUserSpecColor);
		buffer.ReadDword(&crMaterial);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.ReadInteger(&_uStatMatNo);
#endif
		buffer.ReadPoint(norm_x_wing);
		buffer.ReadPoint(norm_y_wing);
		buffer.ReadBooleanThin(&_bVirtualPart);			//	�����Ǹ�
		buffer.ReadDouble(&nominal_length);
		buffer.ReadByte(&m_cHeJiaoReservedWidthType);
		buffer.ReadDouble(&m_fHeJiaoReservedWidth);
		buffer.ReadInteger(&kaihe_base_wing_x0_y1);
		buffer.ReadInteger(&huoqu_base_wing_x0_y1_both2);
		buffer.ReadInteger(&huoqu_start1_end2);
		buffer.ReadByte(&m_cPosCtrlDatumLineType);
		buffer.ReadByte(&m_cPosCtrlType);
		//�Ǹ�������������
		buffer.ReadInteger(&m_bEnableTeG);
		buffer.ReadInteger(&xWingXZhunJu.g);
		buffer.ReadInteger(&xWingXZhunJu.g1);
		buffer.ReadInteger(&xWingXZhunJu.g2);
		buffer.ReadInteger(&xWingXZhunJu.g3);
		buffer.ReadInteger(&xWingYZhunJu.g);
		buffer.ReadInteger(&xWingYZhunJu.g1);
		buffer.ReadInteger(&xWingYZhunJu.g2);
		buffer.ReadInteger(&xWingYZhunJu.g3);

#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.ReadInteger(&size.idClassType);
		buffer.ReadByte(&size.cSubClassType);
#endif
		double ff;
		buffer.ReadDouble(&ff);
		set_wing_wide(ff);
		buffer.ReadDouble(&ff);
		set_wing_thick(ff);
		buffer.ReadDouble(&ff);
		set_y_wing_width(ff);
		buffer.ReadByte(&_material);
		buffer.ReadByte(&_cQualityLevel);
		buffer.ReadInteger(&m_bReplPart);
		buffer.ReadByte(&org_material);
		buffer.ReadDouble(&org_wing_wide);
		buffer.ReadDouble(&org_wing_thick);
		_start_push_flat_x1_y2=(BYTE)buffer.ReadInteger();
		buffer.ReadDouble(&_start_push_flat_length);
		_end_push_flat_x1_y2=(BYTE)buffer.ReadInteger();
		buffer.ReadDouble(&_end_push_flat_length);
		buffer.ReadDouble(&_fCutAngleCoefR);
		buffer.ReadWord(&_nCutAngleSpaceOdd);	//�Ǹ��н�ʱ����ȥ����Բ���뾶ϵ��֮�����ͷ��϶
		///////////////////////////
		//��ȡ���
		BYTE ciIndex,cnCount=0;
		buffer.ReadByte(&cnCount);
		xarrOpenCuts.Clear();
		for(ciIndex=0;ciIndex<cnCount;ciIndex++)
		{
			OPENING_CUT cut;
			buffer.ReadDouble(&cut.dfLateralBtmXorY);
			buffer.ReadDouble(&cut.dfLenPosFrom);
			buffer.ReadDouble(&cut.dfLenPosTo);
			buffer.ReadDouble(&cut.dfTipDeltaS);
			buffer.ReadDouble(&cut.dfTipDeltaE);
			buffer.ReadWord(&cut.siCornerFilletR);
			xarrOpenCuts.Append(cut);
		}
		//д��ֲ����Ͻ�
		buffer.ReadByte(&cnCount);
		xarrKaiheSects.Clear();
		for(ciIndex=0;ciIndex<cnCount;ciIndex++)
		{
			KAIHE_SECTION sect;
			buffer.ReadByte(&sect.ciOperWing);
			buffer.ReadDouble(&sect.dfFromX);
			buffer.ReadDouble(&sect.dfToX);
			buffer.ReadDouble(&sect.dfReservedSizeY);
			buffer.ReadDouble(&sect.dfOpenAngle);
			xarrKaiheSects.Append(sect);
		}
		///////////////////////////

		buffer.ReadInteger(&group_father_jg_h);
		buffer.ReadInteger(&_iSeg.iSeg);
		buffer.ReadString(sPartNo);	//ԭʼ�������
		buffer.ReadInteger(&m_hArcLift);
		// --------- 1.4. ���������-----------
		_bCutRoot=(buffer.ReadInteger()!=0);
		_bCutBer =(buffer.ReadInteger()!=0);
		buffer.ReadInteger(&m_bWeldPart);
		buffer.ReadWord(&m_nCutBerR);
		//��ȡ�м�ѹ����ز��� wht 12-10-09
		_mid_push_flat_x1_y2=(BYTE)buffer.ReadInteger();
		if(_mid_push_flat_x1_y2>0)
		{
			buffer.ReadDouble(&_mid_push_flat_length);
			buffer.ReadInteger(&mid_push_flat_node_handle);
			buffer.ReadDouble(&_mid_push_flat_up_offset);
		}
		
		buffer.ReadInteger(&son_jg_h[0]);
		buffer.ReadInteger(&son_jg_h[1]);
		buffer.ReadBooleanThin(&m_bDisplayHole);		//����ʾ��˨��
		//�����Ϣ
		buffer.ReadInteger(&des_norm_x.bByOtherWing);
		buffer.ReadInteger(&des_norm_x.bSpecific);
		buffer.ReadPoint(des_norm_x.near_norm);
		des_norm_x.spec_norm.FromBuffer(buffer);
		buffer.ReadInteger(&des_norm_x.hViceJg);
		buffer.ReadInteger(&des_norm_y.bByOtherWing);
		buffer.ReadInteger(&des_norm_y.bSpecific);
		buffer.ReadPoint(des_norm_y.near_norm);
		des_norm_y.spec_norm.FromBuffer(buffer);
		buffer.ReadInteger(&des_norm_y.hViceJg);
		buffer.ReadDouble(&huoqu_r);
		buffer.ReadInteger(&huoqu_handle);
		buffer.ReadString(sNotes,51);
		//�м���˨��
		int nLs;
		buffer.ReadInteger(&nLs);
		CLsRef *pLsRef=NULL;
		for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			pLsRef->feature=0;
		for(int ii=0;ii<nLs;ii++)
		{
			THANDLE h;
			DWORD dwRayNo;
			buffer.ReadInteger(&h);
			pLsRef=FindLsByHandle(h);
			buffer.ReadDword(&dwRayNo);
			if(pLsRef==NULL)
			{
				CLDSBolt *pBolt=(CLDSBolt*)m_pModel->FromPartHandle(h,CLS_BOLT);
				if(pBolt)
					pLsRef=AppendMidLsRef(pBolt->GetLsRef(),FALSE);
			}
			if(pLsRef)
			{
				pLsRef->feature=1;
				pLsRef->start1_end2_mid0=0;
				pLsRef->dwRayNo=dwRayNo;
			}
		}
		for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pLsRef->start1_end2_mid0==0&&pLsRef->feature==0)
				GetLsRefList()->DeleteCursor();
		}
		//���������б�
		buffer.ReadInteger(&n);
		RELATIVE_OBJECT *pRelaObj=NULL;
		relativeObjs.Empty();
		for(i=0;i<n;i++)
		{
			pRelaObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin;
			buffer.ReadBooleanThin(&bSpecOrigin);
			if(bSpecOrigin)
				buffer.ReadPoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.ReadByte(&pRelaObj->mirInfo.array_num);
				buffer.ReadWord(&pRelaObj->mirInfo.rotate_angle);
			}
		}
		_bFootNail=(buffer.ReadInteger()!=0);	
		buffer.ReadBooleanThin(&_blOddByBoltCanAcrossMiddleSect);
		buffer.ReadBooleanThin(&_blScatterBoltLocaTolerance);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.ReadInteger(&m_bAutoCalHoleOutCoef);	//�Զ�������˨������������Ϊָ����˨������
		buffer.ReadDouble(&hole_out);				//�˼��������˨������
		//
		//buffer.ReadInteger(&CalLenCoef.iSelRFlag);	//(������ʵ������)
		buffer.ReadInteger(&CalLenCoef.iTypeNo);
		buffer.ReadInteger(&CalLenCoef.hHorizPole);
		buffer.ReadInteger(&CalLenCoef.hRefPole);
		buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.minR.coef);
		buffer.ReadInteger(&CalLenCoef.paraR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.paraR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.paraR.coef);
		buffer.ReadWord(&m_iElemType);			//0��ʾ�Զ��ж�;1��ʾ���������;2��ʾ��������Ԫ;3��ʾ������Ԫ
		buffer.ReadDword(&_uStatMatNo);			//ͳ�����
		buffer.ReadInteger(&m_bNeedSelSize);	//��Ҫѡ����
		buffer.ReadInteger(&m_bNeedSelMat);		//��Ҫѡ�����
		buffer.ReadInteger(&h);					//����ѡ�ķ�ʽ
		sel_mat_method=AUX_POLE_SEL_METHOD(h);
		buffer.ReadDouble(&bear_load_coef);		//������֧����֧�Ÿ˼�������
		buffer.ReadDouble(&m_fWeightRaiseCoef);	//�����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
		buffer.ReadByte(&m_cbRodFlag);
#endif
	}
	if(dwStartPermission&dwUpdateWord)
	{
		//�Ǹ�ʼ�˻�����Ϣ
		buffer.ReadInteger(&h);
		//if(m_pModel->hashObjs.GetHashTableSize()>0)
		//	m_pModel->hashObjs.GetValueAt(h,(CLDSObject*&)pStart);
		//else
			pStart=m_pModel->FromNodeHandle(h);	// �����
		buffer.ReadInteger(&bHuoQuStart);
		// --------- 1.2. �н� -----------
		buffer.ReadInteger(&m_bUserSpecStartCutAngle);
		/*cut_wing_para[0]��ʾʼ����֫cut_wing_para[1]��ʾ�ն���֫
		...[][0]��ʾX����Ƥ��ȡ����
		...[][1]��ʾY����Ƥ��ȡ����
		...[][2]��ʾZ����Ƥ��ȡ����
		*/
		buffer.ReadDouble(&cut_wing_para[0][0]);
		buffer.ReadDouble(&cut_wing_para[0][1]);
		buffer.ReadDouble(&cut_wing_para[0][2]);
		//������֫����֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
		buffer.ReadInteger(&cut_wing[0]);
		// --------- 1.3. ��֫ -----------
		/*
		cut_angle[0]��ʾ���X֫
		cut_angle[1]��ʾ���Y֫
		cut_angle[2]��ʾ�յ�X֫
		cut_angle[3]��ʾ�յ�Y֫
		...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
		*/
		buffer.ReadDouble(&cut_angle[0][0]);
		buffer.ReadDouble(&cut_angle[0][1]);
		buffer.ReadDouble(&cut_angle[1][0]);
		buffer.ReadDouble(&cut_angle[1][1]);
		//�Ǹֵ�Բ����Ϣ
		buffer.ReadDouble(&_dfFilletROfSX);
		buffer.ReadDouble(&_dfFilletROfSY);
		f3dPoint pt;
		buffer.ReadPoint(pt);
		UnlockStartPos();
		baseline.SetStart(pt);  	// ʵ���������
		buffer.ReadPoint(m_xStartBeforeLift);
		buffer.ReadDouble(&start_oddment);
		buffer.ReadInteger(&desStartOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desStartOdd.m_hRefPart1);
		buffer.ReadInteger(&desStartOdd.m_hRefPart2);
		buffer.ReadDouble(&desStartOdd.m_fCollideDist);
		//�洢�Ǹ�ʼ�˶���(�̶�)����ͷ
		buffer.ReadByte(&extraStartOdd.ber.style);
		if(extraStartOdd.ber.style==0)
			buffer.ReadDouble(&extraStartOdd.ber.odd);
		else
			extraStartOdd.ber.extraDatumPoint.FromBuffer(buffer,0);
		buffer.ReadByte(&extraStartOdd.wing_x.style);
		if(extraStartOdd.wing_x.style==0)
			buffer.ReadDouble(&extraStartOdd.wing_x.odd);
		else
			extraStartOdd.wing_x.extraDatumPoint.FromBuffer(buffer,0);
		buffer.ReadByte(&extraStartOdd.wing_y.style);
		if(extraStartOdd.wing_y.style==0)
			buffer.ReadDouble(&extraStartOdd.wing_y.odd);
		else
			extraStartOdd.wing_y.extraDatumPoint.FromBuffer(buffer,0);
		BOOL lock;
		buffer.ReadInteger(&lock);
		if(lock)
			LockStartPos();
		else
			UnlockStartPos();
		buffer.ReadPoint(pre_huoqu_start_xyz);
		desStartPos.FromBuffer(buffer);
		buffer.ReadInteger(&connectStart.d);   	//(��ˮƽ�Ǹֵ��϶˻�ˮƽ�Ǹֵ�)��ʼ����˨��Ϣ
		//(doc_type==1&&version>=4010300)||	//TMA(V4,1,3,0)
		//(doc_type==2&&version>=1010300)||	//LMA(V1,1,3,0)
		//(doc_type==4&&version>=1030002)||	//LDS(V1,3,0,2)
		//(doc_type==5&&version>=1030002))	//TDA(V1,3,0,2)
		{
			buffer.ReadByte(&connectStart.m_iConnectWing);
			buffer.ReadByte(&connectStart.m_iConnectType);
		}
		buffer.Read(&connectStart.uiLegacyN,4);
		buffer.ReadInteger(&connectStart.rows);
		buffer.ReadString(connectStart.grade);	//��˨���� 
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.ReadDouble(&connectStart.maxTension);
		buffer.ReadDouble(&connectStart.maxCompression);
		buffer.ReadDouble(&connectStart.maxMoment);
		WORD w;
		buffer.ReadWord(&w);
		if(w==JOINT_HINGE)
			start_joint_type=JOINT_HINGE;
		else if(w==JOINT_RIGID)
			start_joint_type=JOINT_RIGID;
		else
			start_joint_type=JOINT_AUTOJUSTIFY;
		buffer.ReadWord(&w);
		if(w==CENTRIC_FORCE)
			start_force_type=CENTRIC_FORCE;
		else
			start_force_type=ECCENTRIC_FORCE;
#endif
		//ʼ����˨��
		int nLs;
		buffer.ReadInteger(&nLs);
		CLsRef *pLsRef=NULL;
		for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			pLsRef->feature=0;
		for(int ii=0;ii<nLs;ii++)
		{
			THANDLE h;
			DWORD dwRayNo;
			buffer.ReadInteger(&h);
			pLsRef=FindLsByHandle(h);
			buffer.ReadInteger(&dwRayNo);
			if(pLsRef==NULL)
			{
				CLDSBolt *pBolt=(CLDSBolt*)m_pModel->FromPartHandle(h,CLS_BOLT);
				if(pBolt)
					pLsRef=AppendStartLsRef(pBolt->GetLsRef(),FALSE);
			}
			if(pLsRef)
			{
				pLsRef->feature=1;
				pLsRef->start1_end2_mid0=1;
				pLsRef->dwRayNo=dwRayNo;
			}
		}
		for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pLsRef->start1_end2_mid0==1&&pLsRef->feature==0)
				GetLsRefList()->DeleteCursor();
		}
	}
	//�Ǹ��ն˻�����Ϣ
	if(dwEndPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&h);
		//if(m_pModel->hashObjs.GetHashTableSize()>0)
		//	m_pModel->hashObjs.GetValueAt(h,(CLDSObject*&)pEnd);
		//else
			pEnd=m_pModel->FromNodeHandle(h);	// �յ���
		buffer.ReadInteger(&bHuoQuEnd);
		// --------- 1.2. �н� -----------
		buffer.ReadInteger(&m_bUserSpecEndCutAngle);
		/*cut_wing_para[0]��ʾʼ����֫cut_wing_para[1]��ʾ�ն���֫
		...[][0]��ʾX����Ƥ��ȡ����
		...[][1]��ʾY����Ƥ��ȡ����
		...[][2]��ʾZ����Ƥ��ȡ����
		*/
		buffer.ReadDouble(&cut_wing_para[1][0]);
		buffer.ReadDouble(&cut_wing_para[1][1]);
		buffer.ReadDouble(&cut_wing_para[1][2]);
		//������֫����֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
		buffer.ReadInteger(&cut_wing[1]);
		// --------- 1.3. ��֫ -----------
		/*
		cut_angle[0]��ʾ���X֫
		cut_angle[1]��ʾ���Y֫
		cut_angle[2]��ʾ�յ�X֫
		cut_angle[3]��ʾ�յ�Y֫
		...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
		*/
		buffer.ReadDouble(&cut_angle[2][0]);
		buffer.ReadDouble(&cut_angle[2][1]);
		buffer.ReadDouble(&cut_angle[3][0]);
		buffer.ReadDouble(&cut_angle[3][1]);
		//�Ǹֵ�Բ����Ϣ
		buffer.ReadDouble(&_dfFilletROfEX);
		buffer.ReadDouble(&_dfFilletROfEY);
		f3dPoint pt;
		buffer.ReadPoint(pt);
		UnlockEndPos();
		baseline.SetEnd(pt);  	// ʵ�������յ�
		buffer.ReadPoint(m_xEndBeforeLift);
		buffer.ReadDouble(&end_oddment);
		buffer.ReadInteger(&desEndOdd.m_iOddCalStyle);
		buffer.ReadInteger(&desEndOdd.m_hRefPart1);
		buffer.ReadInteger(&desEndOdd.m_hRefPart2);
		buffer.ReadDouble(&desEndOdd.m_fCollideDist);
		//�洢�Ǹ��ն˶���(�̶�)����ͷ
		buffer.ReadByte(&extraEndOdd.ber.style);
		if(extraEndOdd.ber.style==0)
			buffer.ReadDouble(&extraEndOdd.ber.odd);
		else
			extraEndOdd.ber.extraDatumPoint.FromBuffer(buffer,0);
		buffer.ReadByte(&extraEndOdd.wing_x.style);
		if(extraEndOdd.wing_x.style==0)
			buffer.ReadDouble(&extraEndOdd.wing_x.odd);
		else
			extraEndOdd.wing_x.extraDatumPoint.FromBuffer(buffer,0);
		buffer.ReadByte(&extraEndOdd.wing_y.style);
		if(extraEndOdd.wing_y.style==0)
			buffer.ReadDouble(&extraEndOdd.wing_y.odd);
		else
			extraEndOdd.wing_y.extraDatumPoint.FromBuffer(buffer,0);
		BOOL lock;
		buffer.ReadInteger(&lock);
		if(lock)
			LockEndPos();
		else
			UnlockEndPos();
		buffer.ReadPoint(pre_huoqu_end_xyz);
		desEndPos.FromBuffer(buffer);
		buffer.ReadInteger(&connectEnd.d);   	//(��ˮƽ�Ǹֵ��϶˻�ˮƽ�Ǹֵ�)��ʼ����˨��Ϣ
		//(doc_type==1&&version>=4010300)||	//TMA(V4,1,3,0)
		//(doc_type==2&&version>=1010300)||	//LMA(V1,1,3,0)
		//(doc_type==4&&version>=1030002)||	//LDS(V1,3,0,2)
		//(doc_type==5&&version>=1030002))	//TDA(V1,3,0,2)
		{
			buffer.ReadByte(&connectEnd.m_iConnectWing);
			buffer.ReadByte(&connectEnd.m_iConnectType);
		}
		buffer.Read(&connectEnd.uiLegacyN,4);
		buffer.ReadInteger(&connectEnd.rows);
		buffer.ReadString(connectEnd.grade);	//��˨���� 
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.ReadDouble(&connectEnd.maxTension);
		buffer.ReadDouble(&connectEnd.maxCompression);
		buffer.ReadDouble(&connectEnd.maxMoment);
		WORD w;
		buffer.ReadWord(&w);
		if(w==JOINT_HINGE)
			end_joint_type=JOINT_HINGE;
		else if(w==JOINT_RIGID)
			end_joint_type=JOINT_RIGID;
		else
			end_joint_type=JOINT_AUTOJUSTIFY;
		buffer.ReadWord(&w);
		if(w==CENTRIC_FORCE)
			end_force_type=CENTRIC_FORCE;
		else
			end_force_type=ECCENTRIC_FORCE;
#endif
		//�ն���˨��
		int nLs;
		buffer.ReadInteger(&nLs);
		CLsRef *pLsRef=NULL;
		for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			pLsRef->feature=0;
		for(int ii=0;ii<nLs;ii++)
		{
			THANDLE h;
			DWORD dwRayNo;
			buffer.ReadInteger(&h);
			pLsRef=FindLsByHandle(h);
			buffer.ReadDword(&dwRayNo);
			if(pLsRef==NULL)
			{
				CLDSBolt *pBolt=(CLDSBolt*)m_pModel->FromPartHandle(h,CLS_BOLT);
				if(pBolt)
					pLsRef=AppendEndLsRef(pBolt->GetLsRef(),FALSE);
			}
			if(pLsRef)
			{
				pLsRef->feature=1;
				pLsRef->start1_end2_mid0=2;
				pLsRef->dwRayNo=dwRayNo;
			}
		}
		for(pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pLsRef->start1_end2_mid0==2&&pLsRef->feature==0)
				pLsRefList->DeleteCursor();
		}
	}
	pLsRefList->Clean();
	SetModified();
}
//ͨ���ܵ���Эͬ����д������
void CLDSLineAngle::ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
	{
		IndependentShadowPropToBuffer(buffer);
		return;
	}
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	mpcl.ToBuffer(&buffer);
	buffer.WriteDword(dwLayStyle);
	if(dwPermission&dwUpdateWord)
	{	//�Ǹֻ�����Ϣ
		buffer.WriteString(layer());
		buffer.Write(cfgword.flag.word,24);
		buffer.WriteInteger(m_uStatMatCoef);
		buffer.WriteInteger(m_hPartWeldParent);
		buffer.WriteByte(m_cFuncType);
		buffer.WriteBooleanThin(m_bUserSpecColor);	//�û�ָ��������ɫ
		buffer.WriteDword(crMaterial);		//������ɫ
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteInteger(m_uStatMatNo);
#endif
		buffer.WritePoint(get_norm_x_wing());
		buffer.WritePoint(get_norm_y_wing());
		buffer.WriteBooleanThin(_bVirtualPart);			//	�����Ǹ�
		buffer.WriteDouble(nominal_length);
		buffer.WriteByte(m_cHeJiaoReservedWidthType);
		buffer.WriteDouble(m_fHeJiaoReservedWidth);
		buffer.WriteInteger(kaihe_base_wing_x0_y1);
		buffer.WriteInteger(huoqu_base_wing_x0_y1_both2);
		buffer.WriteInteger(huoqu_start1_end2);
		buffer.WriteByte(m_cPosCtrlDatumLineType);
		buffer.WriteByte(m_cPosCtrlType);
		//�Ǹ�������������
		buffer.WriteInteger(m_bEnableTeG);
		buffer.WriteInteger(xWingXZhunJu.g);
		buffer.WriteInteger(xWingXZhunJu.g1);
		buffer.WriteInteger(xWingXZhunJu.g2);
		buffer.WriteInteger(xWingXZhunJu.g3);
		buffer.WriteInteger(xWingYZhunJu.g);
		buffer.WriteInteger(xWingYZhunJu.g1);
		buffer.WriteInteger(xWingYZhunJu.g2);
		buffer.WriteInteger(xWingYZhunJu.g3);
		
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteInteger(size_idClassType);
		buffer.WriteByte(size_cSubClassType);
#endif
		buffer.WriteDouble(GetWidth());
		buffer.WriteDouble(GetThick());
		buffer.WriteDouble(GetWidthWingY());	//���治�ȱ߽Ǹ�Y֫��֫�� wht 12-04-26
		buffer.WriteByte((BYTE)cMaterial);
		buffer.WriteByte((BYTE)m_cQualityLevel);
		buffer.WriteInteger(m_bReplPart);
		buffer.WriteByte((BYTE)org_material);
		buffer.WriteDouble(org_wing_wide);
		buffer.WriteDouble(org_wing_thick);
		buffer.WriteInteger(start_push_flat_x1_y2);
		buffer.WriteDouble(start_push_flat_length);
		buffer.WriteInteger(end_push_flat_x1_y2);
		buffer.WriteDouble(end_push_flat_length);
		buffer.WriteDouble(_fCutAngleCoefR);
		buffer.WriteWord(_nCutAngleSpaceOdd);
		///////////////////////////
		//д����
		BYTE ciIndex,cnCount=(BYTE)xarrOpenCuts.Count;
		buffer.WriteByte(cnCount);
		for(ciIndex=0;ciIndex<cnCount;ciIndex++)
		{
			OPENING_CUT* pCut=xarrOpenCuts.GetAt(ciIndex);
			buffer.WriteDouble(pCut->dfLateralBtmXorY);
			buffer.WriteDouble(pCut->dfLenPosFrom);
			buffer.WriteDouble(pCut->dfLenPosTo);
			buffer.WriteDouble(pCut->dfTipDeltaS);
			buffer.WriteDouble(pCut->dfTipDeltaE);
			buffer.WriteWord(pCut->siCornerFilletR);
		}
		//д��ֲ����Ͻ�
		cnCount=(BYTE)xarrKaiheSects.Count;
		buffer.WriteByte(cnCount);
		for(ciIndex=0;ciIndex<cnCount;ciIndex++)
		{
			KAIHE_SECTION* pSect=xarrKaiheSects.GetAt(ciIndex);
			buffer.WriteByte(pSect->ciOperWing);
			buffer.WriteDouble(pSect->dfFromX);
			buffer.WriteDouble(pSect->dfToX);
			buffer.WriteDouble(pSect->dfReservedSizeY);
			buffer.WriteDouble(pSect->dfOpenAngle);
		}
		///////////////////////////

		buffer.WriteInteger(group_father_jg_h);
		buffer.WriteInteger(iSeg);
		buffer.WriteString(sPartNo);	//ԭʼ�������
		buffer.WriteInteger(m_hArcLift);
		// --------- 1.4. ������Ϣ-----------
		buffer.WriteInteger(m_bCutRoot);
		buffer.WriteInteger(m_bCutBer);
		buffer.WriteInteger(m_bWeldPart);
		buffer.WriteWord(m_nCutBerR);
		buffer.WriteInteger(mid_push_flat_x1_y2);
		if(mid_push_flat_x1_y2>0)
		{
			buffer.WriteDouble(_mid_push_flat_length);
			buffer.WriteInteger(mid_push_flat_node_handle);
			buffer.WriteDouble(_mid_push_flat_up_offset);
		}
		buffer.WriteInteger(son_jg_h[0]);
		buffer.WriteInteger(son_jg_h[1]);
		buffer.WriteBooleanThin(m_bDisplayHole);		//����ʾ��˨��
		//�����Ϣ
		buffer.WriteInteger(des_norm_x.bByOtherWing);
		buffer.WriteInteger(des_norm_x.bSpecific);
		buffer.WritePoint(des_norm_x.near_norm);
		des_norm_x.spec_norm.ToBuffer(buffer);
		buffer.WriteInteger(des_norm_x.hViceJg);
		buffer.WriteInteger(des_norm_y.bByOtherWing);
		buffer.WriteInteger(des_norm_y.bSpecific);
		buffer.WritePoint(des_norm_y.near_norm);
		des_norm_y.spec_norm.ToBuffer(buffer);
		buffer.WriteInteger(des_norm_y.hViceJg);
		buffer.WriteDouble(huoqu_r);
		buffer.WriteInteger(huoqu_handle);
		buffer.WriteString(sNotes);
		//�м���˨��
		long old_position=buffer.GetCursorPosition();
		buffer.WriteInteger(0);
		int nLs=0;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pLsRef->start1_end2_mid0==0)
			{
				buffer.WriteInteger((*pLsRef)->handle);
				buffer.WriteDword(pLsRef->dwRayNo);
				nLs++;
			}
		}
		long new_position=buffer.GetCursorPosition();
		buffer.SeekPosition(old_position);
		buffer.WriteInteger(nLs);
		buffer.SeekPosition(new_position);
		//���������б�
		RELATIVE_OBJECT *pRelaObj=NULL;
		buffer.WriteInteger(relativeObjs.GetNodeNum());
		for(pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			buffer.WriteInteger(pRelaObj->hObj);
			buffer.WriteByte(pRelaObj->mirInfo.axis_flag);
			bool bSpecOrigin=!pRelaObj->mirInfo.origin.IsZero();
			buffer.WriteBooleanThin(bSpecOrigin);
			if(bSpecOrigin)
				buffer.WritePoint(pRelaObj->mirInfo.origin);
			if(pRelaObj->mirInfo.axis_flag&0x08)
			{
				buffer.WriteByte(pRelaObj->mirInfo.array_num);
				buffer.WriteWord(pRelaObj->mirInfo.rotate_angle);
			}
		}
		buffer.WriteInteger(m_bFootNail);	
		buffer.WriteBooleanThin(_blOddByBoltCanAcrossMiddleSect);
		buffer.WriteBooleanThin(_blScatterBoltLocaTolerance);
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteInteger(m_bAutoCalHoleOutCoef);	//�Զ�������˨������������Ϊָ����˨������
		buffer.WriteDouble(hole_out);			//�˼��������˨������
		//buffer.WriteInteger(CalLenCoef.iSelRFlag);
		buffer.WriteInteger(CalLenCoef.iTypeNo);
		buffer.WriteInteger(CalLenCoef.hHorizPole);
		buffer.WriteInteger(CalLenCoef.hRefPole);
		buffer.WriteInteger(CalLenCoef.minR.hStartNode);
		buffer.WriteInteger(CalLenCoef.minR.hEndNode);
		buffer.WriteDouble(CalLenCoef.minR.coef);
		buffer.WriteInteger(CalLenCoef.paraR.hStartNode);
		buffer.WriteInteger(CalLenCoef.paraR.hEndNode);
		buffer.WriteDouble(CalLenCoef.paraR.coef);
		buffer.WriteWord(m_iElemType);			//0��ʾ�Զ��ж�;1��ʾ���������;2��ʾ��������Ԫ;3��ʾ������Ԫ
		buffer.WriteDword(m_uStatMatNo);		//ͳ�����
		buffer.WriteInteger(m_bNeedSelSize);	//��Ҫѡ����
		buffer.WriteInteger(m_bNeedSelMat);		//��Ҫѡ�����
		buffer.WriteInteger(sel_mat_method);	//����ѡ�ķ�ʽ
		buffer.WriteDouble(bear_load_coef);		//������֧����֧�Ÿ˼�������
		buffer.WriteDouble(m_fWeightRaiseCoef);	//�����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
		buffer.WriteByte(m_cbRodFlag);
#endif
	}
	if(dwStartPermission&dwUpdateWord)
	{	//�Ǹ�ʼ�˻�����Ϣ
		if(pStart)	// �����
			buffer.WriteInteger(pStart->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteInteger(bHuoQuStart);
		// --------- 1.2. �н� -----------
		buffer.WriteInteger(m_bUserSpecStartCutAngle);
		/*cut_wing_para[0]��ʾʼ����֫cut_wing_para[1]��ʾ�ն���֫
		...[][0]��ʾX����Ƥ��ȡ����
		...[][1]��ʾY����Ƥ��ȡ����
		...[][2]��ʾZ����Ƥ��ȡ����
		*/
		buffer.WriteDouble(cut_wing_para[0][0]);
		buffer.WriteDouble(cut_wing_para[0][1]);
		buffer.WriteDouble(cut_wing_para[0][2]);
		//������֫����֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
		buffer.WriteInteger(cut_wing[0]);
		// --------- 1.3. ��֫ -----------
		/*
		cut_angle[0]��ʾ���X֫
		cut_angle[1]��ʾ���Y֫
		cut_angle[2]��ʾ�յ�X֫
		cut_angle[3]��ʾ�յ�Y֫
		...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
		*/
		buffer.WriteDouble(cut_angle[0][0]);
		buffer.WriteDouble(cut_angle[0][1]);
		buffer.WriteDouble(cut_angle[1][0]);
		buffer.WriteDouble(cut_angle[1][1]);
		//�Ǹֵ�Բ����Ϣ
		buffer.WriteDouble(_dfFilletROfSX);
		buffer.WriteDouble(_dfFilletROfSY);

		buffer.WritePoint(baseline.Start());  	// ʵ���������
		buffer.WritePoint(m_xStartBeforeLift);
		buffer.WriteDouble(start_oddment);
		buffer.WriteInteger(desStartOdd.m_iOddCalStyle);
		buffer.WriteInteger(desStartOdd.m_hRefPart1);
		buffer.WriteInteger(desStartOdd.m_hRefPart2);
		buffer.WriteDouble(desStartOdd.m_fCollideDist);
		//�洢�Ǹ�ʼ�˶���(�̶�)����ͷ
		buffer.WriteByte(extraStartOdd.ber.style);
		if(extraStartOdd.ber.style==0)
			buffer.WriteDouble(extraStartOdd.ber.odd);
		else
			extraStartOdd.ber.extraDatumPoint.ToBuffer(buffer);
		buffer.WriteByte(extraStartOdd.wing_x.style);
		if(extraStartOdd.wing_x.style==0)
			buffer.WriteDouble(extraStartOdd.wing_x.odd);
		else
			extraStartOdd.wing_x.extraDatumPoint.ToBuffer(buffer);
		buffer.WriteByte(extraStartOdd.wing_y.style);
		if(extraStartOdd.wing_y.style==0)
			buffer.WriteDouble(extraStartOdd.wing_y.odd);
		else
			extraStartOdd.wing_y.extraDatumPoint.ToBuffer(buffer);
		buffer.WriteInteger(IsStartPosLocked());
		buffer.WritePoint(pre_huoqu_start_xyz);
		desStartPos.ToBuffer(buffer);
		buffer.WriteInteger(connectStart.d);   	//(��ˮƽ�Ǹֵ��϶˻�ˮƽ�Ǹֵ�)��ʼ����˨��Ϣ
		buffer.WriteByte(connectStart.m_iConnectWing);
		buffer.WriteByte(connectStart.m_iConnectType);
		buffer.Write(&connectStart.uiLegacyN,4);//connectStart.N);
		buffer.WriteInteger(connectStart.rows);	
		buffer.WriteString(connectStart.grade);	//��˨���� 
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteDouble(connectStart.maxTension);
		buffer.WriteDouble(connectStart.maxCompression);
		buffer.WriteDouble(connectStart.maxMoment);
		buffer.WriteWord((WORD)start_force_type);
		buffer.WriteWord((WORD)start_joint_type);
#endif
		//ʼ����˨��
		long old_position=buffer.GetCursorPosition();
		buffer.WriteInteger(0);
		int nLs=0;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pLsRef->start1_end2_mid0==1)
			{
				buffer.WriteInteger((*pLsRef)->handle);
				buffer.WriteDword(pLsRef->dwRayNo);
				nLs++;
			}
		}
		long new_position=buffer.GetCursorPosition();
		buffer.SeekPosition(old_position);
		buffer.WriteInteger(nLs);
		buffer.SeekPosition(new_position);
	}
	if(dwEndPermission&dwUpdateWord)
	{	//�Ǹ��ն˻�����Ϣ
		if(pEnd)	// �յ���
			buffer.WriteInteger(pEnd->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteInteger(bHuoQuEnd);
		// --------- 1.2. �н� -----------
		buffer.WriteInteger(m_bUserSpecEndCutAngle);
		/*cut_wing_para[0]��ʾʼ����֫cut_wing_para[1]��ʾ�ն���֫
		...[][0]��ʾX����Ƥ��ȡ����
		...[][1]��ʾY����Ƥ��ȡ����
		...[][2]��ʾZ����Ƥ��ȡ����
		*/
		buffer.WriteDouble(cut_wing_para[1][0]);
		buffer.WriteDouble(cut_wing_para[1][1]);
		buffer.WriteDouble(cut_wing_para[1][2]);
		//������֫����֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
		buffer.WriteInteger(cut_wing[1]);
		// --------- 1.3. ��֫ -----------
		/*
		cut_angle[0]��ʾ���X֫
		cut_angle[1]��ʾ���Y֫
		cut_angle[2]��ʾ�յ�X֫
		cut_angle[3]��ʾ�յ�Y֫
		...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
		*/
		buffer.WriteDouble(cut_angle[2][0]);
		buffer.WriteDouble(cut_angle[2][1]);
		buffer.WriteDouble(cut_angle[3][0]);
		buffer.WriteDouble(cut_angle[3][1]);
		//�Ǹֵ�Բ����Ϣ
		buffer.WriteDouble(_dfFilletROfEX);
		buffer.WriteDouble(_dfFilletROfEY);

		buffer.WritePoint(baseline.End());  	// ʵ�������յ�
		buffer.WritePoint(m_xEndBeforeLift);
		buffer.WriteDouble(end_oddment);   // �Ǹ�ʼ�ն�����ͷ.
		buffer.WriteInteger(desEndOdd.m_iOddCalStyle);
		buffer.WriteInteger(desEndOdd.m_hRefPart1);
		buffer.WriteInteger(desEndOdd.m_hRefPart2);
		buffer.WriteDouble(desEndOdd.m_fCollideDist);
		//�洢�Ǹ�ʼ�˶���(�̶�)����ͷ
		buffer.WriteByte(extraEndOdd.ber.style);
		if(extraEndOdd.ber.style==0)
			buffer.WriteDouble(extraEndOdd.ber.odd);
		else
			extraEndOdd.ber.extraDatumPoint.ToBuffer(buffer);
		buffer.WriteByte(extraEndOdd.wing_x.style);
		if(extraEndOdd.wing_x.style==0)
			buffer.WriteDouble(extraEndOdd.wing_x.odd);
		else
			extraEndOdd.wing_x.extraDatumPoint.ToBuffer(buffer);
		buffer.WriteByte(extraEndOdd.wing_y.style);
		if(extraEndOdd.wing_y.style==0)
			buffer.WriteDouble(extraEndOdd.wing_y.odd);
		else
			extraEndOdd.wing_y.extraDatumPoint.ToBuffer(buffer);
		buffer.WriteInteger(IsEndPosLocked());
		buffer.WritePoint(pre_huoqu_end_xyz);
		desEndPos.ToBuffer(buffer);
		buffer.WriteInteger(connectEnd.d);   	//(��ˮƽ�Ǹֵ��϶˻�ˮƽ�Ǹֵ�)��ʼ����˨��Ϣ
		buffer.WriteByte(connectEnd.m_iConnectWing);
		buffer.WriteByte(connectEnd.m_iConnectType);
		buffer.Write(&connectEnd.uiLegacyN,4);//.N);
		buffer.WriteInteger(connectEnd.rows);
		buffer.WriteString(connectEnd.grade);	//��˨���� 
#ifdef __PART_DESIGN_INC_//defined(__LDS_FILE_)
		buffer.WriteDouble(connectEnd.maxTension);
		buffer.WriteDouble(connectEnd.maxCompression);
		buffer.WriteDouble(connectEnd.maxMoment);
		buffer.WriteWord((WORD)end_force_type);
		buffer.WriteWord((WORD)end_joint_type);
#endif
		//ĩ����˨��
		long old_position=buffer.GetCursorPosition();
		buffer.WriteInteger(0);
		int nLs=0;
		for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(pLsRef->start1_end2_mid0==2)
			{
				buffer.WriteInteger((*pLsRef)->handle);
				buffer.WriteDword(pLsRef->dwRayNo);
				nLs++;
			}
		}
		long new_position=buffer.GetCursorPosition();
		buffer.SeekPosition(old_position);
		buffer.WriteInteger(nLs);
		buffer.SeekPosition(new_position);
	}
}

int CLDSLineAngle::AutoCorrectSonNode()
{
	if(pStart==NULL||pEnd==NULL)
		return 0;
	f3dPoint start=pStart->Position(false);
	f3dPoint end=pEnd->Position(false);
	f3dPoint valid_vec=end-start;
	normalize(valid_vec);
	int nHits=0;
	for(CLDSNode *pNode=BelongModel()->EnumNodeFirst();pNode;pNode=BelongModel()->EnumNodeNext())
	{
		if(pNode->hFatherPart==handle)
			continue;	//�˽ڵ㸸�Ǹ��Ѿ��ǵ�ǰ�Ǹ֣��������ж�
		f3dPoint start_vec=pNode->Position(false)-start;
		f3dPoint end_vec=pNode->Position(false)-end;
		normalize(start_vec);
		normalize(end_vec);
		if(valid_vec*start_vec>EPS_COS&&valid_vec*end_vec<-EPS_COS)
		{
			CLDSLineAngle *pJg=(CLDSLineAngle*)BelongModel()->FromPartHandle(pNode->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if(pJg)
			{
				f3dLine line;
				line.startPt=pJg->pStart->Position(false);
				line.endPt=pJg->pEnd->Position(false);
				if(line.PtInLine(pNode->Position(false))>0)
					continue;	//�ڵ㵱ǰ���Ǹ��Ѿ��ǺϷ��ĸ��Ǹ֣��������޸��丸�Ǹ�
			}
			pNode->hFatherPart=handle;
			nHits++;
		}
	}
	return nHits;
}

int CLDSLineAngle::IsNodeInJg(CLDSNode *pNode)
{
	if(pStart&&pEnd)
	{
		f3dLine line(pStart->Position(false),pEnd->Position(false));
		return line.PtInLine(pNode->Position(false));
	}
	else
		return 0;
}

bool CLDSLineAngle::IsCutAngleOrWing()
{
	double cut_angle_value= cut_angle[0][0]+cut_angle[0][1]+
				cut_angle[1][0]+cut_angle[1][1]+
				cut_angle[2][0]+cut_angle[2][1]+
				cut_angle[3][0]+cut_angle[3][1];
	if(cut_wing[0]>0||cut_wing[1]>0||cut_angle_value>0)
		return true;
	else
		return false;
}

bool CLDSLineAngle::IsStartCutAngleOrWing()
{
	double cut_angle_value= cut_angle[0][0]+cut_angle[0][1]+
		cut_angle[1][0]+cut_angle[1][1];
	if(cut_wing[0]>0||cut_angle_value>0)
		return true;
	else
		return false;
}

bool CLDSLineAngle::IsEndCutAngleOrWing()
{
	double cut_angle_value= cut_angle[2][0]+cut_angle[2][1]+
		cut_angle[3][0]+cut_angle[3][1];
	if(cut_wing[1]>0||cut_angle_value>0)
		return true;
	else
		return false;
}

double CLDSLineAngle::GetStartOffsetByWingX()
{
	if(!m_bEnableTeG)
		getjgzj(xWingYZhunJu,size_wide);
	if(desStartPos.wing_x_offset.gStyle==0)
		return xWingYZhunJu.g;
	else if(desStartPos.wing_x_offset.gStyle==1)
		return xWingYZhunJu.g1;
	else if(desStartPos.wing_x_offset.gStyle==2)
		return xWingYZhunJu.g2;
	else if(desStartPos.wing_x_offset.gStyle==3)
		return xWingYZhunJu.g3;
	else //if(desStartPos.wing_x_offset.gStyle==4)
		return desStartPos.wing_x_offset.offsetDist;
}

double CLDSLineAngle::GetStartOffsetByWingY()
{
	if(!m_bEnableTeG)
		getjgzj(xWingXZhunJu,size_wide);
	if(desStartPos.wing_y_offset.gStyle==0)
		return xWingXZhunJu.g;
	else if(desStartPos.wing_y_offset.gStyle==1)
		return xWingXZhunJu.g1;
	else if(desStartPos.wing_y_offset.gStyle==2)
		return xWingXZhunJu.g2;
	else if(desStartPos.wing_y_offset.gStyle==3)
		return xWingXZhunJu.g3;
	else //if(desStartPos.wing_y_offset.gStyle==4)
		return desStartPos.wing_y_offset.offsetDist;
}
double CLDSLineAngle::GetEndOffsetByWingX()
{
	if(!m_bEnableTeG)
		getjgzj(xWingYZhunJu,size_wide);
	if(desEndPos.wing_x_offset.gStyle==0)
		return xWingYZhunJu.g;
	else if(desEndPos.wing_x_offset.gStyle==1)
		return xWingYZhunJu.g1;
	else if(desEndPos.wing_x_offset.gStyle==2)
		return xWingYZhunJu.g2;
	else if(desEndPos.wing_x_offset.gStyle==3)
		return xWingYZhunJu.g3;
	else //if(desEndPos.wing_x_offset.gStyle==4)
		return desEndPos.wing_x_offset.offsetDist;
}
double CLDSLineAngle::GetEndOffsetByWingY()
{
	if(!m_bEnableTeG)
		getjgzj(xWingXZhunJu,size_wide);
	if(desEndPos.wing_y_offset.gStyle==0)
		return xWingXZhunJu.g;
	else if(desEndPos.wing_y_offset.gStyle==1)
		return xWingXZhunJu.g1;
	else if(desEndPos.wing_y_offset.gStyle==2)
		return xWingXZhunJu.g2;
	else if(desEndPos.wing_y_offset.gStyle==3)
		return xWingXZhunJu.g3;
	else //if(desEndPos.wing_y_offset.gStyle==4)
		return desEndPos.wing_y_offset.offsetDist;
}

void CLDSLineAngle::CalStartOddment(char ciAutoCalMode/*=0*/,int liInitOddment/*=0*/)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	double fOldStartOdd=start_oddment;
	if(desStartOdd.m_iOddCalStyle==0||(ciAutoCalMode==1&&desEndOdd.m_iOddCalStyle==2))
	{	//������ײ��������ͷ
		if(!m_bUserSpecStartCutAngle)
		{	//���û�ָ���н���Ϣʱ�������֫�н���Ϣ wht 11-07-04
			cut_angle[0][0]=cut_angle[0][1]=cut_angle[1][0]=cut_angle[1][1]=0;
			cut_wing[0]=0;
			cut_wing_para[0][0]=cut_wing_para[0][1]=cut_wing_para[0][2]=0;
		}
		CLDSPart *pPartArr[2]={NULL};
		pPartArr[0]=BelongModel()->FromPartHandle(desStartOdd.m_hRefPart1);
		pPartArr[1]=BelongModel()->FromPartHandle(desStartOdd.m_hRefPart2);
		double length=DISTANCE(Start(),End());
		if(pPartArr[0]||pPartArr[1])
			start_oddment=liInitOddment;
		else
			return;
		BOOL bCollide=TRUE;
		int ENDGAP_STEP=160;
		while(bCollide)
		{
			bCollide = FALSE;
			for(int i=0;i<2;i++)
			{
				if(pPartArr[i])
				{
					if(pPartArr[i]->GetClassTypeId()==CLS_LINEANGLE&&group_father_jg_h>0&&group_father_jg_h==((CLDSLineAngle*)pPartArr[i])->group_father_jg_h)
						continue;	//ͬһ��ϽǸ��е��ӽǸּ䲻�����ײ
					else if(pPartArr[i]->GetClassTypeId()==CLS_GROUPLINEANGLE&&group_father_jg_h==pPartArr[i]->handle)
						continue;	//��ϽǸֵ��ӽǸ�����ϽǸּ䲻�����ײ
					if(pPartArr[i]->GetClassTypeId()==CLS_GROUPLINEANGLE)
					{
						CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pPartArr[i];
						for(int j=0;j<4;j++)
						{
							if(pGroupAngle->group_style<2&&j>=2)
								continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ���
							CLDSPart *pSonAngle=BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[j],CLS_LINEANGLE);
							if(pSonAngle==NULL)
								continue;
							if(IsPartsCollide(pSonAngle,this,ftoi(desStartOdd.m_fCollideDist)))
							{
								if(ENDGAP_STEP>=10)
									ENDGAP_STEP/=2;
								start_oddment-=ENDGAP_STEP;//5;
								if(length+start_oddment+end_oddment<5)	//
								{	//�粻�Ӵ��ж�,���ʼ���нǲ��ո˼�Ϊ�ն˸��˼��Ļ�,��������ѭ�� wjh-2011.6.26
									bCollide=FALSE;
									if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
										log2file->Log("0x%X Angle start oddment calculates wrong,oddment calculates interruptive",handle);
#else
										log2file->Log("0x%X�Ǹ�ʼ�˸�ͷ�������,����ͷ�����ж�",handle);
#endif
									return;
								}
								else
									bCollide=TRUE;
								break;
							}
						}
						if(bCollide)
							break;
					}
					else
					{
						if(IsPartsCollide(pPartArr[i],this,ftoi(desStartOdd.m_fCollideDist)))
						{
							if(ENDGAP_STEP>=10)
								ENDGAP_STEP/=2;
							start_oddment-=ENDGAP_STEP;//5;
							if(length+start_oddment+end_oddment<5)	//
							{
								bCollide=FALSE;
								if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
									log2file->Log("0x%X Angle start oddment calculates wrong,oddment calculates interruptive",handle);
#else
									log2file->Log("0x%X�Ǹ�ʼ�˸�ͷ�������,����ͷ�����ж�",handle);
#endif
								return;
							}
							else
								bCollide=TRUE;
							break;
						}
					}
				}
			}
			if(!bCollide&&ENDGAP_STEP>=10)
			{
				ENDGAP_STEP/=2;
				if(start_oddment+ENDGAP_STEP<0)
				{
					start_oddment+=ENDGAP_STEP;//5;
					bCollide=TRUE;
				}
			}
		}
	}
	else
	{
		if(desStartOdd.m_iOddCalStyle==1)
		{	//������˨��������ͷ
			BOOL bFirstLs=TRUE;
			f3dLine ber_line;
			if(GetLsCount()==0)
				return;	//����˨����Ϊ��������
			bool bHasTipNode=(pStart!=NULL&&pEnd!=NULL);
			double dfLineLength=!bHasTipNode?0:DISTANCE(pStart->xOriginalPos,pEnd->xOriginalPos);
			double dfDetectLength=dfLineLength*0.4;
			ber_line.startPt=Start();
			ber_line.endPt=End();
			double head_len=0;
			f3dPoint base_vec,len_vec;
			base_vec=ber_line.endPt-ber_line.startPt;
			normalize(base_vec);
			long start_d,end_d;
			for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				f3dPoint lspos;
				//��GetBoltIntersPos����λ�÷�������ͷ��ʵ������С�������״��� wjh-2016.8.20
				GetBoltIntersPos(pLsRef->GetLsPtr(),lspos);
				SnapPerp(&lspos,ber_line,lspos);
				len_vec=lspos-ber_line.startPt;
				double len=base_vec*len_vec;
				if(bHasTipNode&&len>dfDetectLength&&!m_blOddByBoltCanAcrossMiddleSect)
					continue;	//��˨λ�ó����˼�����40%��Ϊȱʧ��ͷ��˨�������Ǹ�����˨�������ͷ wjh-2019.6.24

				if(bFirstLs)
				{
					head_len=len;
					start_d=end_d=(*pLsRef)->get_d();
					bFirstLs=FALSE;
				}
				else if(len<head_len)
				{
					head_len=len;
					start_d=(*pLsRef)->get_d();
				}
			}
			if(!bFirstLs)
			{
				LSSPACE_STRU LsSpace;
				GetLsSpace(LsSpace,start_d);
				start_oddment=fto_halfi(LsSpace.EndSpace-head_len);
				//start_oddment=ftoi((LsSpace.EndSpace-head_len)*10)*0.1;
			}
			else
				start_oddment=0;
		}
		CLDSLineAngle *pCutAngle1=(CLDSLineAngle*)BelongModel()->FromPartHandle(desStartOdd.m_hRefPart1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CLDSLineAngle *pCutAngle2=(CLDSLineAngle*)BelongModel()->FromPartHandle(desStartOdd.m_hRefPart2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
/*		
		if(pCutAngle1&&pCutAngle1->GetClassTypeId()==CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("The collision ref rod is an angle combination, not real solid angle. angle cutting may be inaccurate!",desStartOdd.m_hRefPart1);
#else
			Log2File()->Log("�Ǹֵ���ײ���ո˼�0X%XΪ��ϽǸ֣�������ʵʵ�壬�нǼ�����ܲ�׼ȷ!",desStartOdd.m_hRefPart1);
#endif
		if(pCutAngle2&&pCutAngle2->GetClassTypeId()==CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("The collision ref rod is an angle combination, not real solid angle. angle cutting may be inaccurate!",desStartOdd.m_hRefPart2);
#else
			Log2File()->Log("�Ǹֵ���ײ���ո˼�0X%XΪ��ϽǸ֣�������ʵʵ�壬�нǼ�����ܲ�׼ȷ!",desStartOdd.m_hRefPart2);
#endif
*/
		if(pCutAngle1||pCutAngle2)
			CalCutAngleInfo(pCutAngle1,pCutAngle2,true,NULL,ftoi(desStartOdd.m_fCollideDist));
	}
	if(fabs(fOldStartOdd-start_oddment)>ODDMENT_MINIMAL_MODIFIED&&(_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)==0)
	{
		CLDSLineAngle::AppendModifiedAngles(this);
		GatherOddRelaParts(1);
	}
	SetModified();
#endif
}
void CLDSLineAngle::CalEndOddment(char ciAutoCalMode/*=0*/,int liInitOddment/*=0*/)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	double fOldEndOdd=end_oddment;
	if(desEndOdd.m_iOddCalStyle==0||(ciAutoCalMode==1&&desEndOdd.m_iOddCalStyle==2))
	{	//������ײ��������ͷ
		if(!m_bUserSpecEndCutAngle)
		{	//���û�ָ���н���Ϣʱ�������֫�н���Ϣ wht 11-07-04
			cut_angle[2][0]=cut_angle[2][1]=cut_angle[3][0]=cut_angle[3][1]=0;
			cut_wing[1]=0;
			cut_wing_para[1][0]=cut_wing_para[1][1]=cut_wing_para[1][2]=0;
		}
		double length=DISTANCE(Start(),End());
		CLDSPart *pPartArr[2]={NULL};
		pPartArr[0]=BelongModel()->FromPartHandle(desEndOdd.m_hRefPart1);
		pPartArr[1]=BelongModel()->FromPartHandle(desEndOdd.m_hRefPart2);
		if(pPartArr[0]||pPartArr[1])
			end_oddment=liInitOddment;
		else
			return;
		BOOL bCollide=TRUE;
		int ENDGAP_STEP=160;
		while(bCollide)
		{
			bCollide = FALSE;
			for(int i=0;i<2;i++)
			{
				if(pPartArr[i])
				{
					if(pPartArr[i]->GetClassTypeId()==CLS_LINEANGLE&&group_father_jg_h>0&&group_father_jg_h==((CLDSLineAngle*)pPartArr[i])->group_father_jg_h)
						continue;	//ͬһ��ϽǸ��е��ӽǸּ䲻�����ײ
					else if(pPartArr[i]->GetClassTypeId()==CLS_GROUPLINEANGLE&&group_father_jg_h==pPartArr[i]->handle)
						continue;	//��ϽǸֵ��ӽǸ�����ϽǸּ䲻�����ײ
					if(pPartArr[i]->GetClassTypeId()==CLS_GROUPLINEANGLE)
					{
						CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)pPartArr[i];
						for(int j=0;j<4;j++)
						{
							if(pGroupAngle->group_style<2&&j>=2)
								continue;	//�����ĽǸ�ת��Ϊ˫ƴ�Ǹ�ʱ����ѵ������ĸ��ӽǸ�ת��Ϊ��Ǹִ���
							CLDSPart *pSonAngle=BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[j],CLS_LINEANGLE);
							if(pSonAngle==NULL)
								continue;
							if(IsPartsCollide(pSonAngle,this,ftoi(desEndOdd.m_fCollideDist)))
							{
								if(ENDGAP_STEP>=10)
									ENDGAP_STEP/=2;
								end_oddment-=ENDGAP_STEP;//5;
								if(length+start_oddment+end_oddment<5)	//
								{	//�粻�Ӵ��ж�,����ն��нǲ��ո˼�Ϊʼ�˸��˼��Ļ�,��������ѭ�� wjh-2011.6.26
									bCollide=FALSE;
									if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
										log2file->Log("0x%X Angle end oddment calculates wrong,oddment calculates interruptive",handle);
#else
										log2file->Log("0x%X�Ǹ��ն˸�ͷ�������,����ͷ�����ж�",handle);
#endif
									return;
								}
								else
									bCollide=TRUE;
								break;
							}
						}
						if(bCollide)
							break;
					}
					else
					{
						if(IsPartsCollide(pPartArr[i],this,ftoi(desEndOdd.m_fCollideDist)))
						{
							if(ENDGAP_STEP>=10)
								ENDGAP_STEP/=2;
							end_oddment-=ENDGAP_STEP;//5;
							if(length+start_oddment+end_oddment<5)	//
							{
								bCollide=FALSE;
								if(log2file)
#ifdef AFX_TARG_ENU_ENGLISH
									log2file->Log("0x%X Angle end oddment calculates wrong,oddment calculates interruptive",handle);
#else
									log2file->Log("0x%X�Ǹ��ն˸�ͷ�������,����ͷ�����ж�",handle);
#endif
								return;
							}
							else
								bCollide=TRUE;
							break;
						}
					}
				}
			}
			if(!bCollide&&ENDGAP_STEP>=10)
			{
				ENDGAP_STEP/=2;
				if(end_oddment+ENDGAP_STEP<0)
				{
					end_oddment+=ENDGAP_STEP;//5;
					bCollide=TRUE;
				}
			}
		}
	}
	else
	{
		if(desEndOdd.m_iOddCalStyle==1)
		{	//������˨��������ͷ
			BOOL bFirstLs=TRUE;
			f3dLine ber_line;
			if(GetLsCount()==0)
				return;	//����˨����Ϊ��������
			bool bHasTipNode=(pStart!=NULL&&pEnd!=NULL);
			double dfLineLength=!bHasTipNode?0:DISTANCE(pStart->xOriginalPos,pEnd->xOriginalPos);
			double dfDetectLength=dfLineLength*0.4;
			ber_line.startPt=Start();
			ber_line.endPt=End();
			double head_len=0;
			f3dPoint base_vec,len_vec;
			base_vec=ber_line.startPt-ber_line.endPt;
			normalize(base_vec);
			long start_d,end_d;
			for(CLsRef *pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
			{
				f3dPoint lspos;
				GetBoltIntersPos(pLsRef->GetLsPtr(),lspos);
				SnapPerp(&lspos,ber_line,lspos);
				len_vec=lspos-ber_line.endPt;
				double len=base_vec*len_vec;
				if(bHasTipNode&&len>dfDetectLength&&!m_blOddByBoltCanAcrossMiddleSect)
					continue;	//��˨λ�ó����˼�����40%��Ϊȱʧ��ͷ��˨�������Ǹ�����˨�������ͷ wjh-2019.6.24
				if(bFirstLs)
				{
					head_len=len;
					start_d=end_d=(*pLsRef)->get_d();
					bFirstLs=FALSE;
				}
				else if(len<head_len)
				{
					head_len=len;
					start_d=(*pLsRef)->get_d();
				}
			}
			if(!bFirstLs)
			{
				LSSPACE_STRU LsSpace;
				GetLsSpace(LsSpace,start_d);
				end_oddment=fto_halfi(LsSpace.EndSpace-head_len);
				//��ϽǸ�ʱ��˨��������ͷ�����Բ�����ȿ�����0.5���ܻᵼ�´����������Ȳ�<0.5�ķŴ󵽲�1mm wjh-2015.5.19
				//end_oddment=ftoi((LsSpace.EndSpace-head_len)*10)*0.1;
			}
			else
				end_oddment=0;
		}
		CLDSLineAngle *pCutAngle1=(CLDSLineAngle*)BelongModel()->FromPartHandle(desEndOdd.m_hRefPart1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CLDSLineAngle *pCutAngle2=(CLDSLineAngle*)BelongModel()->FromPartHandle(desEndOdd.m_hRefPart2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
/*		
		if(pCutAngle1&&pCutAngle1->GetClassTypeId()==CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("The collision ref rod is an angle combination, not real solid angle. angle cutting may be inaccurate!",desEndOdd.m_hRefPart1);
#else
			Log2File()->Log("�Ǹֵ���ײ���ո˼�0X%XΪ��ϽǸ֣�������ʵʵ�壬�нǼ�����ܲ�׼ȷ!",desEndOdd.m_hRefPart1);
#endif
		if(pCutAngle2&&pCutAngle2->GetClassTypeId()==CLS_GROUPLINEANGLE)
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->Log("The collision ref rod is an angle combination, not real solid angle. angle cutting may be inaccurate!",desEndOdd.m_hRefPart2);
#else
			Log2File()->Log("�Ǹֵ���ײ���ո˼�0X%XΪ��ϽǸ֣�������ʵʵ�壬�нǼ�����ܲ�׼ȷ!",desEndOdd.m_hRefPart2);
#endif
*/
		if(pCutAngle1||pCutAngle2)
			CalCutAngleInfo(pCutAngle1,pCutAngle2,false,NULL,ftoi(desEndOdd.m_fCollideDist));
	}
	if(fabs(fOldEndOdd-end_oddment)>ODDMENT_MINIMAL_MODIFIED&&(_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)==0)
	{
		CLDSLineAngle::AppendModifiedAngles(this);
		GatherOddRelaParts(2);
	}
	SetModified();
#endif
}

int CLDSLineAngle::GetWingXG(int styleG0123)
{
	JGZJ jgzj;
	if(m_bEnableTeG)
		jgzj=xWingXZhunJu;
	else
		getjgzj(jgzj,size_wide);
	if(styleG0123==0)
		return jgzj.g;
	else if(styleG0123==1)
		return jgzj.g1;
	else if(styleG0123==2)
		return jgzj.g2;
	else if(styleG0123==3)
		return jgzj.g3;
	else
		return 0;
}

double CLDSLineAngle::GetCutAngleCoefR()//��ȡ�нǰ뾶ϵ��
{
	if(m_fCutAngleCoefR>0||m_nCutAngleSpaceOdd>0)
		return m_fCutAngleCoefR;
	else if(library!=NULL)
		return library->GetCutAngleCoefR();
	else
		return 2/3.0;
}
CXhChar16 CLDSLineAngle::GetCutAngleSpaceExpr()//��ȡ�Ǹ�֫�ڲ��н�ʱ�ļ�϶ֵ����Բ���뾶*ϵ��+��ͷ��
{
	if(m_fCutAngleCoefR<=0&&m_nCutAngleSpaceOdd>0)
		return CXhChar16("%d",m_nCutAngleSpaceOdd);	//ָ����϶ֵ
	else if(m_fCutAngleCoefR<=0&&m_nCutAngleSpaceOdd==0)
		return CXhChar16("0");	//ϵͳĬ��ֵ
	else
	{
		CXhChar16 expr;
		CExpression::ToExpression(m_fCutAngleCoefR,3,expr,8);
		
		if(expr.Equal("1"))
			expr.Copy("R");
		else if(!expr.Equal("0"))
			expr.InsertBefore("R*");
		if(m_nCutAngleSpaceOdd>0)
			expr.Append(CXhChar16("+%d",m_nCutAngleSpaceOdd));
		return expr;
	}
}
double CLDSLineAngle::GetWeigthCenterG()
{
	return size.wide*0.25;
}
JGZJ CLDSLineAngle::GetAngleZJ(double wide)		//��ѯ�Ǹ�׼��
{
	JGZJ jgzj;
	if(library)
		jgzj=library->GetAngleZJ(wide);
	else
	{
		jgzj.g=ftoi(wide*0.5);
		jgzj.g1=jgzj.g2=jgzj.g3=0;
	}
	return jgzj;
}
JGZJ CLDSLineAngle::GetZhunJu(char cWorkWing/*='X'*/)
{
	JGZJ jgzj;
	jgzj.g=jgzj.g1=jgzj.g2=jgzj.g3=0;
	if(m_bEnableTeG)
	{
		if(cWorkWing=='X'||cWorkWing=='x'||cWorkWing==0)
			jgzj=xWingXZhunJu;
		else
			jgzj=xWingYZhunJu;
	}
	else if(library)
		jgzj=library->GetAngleZJ(GetWidth());
	else
	{
		jgzj.g=ftoi(GetWidth()*0.5);
		jgzj.g1=jgzj.g2=jgzj.g3=0;
	}
	return jgzj;
}
int CLDSLineAngle::GetWingYG(int styleG0123)
{
	JGZJ jgzj;
	if(m_bEnableTeG)
		jgzj=xWingYZhunJu;
	else
	{
		if(IsUnequalAngle())
			getjgzj(jgzj,size_height);
		else 
			getjgzj(jgzj,size_wide);
	}
	if(styleG0123==0)
		return jgzj.g;
	else if(styleG0123==1)
		return jgzj.g1;
	else if(styleG0123==2)
		return jgzj.g2;
	else if(styleG0123==3)
		return jgzj.g3;
	else
		return 0;
}

bool CLDSLineAngle::IsWeldPart()
{
	if (m_bWeldPart)
		return true;
	else if(BelongModel()->FromPartHandle(m_hPartWeldParent))
		return true;
	else
		return false;
}
//<DEVELOP_PROCESS_MARK nameId="CLDSLineAngle::CalCutAngleByAngle">
//����ֵΪtrue��ʾΪ�������ӣ�falseΪ�ǹ�������
//angle1_wing,angle2_wing��1:X֫���棻0:Y֫����
bool CLDSLineAngle::CoplanarConnectDistinguish(CLDSLineAngle* pAngle1,CLDSLineAngle* pAngle2,
	int* angle1_wing,int* angle2_wing,BOOL* same_direct/*=NULL*/,double justify/*=0.5*/)
{
	//mx(y)--main jg's x(y) wing rx(y) ray jg's x(y) wing
	if(pAngle1==NULL || pAngle2==NULL)
		return false;
	double ddmx_rx,ddmx_ry,ddmy_rx,ddmy_ry,ddmax=0;
	ddmx_rx = dot_prod(pAngle1->get_norm_x_wing(),pAngle2->get_norm_x_wing());
	ddmx_ry = dot_prod(pAngle1->get_norm_x_wing(),pAngle2->get_norm_y_wing());
	ddmy_rx = dot_prod(pAngle1->get_norm_y_wing(),pAngle2->get_norm_x_wing());
	ddmy_ry = dot_prod(pAngle1->get_norm_y_wing(),pAngle2->get_norm_y_wing());
	BOOL same_wingnorm=TRUE;
	int coplanar_wing1,coplanar_wing2;
	if(fabs(ddmx_rx)>=fabs(ddmx_ry)&&fabs(ddmx_rx)>=fabs(ddmy_rx)&&fabs(ddmx_rx)>=fabs(ddmy_ry))
	{	//���Ǹ�X֫�����߽Ǹ�X֫����
		if(ddmx_rx<0)	//֫���߷����෴,�����н���֫
			same_wingnorm=FALSE;//return false;
		coplanar_wing1 = TRUE;
		coplanar_wing2 = TRUE;
		ddmax=fabs(ddmx_rx);
	}
	else if(fabs(ddmx_ry)>=fabs(ddmx_rx)&&fabs(ddmx_ry)>=fabs(ddmy_rx)&&fabs(ddmx_ry)>=fabs(ddmy_ry))
	{	//���Ǹ�X֫�����߽Ǹ�Y֫����
		if(ddmx_ry<0)	//֫���߷����෴,�����н���֫
			same_wingnorm=FALSE;//return false;
		coplanar_wing1 = TRUE;
		coplanar_wing2 = FALSE;
		ddmax=fabs(ddmx_ry);
	}
	else if(fabs(ddmy_rx)>=fabs(ddmx_rx)&&fabs(ddmy_rx)>=fabs(ddmx_ry)&&fabs(ddmy_rx)>=fabs(ddmy_ry))
	{	//���Ǹ�Y֫�����߽Ǹ�X֫����
		if(ddmy_rx<0)	//֫���߷����෴,�����н���֫
			same_wingnorm=FALSE;//return false;
		coplanar_wing1 = FALSE;
		coplanar_wing2 = TRUE;
		ddmax=fabs(ddmy_rx);
	}
	else if(fabs(ddmy_ry)>=fabs(ddmx_rx)&&fabs(ddmy_ry)>=fabs(ddmx_ry)&&fabs(ddmy_ry)>=fabs(ddmy_rx))
	{	//���Ǹ�Y֫�����߽Ǹ�Y֫����
		if(ddmy_ry<0)	//֫���߷����෴,�����н���֫
			same_wingnorm=FALSE;//return false;
		coplanar_wing1 = FALSE;
		coplanar_wing2 = FALSE;
		ddmax=fabs(ddmy_ry);
	}
	if(ddmax<justify)
		return false;
	//TODO: ����Ӧ������Ǹ�֫��ƽ�У�������������ж�
	//...
	if(angle1_wing)
		*angle1_wing=coplanar_wing1;
	if(angle2_wing)
		*angle2_wing=coplanar_wing2;
	if(same_direct)
		*same_direct=same_wingnorm;
	return true;
}
//</DEVELOP_PROCESS_MARK>

//<DEVELOP_PROCESS_MARK nameId="CLDSLineAngle::CalCutAngleInfo">
//�����϶: vertex_dist		���߽Ǹ�λ�����Ǹ����ʱʹ�ö����϶ֵ	ͨ������library->GetVertexDist();��� wht 10-07-02
//�нǼ�϶: cut_angle_space �нǼ�϶Ĭ��ȡ2/3*r(�Ǹ���Բ���뾶) ���û�ָ���нǼ�϶�Ҵ���2/3*rʱʹ���û�����ֵ  
int GetCutSpaceFromUI(CLDSLineAngle* pAngle,bool bStart,int (*InputCollideDist)(int dist),double spaceInnerR=0)
{
	int clearance=0;
	if(bStart)		//�û�����ֵ����0ʱȡ�û�����ֵ
		clearance=ftoi(pAngle->desStartOdd.m_fCollideDist);
	else
		clearance=ftoi(pAngle->desEndOdd.m_fCollideDist);
	if(clearance<=0)
		clearance=CLDSPart::library!=NULL?CLDSPart::library->GetVertexDist():5;
	if(spaceInnerR>0)	//����ΰ��������ΰ����,�Ǹ��ڲ��н�ʱӦ��ȫ����Բ���нǼ�϶Ϊ�ж�����,��Ӧ�ٿ��Ƕ�����ײ��϶ wjh-2016.9.7
		clearance=(int)ceil(spaceInnerR);//max(clearance,ftoi(spaceInnerR));
	if(InputCollideDist!=NULL)
	{
		clearance=InputCollideDist(ftoi(clearance));
		if(bStart)		//�û�����ֵ����0ʱȡ�û�����ֵ
			pAngle->desStartOdd.m_fCollideDist=clearance;
		else
			pAngle->desEndOdd.m_fCollideDist=clearance;
	}
	return clearance;
}
BOOL VerifyAngleCuttingData(CLDSLineAngle* pAngle,BYTE cbVerifyScope,double minCutWidth,double maxIgnoreCutWidth);
bool CLDSLineAngle::CalCutAngleInfo(CLDSLineAngle *pCutAngle1,CLDSLineAngle *pCutAngle2,bool bStart,int (*InputCollideDist)(int dist),int cut_angle_space/*=0*/)
{
	if(m_bUserSpecStartCutAngle&&bStart)
		return false;	//�ֶ�ָ���н���Ϣ����������Ƽ���
	else if(m_bUserSpecEndCutAngle&&!bStart)
		return false;	//�ֶ�ָ���н���Ϣ����������Ƽ���
	if(cut_angle_space!=0&&bStart)
		desStartOdd.m_fCollideDist=abs(cut_angle_space);
	else if(cut_angle_space!=0&&!bStart)
		desEndOdd.m_fCollideDist=abs(cut_angle_space);
	int vertex_dist=5;	//�����϶
	if(library)
		vertex_dist=library->GetVertexDist();
// һ���ж����ǸֵĹ���֫
	BOOL bInMainWingX,bCutRayWingX;	//�и�λ�����Ǹ�X֫��,�и����߽Ǹ�X֫
	//0:����ʼ�� 1����ֹ��;2֫����ʼ�� 3֫����ֹ��
	f3dPoint vec,vertex[4];
	//ber_line:���߽Ǹ�����;ray_end_line:���߽Ǹֶ�ͷ��Ƥ����; 
	// wing_line���߽Ǹ�֫����; cut_line���Ǹֶ����߽Ǹֵ��и���
	f3dLine ber_line,ray_end_line,wing_line, cut_line;
	UCS_STRU main_cs;	//�н���֫����ʱ�Ĳ�������ϵ--ѡ���Ǹ��������ϵ
	double wide,thick;

	if(bStart)	//���߽Ǹ���ʼ�ڵ������Ǹ�����
	{
		if(pCutAngle1)//&&desStartOdd.m_hRefPart1<0x20)
			desStartOdd.m_hRefPart1=pCutAngle1->handle;
		//��������ͷ��ײ��׼Ϊ�ֹ�ʱ,�˴�������ܵ��¶�ʧ����ͷ�������ݸֹ� wjh-2013.12.22
		//else
		//	desStartOdd.m_hRefPart1=0;
		if(pCutAngle2)//&&desStartOdd.m_hRefPart2<0x20)
			desStartOdd.m_hRefPart2=pCutAngle2->handle;
		//else
		//	desStartOdd.m_hRefPart2=0;
		//�����ǰ�������и��¼
		cut_wing_para[0][0]=cut_wing_para[0][1]=cut_wing_para[0][2] = 0;
		cut_wing[0]=0;	//���ʼ���н�
		cut_angle[0][0]=cut_angle[1][0]=cut_angle[0][1]=cut_angle[1][1]=0;	//���ʼ����֫

	}
	else					//���߽Ǹ���ֹ�ڵ������Ǹ�����
	{	
		if(pCutAngle1)//&&desEndOdd.m_hRefPart1<0x20)
			desEndOdd.m_hRefPart1=pCutAngle1->handle;
		//��������ͷ��ײ��׼Ϊ�ֹ�ʱ,�˴�������ܵ��¶�ʧ����ͷ�������ݸֹ� wjh-2013.12.22
		//else
		//	desEndOdd.m_hRefPart1=0;
		if(pCutAngle2)//&&desEndOdd.m_hRefPart2<0x20)
			desEndOdd.m_hRefPart2=pCutAngle2->handle;
		//��������ͷ��ײ��׼Ϊ�ֹ�ʱ,�˴�������ܵ��¶�ʧ����ͷ�������ݸֹ� wjh-2013.12.22
		//else
		//	desEndOdd.m_hRefPart2=0;
		//�����ǰ�������и��¼
		cut_wing_para[1][0]=cut_wing_para[1][1]=cut_wing_para[1][2] = 0;
		cut_wing[1]=0;	//����ն��н�
		cut_angle[2][0]=cut_angle[3][0]=cut_angle[2][1] =cut_angle[3][1]=0;	//����ն���֫
	}
	bool bHasCut = false;
	GEPOINT lenStdVec=End()-Start();
	normalize(lenStdVec);
	for(int i=0;i<2;i++)
	{
		CLDSLineAngle *pMainAngle=NULL;
		if(i==0)
			pMainAngle=pCutAngle1;
		else
			pMainAngle=pCutAngle2;
		if(pMainAngle==NULL)
			continue;
		GEPOINT lenRayStdVec=pMainAngle->End()-pMainAngle->Start();
		normalize(lenRayStdVec);
		bool verticalToDatum=false;
		bool outputLogError=false;
		if(fabs(lenStdVec*lenRayStdVec)<0.001)
			verticalToDatum=true;	//ȡֵ0.001�ǿ��ǵ��Ǹ�֫�����500��ʱ���򲻴�ֱ�����������н�ֵ��500*0.001=0.5mm wjh-2015.10.9
		BOOL same_direct=0;
		bool coplanar=CoplanarConnectDistinguish(pMainAngle,this,&bInMainWingX,&bCutRayWingX,&same_direct);
		if(!coplanar && InputCollideDist!=NULL)						//���Ǹ�֫�������������ݴ���
#ifdef AFX_TARG_ENU_ENGLISH
			throw "These two angles don't have coplanar working leg,error calculate cutting leg";
#else
			throw "���Ǹ�û�й���Ĺ���֫,�����н���֫����";
#endif
		else if(!coplanar || !same_direct)
			continue;	//�й���֫����֫���߷����෴,�����н���֫
		//else	//������֫����ͬ�򣬼�ͬΪ������
		// ���������Ǹֹ���֫Ϊ��׼�����������ϵmain_cs���Ա�����ʱ����ת����
		//�����Ǹֵ��������ϵ,��Ϊ��Ʋ�������ϵ
		main_cs.origin = pMainAngle->Start();
		//����Ǹ�Z�᷽��
		Sub_Pnt( main_cs.axis_z, pMainAngle->End(), pMainAngle->Start());
		if(bInMainWingX)	//X֫Ϊ����֫,��X֫Ϊ��׼�����������ϵ
		{
			main_cs.axis_x = main_cs.axis_z^pMainAngle->get_norm_x_wing();
			main_cs.axis_y = main_cs.axis_z^main_cs.axis_x;
			normalize(main_cs.axis_x);
			normalize(main_cs.axis_y);
			normalize(main_cs.axis_z);
		}
		else				//Y֫Ϊ����֫,��Y֫Ϊ��׼�����������ϵ
		{
			main_cs.axis_y = pMainAngle->get_norm_y_wing()^main_cs.axis_z;
			main_cs.axis_x = main_cs.axis_y^main_cs.axis_z;
			normalize(main_cs.axis_x);
			normalize(main_cs.axis_y);
			normalize(main_cs.axis_z);
		}
		// �����������߽Ǹֹ���֫���༰�˱�Ե��L1.L2.EL���Ա������и�λ��
		wide = GetWidth();
		thick= GetThick();
		Sub_Pnt(vec,End(),Start());
		normalize(vec);
		vertex[0] = Start();	//���߽Ǹ�������ʼ��
		vertex[1] = End();		//���߽Ǹ�������ֹ��
		Sub_Pnt(vertex[0],vertex[0],vec*startOdd());
		Add_Pnt(vertex[1],vertex[1],vec*endOdd());
		if(bCutRayWingX)//�и����߽Ǹ�X֫
		{
			vertex[2] = vertex[0]+wide*GetWingVecX();
			vertex[3] = vertex[1]+wide*GetWingVecX();
		}
		else			//�и����߽Ǹ�Y֫
		{
			vertex[2] = vertex[0]+wide*GetWingVecY();
			vertex[3] = vertex[1]+wide*GetWingVecY();
		}

		for(int i=0;i<4;i++)
		{
			//�����߽ǸֵĶ���ת������������ϵ��
			coord_trans(vertex[i],main_cs,FALSE);
			if(bInMainWingX)	//�и�λ�����Ǹ�X֫��ʱͶӰ��YֵΪ��
				vertex[i].y = 0;
			else				//�и�λ�����Ǹ�Y֫��ʱͶӰ��XֵΪ��
				vertex[i].x = 0;
		}

		ber_line.startPt = vertex[0];
		ber_line.endPt = vertex[1];
		if(bStart)	//���߽Ǹ���ʼ�ڵ������Ǹ�����
		{
			ray_end_line.startPt = vertex[0];
			ray_end_line.endPt = vertex[2];
		}
		else					//���߽Ǹ���ֹ�ڵ������Ǹ�����
		{	
			ray_end_line.startPt = vertex[1];
			ray_end_line.endPt = vertex[3];
		}
		wing_line.startPt = vertex[2];
		wing_line.endPt = vertex[3];
		// �ġ��ж����߽Ǹַ�λ
		wide = pMainAngle->GetWidth();
		thick = pMainAngle->GetThick();
		if(bStart)
			Sub_Pnt(vec,End(),Start());
		else
			Sub_Pnt(vec,Start(),End());
		double dd;
		if(bInMainWingX)
			dd = main_cs.axis_x*vec;
		else
			dd = main_cs.axis_y*vec;
		int collide_dist=0;	//��ײ��϶
		if(dd<=0)	//���߽Ǹ��и�λ�����Ǹ���� 
		{	
			collide_dist=GetCutSpaceFromUI(this,bStart,InputCollideDist);
			if(bInMainWingX)
			{
				cut_line.startPt.Set(-collide_dist,0,-pMainAngle->startOdd());
				cut_line.endPt.Set(-collide_dist,0,pMainAngle->GetLength()-pMainAngle->startOdd());
			}
			else
			{
				cut_line.startPt.Set(0,-collide_dist,-pMainAngle->startOdd());
				cut_line.endPt.Set(0,-collide_dist,pMainAngle->GetLength()-pMainAngle->startOdd());
			}
		}
		else		//���߽Ǹ��и�λ�����Ǹ��ڲ�
		{
			f3dPoint pick;
			//��ѯ���Ǹֵ���Բ���뾶
			double r = FindJg_r(wide);
			if(bStart)	//���߽Ǹ������и�
				pick = Start();
			else				//���߽Ǹ��յ���и�
				pick = End();
			coord_trans(pick,main_cs,FALSE);
			//�������Ǹ��������ϵ�µ��и���
			//������֫���Ĵ�СӦ����λ�ö�����һ�����Ǹ�Բ����r/3��������
			//�ɲ���֫������Ǹ�Բ����r/3������Ӧ����֫�������ߴ硣
			if(bInMainWingX)
			{	//�ж��и���
				//if(pick.y<thick*2.0/3.0)	//����֫����Ϊ�и���2/3ֻ��һ�����Ԥ��ģ����
				if(pick.y<thick*0.5)	//��Χ��Ϊ0.5,������������б�ĸ������п��������ԭ�����нǴ��� wjh-2013.10.13
				{	
					collide_dist=GetCutSpaceFromUI(this,bStart,InputCollideDist);
					cut_line.startPt.Set(wide+collide_dist,0,-pMainAngle->startOdd());
					cut_line.endPt.Set(wide+collide_dist,0,pMainAngle->GetLength()-pMainAngle->startOdd());
				}
				else							//�ǹ���֫�ڲ�Ϊ�и���
				{	
					int space = ftoi(pMainAngle->GetCutAngleCoefR()*r)+pMainAngle->m_nCutAngleSpaceOdd;
					if(cut_angle_space<0)	//��ʾ����ײ��϶Ϊ�нǼ�������,���������нǼ�϶����ʱ,��������ײ��϶Ϊ�нǼ������� wjh-2016.12.17
					{
						if(bStart)	//��ײ��϶����Բ����ͬʱӦ�Զ���Ϊָ���н�ֵ,�Ա�������Զ����ʱ�ٸ�������ͷ
							m_bUserSpecStartCutAngle=(space!=abs(cut_angle_space));
						else
							m_bUserSpecEndCutAngle  =(space!=abs(cut_angle_space));
						space=abs(cut_angle_space);
					}
					collide_dist=GetCutSpaceFromUI(this,bStart,InputCollideDist,space);
					cut_line.startPt.Set(thick+collide_dist,0,-pMainAngle->startOdd());
					cut_line.endPt.Set(thick+collide_dist,0,pMainAngle->GetLength()-pMainAngle->startOdd());
				}
			}
			else
			{	//�ж��и���
				//if(pick.x<thick*2.0/3)		//����֫����Ϊ�и���
				if(pick.x<thick*0.5)		//����֫����Ϊ�и���
				{	
					collide_dist=GetCutSpaceFromUI(this,bStart,InputCollideDist);
					cut_line.startPt.Set(0,wide+collide_dist,-pMainAngle->startOdd());
					cut_line.endPt.Set(0,wide+collide_dist,pMainAngle->GetLength()-pMainAngle->startOdd());
				}
				else							//�ǹ���֫�ڲ�Ϊ�и���
				{	
					int space = ftoi(pMainAngle->GetCutAngleCoefR()*r)+pMainAngle->m_nCutAngleSpaceOdd;
					if(cut_angle_space<0)	//��ʾ����ײ��϶Ϊ�нǼ�������,���������нǼ�϶����ʱ,��������ײ��϶Ϊ�нǼ������� wjh-2016.12.17
					{
						if(bStart)	//��ײ��϶����Բ����ͬʱӦ�Զ���Ϊָ���н�ֵ,�Ա�������Զ����ʱ�ٸ�������ͷ
							m_bUserSpecStartCutAngle=(space!=abs(cut_angle_space));
						else
							m_bUserSpecEndCutAngle  =(space!=abs(cut_angle_space));
						space=abs(cut_angle_space);
					}
					collide_dist=GetCutSpaceFromUI(this,bStart,InputCollideDist,space);
					cut_line.startPt.Set(0,thick+collide_dist,-pMainAngle->startOdd());
					cut_line.endPt.Set(0,thick+collide_dist,pMainAngle->GetLength()-pMainAngle->startOdd());
				}
			}
		}

		//�����н���֫��
		f3dPoint inters;	//����
		double dist;
		bool bCutAngle = false;
		//if(Int3dpl(ber_line,cut_line,inters)<=0) //replaced by next line for cut accurately wjh 2003.02.25
		int rslt=Int3dll(ber_line,cut_line,inters);
		if(rslt==1&&verticalToDatum)
		{
			outputLogError=true;
#ifdef AFX_TARG_ENU_ENGLISH
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "Angle 0x%X vertical to cut reference angle 0x%X��although the collision, "
				"but not suitable for angle cutting, proposed to adjust the rod ends!", handle, pMainAngle->handle);
#else
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "0x%X�Ǹ����нǼ�����սǸ�0x%X��ֱ���䷢����ײ���������нǴ��������������ͷ!", handle, pMainAngle->handle);
#endif
		}
		else if(rslt==1)	//�������и����ཻ
			//	return FALSE;		//�������и���ƽ�в��ཻʱ���������н���֫
			//if(ber_line.PtInLine(inters)>0)	//�������и����ཻ
		{
			if(bStart)
			{
				dist = DISTANCE(inters,vertex[0]);
				cut_wing_para[0][0] = (int)ceil(dist) ;	//������ȡ����
				//��֫��ȡ����
				//�����10mm��϶�Ǻ�չͬ�߹���˵��  WJH--2001.09.14
				cut_wing_para[0][2] = (int)ceil(dist);//size_wide-size_thick;+10

				if(bCutRayWingX)
					cut_wing[0] = 2;	//Y֫Ϊ�н���֫��Y֫ȫ�����У�
				else
					cut_wing[0] = 1;	//X֫Ϊ�н���֫��X֫ȫ�����У�
				bCutAngle = true;
			}
			else
			{
				dist = DISTANCE(inters,vertex[1]);
				cut_wing_para[1][0] = (int)ceil(dist);	//������ȡ����
				//��֫��ȡ����
				//�����10mm��϶�Ǻ�չͬ�߹���˵��  WJH--2001.09.14
				cut_wing_para[1][2] = (int)ceil(dist);//size_wide-size_thick;+10
				if(bCutRayWingX)
					cut_wing[1] = 2;	//Y֫Ϊ�н���֫��Y֫ȫ�����У�
				else
					cut_wing[1] = 1;	//X֫Ϊ�н���֫��X֫ȫ�����У�
				bCutAngle = true;
			}
		}
		//if(Int3dpl(ray_end_line,cut_line,inters)<=0)
		//	return FALSE;
		rslt=Int3dll(ray_end_line,cut_line,inters);
		if(rslt==1&&verticalToDatum)
		{
			if(!outputLogError)
			{
				outputLogError=true;
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("Angle 0x%X vertical to cut reference angle 0x%X��although the collision, "
					"but not suitable for angle cutting, proposed to adjust the rod ends!",handle,pMainAngle->handle);
#else
				Log2File()->Log("0x%X�Ǹ����нǼ�����սǸ�0x%X��ֱ���䷢����ײ���������нǴ��������������ͷ!",handle,pMainAngle->handle);
#endif
			}
		}
		else if(rslt==1)
			//if(ray_end_line.PtInLine(inters)>0)	//���߽Ǹֶ�ͷ�����и����ཻ
		{
			if(bStart)
			{
				if(bCutAngle)
				{
					dist = DISTANCE(inters,vertex[0]);
					cut_wing_para[0][1] = (int)ceil(dist);
				}
				else
				{
					dist = DISTANCE(inters,vertex[2]);
					if(bCutRayWingX)	//��ʼ��X֫������ȡ��
						cut_angle[0][1] = (int)ceil(dist);
					else				//��ʼ��Y֫������ȡ��
						cut_angle[1][1] = (int)ceil(dist);
					bCutAngle = true;
				}
			}
			else
			{
				if(bCutAngle)
				{
					dist = DISTANCE(inters,vertex[1]);
					cut_wing_para[1][1] = (int)ceil(dist);
				}
				else
				{
					dist = DISTANCE(inters,vertex[3]);
					if(bCutRayWingX)	//��ֹ��X֫������ȡ��
						cut_angle[2][1] = (int)ceil(dist);
					else				//��ֹ��Y֫������ȡ��
						cut_angle[3][1] = (int)ceil(dist);
					bCutAngle = true;
				}
			}
		}
		//else
		//	return FALSE;
		//if(Int3dpl(wing_line,cut_line,inters)<=0)	//֫�������и����ཻ
		//	return FALSE;
		//if(wing_line.PtInLine(inters)>0)
		rslt=Int3dll(wing_line,cut_line,inters);
		if(rslt==1&&verticalToDatum)
		{
			if(!outputLogError)
			{
				outputLogError=true;
#ifdef AFX_TARG_ENU_ENGLISH
				Log2File()->Log("Angle 0x%X vertical to cut reference angle 0x%X��although the collision, "
					"but not suitable for angle cutting, proposed to adjust the rod ends!",handle,pMainAngle->handle);
#else
				Log2File()->Log("0x%X�Ǹ����нǼ�����սǸ�0x%X��ֱ���䷢����ײ���������нǴ��������������ͷ!",handle,pMainAngle->handle);
#endif
			}
		}
		else if(rslt==1)	//֫�������и����ཻ
		{
			if(bStart)
			{
				dist = DISTANCE(inters,vertex[2]);
				if(bCutRayWingX)	//��ʼ��X֫������ȡ��
					cut_angle[0][0] = (int)ceil(dist);
				else				//��ʼ��Y֫������ȡ��
					cut_angle[1][0] = (int)ceil(dist);
				bCutAngle = true;
			}
			else
			{
				dist = DISTANCE(inters,vertex[3]);
				if(bCutRayWingX)	//��ֹ��X֫������ȡ��
					cut_angle[2][0] = (int)ceil(dist);
				else				//��ֹ��Y֫������ȡ��
					cut_angle[3][0] = (int)ceil(dist);
				bCutAngle = true;
			}
		}
		bHasCut|=bCutAngle;
	}
	if(bHasCut)
	{	//���и�
		const BYTE START_WING_X = 0x01;
		const BYTE START_WING_Y = 0x02;
		const BYTE END_WING_X   = 0x04;
		const BYTE END_WING_Y   = 0x08;
		double minCutSpace=library?library->GetMinCutAngleLen():0;	//��С�и��϶
		double fMaxIgnoreCutSpace=library?library->GetMaxIgnoreCutAngleLen():0;
		if(bStart)
			bHasCut|=(VerifyAngleCuttingData(this,START_WING_X|START_WING_Y,minCutSpace,fMaxIgnoreCutSpace)==TRUE);
		else
			bHasCut|=(VerifyAngleCuttingData(this,END_WING_X|END_WING_Y,minCutSpace,fMaxIgnoreCutSpace)==TRUE);
	}
	SetModified(bHasCut);
	return bHasCut;
}
//</DEVELOP_PROCESS_MARK>
//<DEVELOP_PROCESS_MARK nameId="CLDSLineAngle::CalCutAngleByPlane">
//ָ���и���ԽǸֽ����и����
BOOL VerifyAngleCuttingData(CLDSLineAngle* pAngle,BYTE cbVerifyScope,double minCutWidth,double maxIgnoreCutWidth)
{
	const BYTE START_WING_X = 0x01;
	const BYTE START_WING_Y = 0x02;
	const BYTE END_WING_X   = 0x04;
	const BYTE END_WING_Y   = 0x08;
	bool hasCut=FALSE;
	if(cbVerifyScope&START_WING_X)
	{
		if(pAngle->cut_wing[0]==1)  //��֫
		{
			if(pAngle->cut_wing_para[0][2]<=maxIgnoreCutWidth)
			{
				pAngle->cut_wing[0]=0;
				pAngle->cut_wing_para[0][0]=0;
				pAngle->cut_wing_para[0][1]=0;
				pAngle->cut_wing_para[0][2]=0;
				pAngle->m_bUserSpecStartCutAngle=FALSE;;
			}
			else
			{
				if(pAngle->cut_wing_para[0][1]<=pAngle->size_thick)
					pAngle->cut_wing_para[0][1]=pAngle->size_thick+1;
				if(pAngle->cut_wing_para[0][2]>0&&pAngle->cut_wing_para[0][2]<minCutWidth)	//֫������ȡ���Ȳ���Ϊ0��
					pAngle->cut_wing_para[0][2]=minCutWidth;
				double minBerCutWidth=pAngle->cut_wing_para[0][2]*pAngle->size_thick/pAngle->size_wide;
				if(pAngle->cut_wing_para[0][0]>0&&pAngle->cut_wing_para[0][0]<minBerCutWidth)     //���߲���Ϊ0��0��ʾû��
					pAngle->cut_wing_para[0][0]=minBerCutWidth;
				hasCut=TRUE;
			}
		}
		else if(pAngle->cut_angle[0][0]!=0||pAngle->cut_angle[0][1]!=0)  //�����н�
		{
			double fMinWingWidth= min(pAngle->cut_angle[0][0],pAngle->cut_angle[0][1]);
			if (fMinWingWidth<=maxIgnoreCutWidth)
			{
				pAngle->cut_angle[0][0]=0;
				pAngle->cut_angle[0][1]=0;
				pAngle->m_bUserSpecStartCutAngle=FALSE;
			}
			else
			{
				if(pAngle->cut_angle[0][0]>0&&pAngle->cut_angle[0][0]<minCutWidth)
					pAngle->cut_angle[0][0]=minCutWidth;
				if(pAngle->cut_angle[0][1]>0&&pAngle->cut_angle[0][1]<minCutWidth)
					pAngle->cut_angle[0][1]=minCutWidth;
				hasCut=TRUE;
			}
		}
	}
	if(cbVerifyScope&START_WING_Y)
	{
		if(pAngle->cut_wing[0]==2)
		{
			if(pAngle->cut_wing_para[0][2]<=maxIgnoreCutWidth)
			{
				pAngle->cut_wing[0]=0;
				pAngle->cut_wing_para[0][0]=0;
				pAngle->cut_wing_para[0][1]=0;
				pAngle->cut_wing_para[0][2]=0;
				pAngle->m_bUserSpecStartCutAngle=FALSE;
			}
			else
			{
				if(pAngle->cut_wing_para[0][1]<=pAngle->size_thick)
					pAngle->cut_wing_para[0][1]=pAngle->size_thick+1;
				if(pAngle->cut_wing_para[0][2]>0&&pAngle->cut_wing_para[0][2]<minCutWidth)		//֫������ȡ���Ȳ���Ϊ0��
					pAngle->cut_wing_para[0][2]=minCutWidth;
				double minBerCutWidth=pAngle->cut_wing_para[0][2]*pAngle->size_thick/pAngle->size_wide;
				if(pAngle->cut_wing_para[0][0]>0&&pAngle->cut_wing_para[0][0]<minBerCutWidth)		//���߲���Ϊ0��0��ʾû��
					pAngle->cut_wing_para[0][0]=minBerCutWidth;
				hasCut=TRUE;
			}
		}
		else if(pAngle->cut_angle[1][1]!=0||pAngle->cut_angle[1][0]!=0)	 //�����н�
		{
			double fMinWingWidth=min(pAngle->cut_angle[1][0],pAngle->cut_angle[1][1]);
			if (fMinWingWidth<=maxIgnoreCutWidth)
			{
				pAngle->cut_angle[1][0]=0;
				pAngle->cut_angle[1][1]=0;
				pAngle->m_bUserSpecStartCutAngle=FALSE;
			}
			else
			{
				if(pAngle->cut_angle[1][0]>0&&pAngle->cut_angle[1][0]<minCutWidth)
					pAngle->cut_angle[1][0]=minCutWidth;
				if(pAngle->cut_angle[1][1]>0&&pAngle->cut_angle[1][1]<minCutWidth)
					pAngle->cut_angle[1][1]=minCutWidth;
				hasCut=TRUE;
			}
		}
	}
	if(cbVerifyScope&END_WING_Y)
	{
		if(pAngle->cut_wing[1]==2)	//��֫
		{
			if(pAngle->cut_wing_para[1][2]<=maxIgnoreCutWidth)
			{
				pAngle->cut_wing[1]=0;
				pAngle->cut_wing_para[1][0]=0;
				pAngle->cut_wing_para[1][1]=0;
				pAngle->cut_wing_para[1][2]=0;
				pAngle->m_bUserSpecEndCutAngle=FALSE;
			}
			else
			{
				if(pAngle->cut_wing_para[1][1]<=pAngle->size_thick)
					pAngle->cut_wing_para[1][1]=pAngle->size_thick+1;
				if(pAngle->cut_wing_para[1][2]>0&&pAngle->cut_wing_para[1][2]<minCutWidth)	//֫������ȡ���Ȳ���Ϊ0��
					pAngle->cut_wing_para[1][2]=minCutWidth;
				double minBerCutWidth=pAngle->cut_wing_para[1][2]*pAngle->size_thick/pAngle->size_wide;
				if(pAngle->cut_wing_para[1][0]>0&&pAngle->cut_wing_para[1][0]<minBerCutWidth)	//���߲���Ϊ0��0��ʾû��
					pAngle->cut_wing_para[1][0]=minBerCutWidth;
				hasCut = TRUE;
			}
		}
		else if(pAngle->cut_angle[3][1]!=0||pAngle->cut_angle[3][0]!=0)	//�н�
		{
			double fMinWingWidth=min(pAngle->cut_angle[3][0],pAngle->cut_angle[3][1]);
			if (fMinWingWidth<=maxIgnoreCutWidth)
			{
				pAngle->cut_angle[3][0]=0;
				pAngle->cut_angle[3][1]=0;
				pAngle->m_bUserSpecEndCutAngle=FALSE;
			}
			else
			{
				if(pAngle->cut_angle[3][0]>0&&pAngle->cut_angle[3][0]<minCutWidth)
					pAngle->cut_angle[3][0]=minCutWidth;
				if(pAngle->cut_angle[3][1]>0&&pAngle->cut_angle[3][1]<minCutWidth)
					pAngle->cut_angle[3][1]=minCutWidth;
				hasCut=TRUE;
			}
		}
	}
	if(cbVerifyScope&END_WING_X)
	{
		if(pAngle->cut_wing[1]==1)	//��֫
		{
			if(pAngle->cut_wing_para[1][2]<=maxIgnoreCutWidth)
			{
				pAngle->cut_wing[1]=0;
				pAngle->cut_wing_para[1][0]=0;
				pAngle->cut_wing_para[1][1]=0;
				pAngle->cut_wing_para[1][2]=0;
				pAngle->m_bUserSpecEndCutAngle=FALSE;
			}
			else
			{
				if(pAngle->cut_wing_para[1][1]<=pAngle->size_thick)
					pAngle->cut_wing_para[1][1]=pAngle->size_thick+1;
				if(pAngle->cut_wing_para[1][2]>0&&pAngle->cut_wing_para[1][2]<minCutWidth)	//֫������ȡ���Ȳ���Ϊ0��
					pAngle->cut_wing_para[1][2]=minCutWidth;
				double minBerCutWidth=pAngle->cut_wing_para[1][2]*pAngle->size_thick/pAngle->size_wide;
				if(pAngle->cut_wing_para[1][0]>0&&pAngle->cut_wing_para[1][0]<minBerCutWidth)	//���߲���Ϊ0��0��ʾû��
					pAngle->cut_wing_para[1][0]=minBerCutWidth;
				hasCut = TRUE;
			}
		}
		else if(pAngle->cut_angle[2][0]!=0||pAngle->cut_angle[2][1]!=0) //�н�
		{
			double fMinWingWidth=min(pAngle->cut_angle[2][0],pAngle->cut_angle[2][1]);
			if (fMinWingWidth<=maxIgnoreCutWidth)
			{
				pAngle->cut_angle[2][0]=0;
				pAngle->cut_angle[2][1]=0;
				pAngle->m_bUserSpecEndCutAngle=FALSE;
			}
			else
			{
				if(pAngle->cut_angle[2][0]>0&&pAngle->cut_angle[2][0]<minCutWidth)
					pAngle->cut_angle[2][0]=minCutWidth;
				if(pAngle->cut_angle[2][1]>0&&pAngle->cut_angle[2][1]<minCutWidth)
					pAngle->cut_angle[2][1]=minCutWidth;
				hasCut=TRUE;
			}
		}
	}
	return hasCut;
}
BOOL CLDSLineAngle::CalCutAngleInfo(f3dPoint face_pick,f3dPoint face_norm,int (*InputCollideDist)(int dist),int cut_angle_space/* =0 */)
{
	f3dPoint ray_vec=End()-Start();
	normalize(ray_vec);
	f3dLine ber_line,wing_x_edge,wing_y_edge,wing_x_profile,wing_y_profile;
	ber_line.startPt=Start()-ray_vec*startOdd();
	ber_line.endPt  =End()+ray_vec*endOdd();
	wing_x_edge.startPt=ber_line.startPt+GetWingVecX()*GetWidth();
	wing_x_edge.endPt  =ber_line.endPt+GetWingVecX()*GetWidth();
	wing_y_edge.startPt=ber_line.startPt+GetWingVecY()*GetWidth();
	wing_y_edge.endPt  =ber_line.endPt+GetWingVecY()*GetWidth();
	//�����϶
	if(cut_angle_space<=0&&library)
		cut_angle_space=library->GetVertexDist();
	if(InputCollideDist)
		cut_angle_space=InputCollideDist(cut_angle_space);
	//�����и����ڽǸ�ʼ��һ�໹���ն�һ��
	BOOL is_start_end;
	f3dPoint inters;
	if(Int3dlf(inters,ber_line,face_pick,face_norm)!=1)
		return FALSE;
	if(DISTANCE(inters,ber_line.startPt)<=DISTANCE(inters,ber_line.endPt))
		is_start_end=TRUE;
	else
		is_start_end=FALSE;

	if(is_start_end)	//����ʼ���и�
	{
		if(ray_vec*face_norm>0)
			face_pick+=face_norm*cut_angle_space;
		else
			face_pick-=face_norm*cut_angle_space;
		wing_x_profile.startPt=ber_line.startPt;
		wing_x_profile.endPt=wing_x_edge.startPt;
		wing_y_profile.startPt=ber_line.startPt;
		wing_y_profile.endPt=wing_y_edge.startPt;
	}
	else
	{
		if(ray_vec*face_norm>0)
			face_pick-=face_norm*cut_angle_space;
		else
			face_pick+=face_norm*cut_angle_space;
		wing_x_profile.startPt=ber_line.endPt;
		wing_x_profile.endPt=wing_x_edge.endPt;
		wing_y_profile.startPt=ber_line.endPt;
		wing_y_profile.endPt=wing_y_edge.endPt;
	}
	//ͨ��ָ���и�����нǸ��и����ʾ�û�ָ����Ϣ
//	if(m_bUserSpecStartCutAngle&&is_start_end)
//	{
//#ifdef AFX_TARG_ENU_ENGLISH
//		logerr.Log("The cutting leg process information of 0X%X angle's start is user-defined,system ignores this��",handle);
//#else
//		logerr.Log("0X%X�Ǹ�ʼ�˵��нǹ�����Ϣ�趨Ϊ�û�ָ����ϵͳ�����˴˽Ǹ�ʼ�˵��нǲ�����",handle);
//#endif
//		return TRUE;		//�Ǹ�ʼ���ֶ�ָ���н���Ϣ����������Ƽ���
//	}
//	else if(m_bUserSpecEndCutAngle&&!is_start_end)
//	{
//#ifdef AFX_TARG_ENU_ENGLISH
//		logerr.Log("The cutting leg process information of 0X%X angle's end is user-defined,system ignores this��",handle);
//#else
//		logerr.Log("0X%X�Ǹ��ն˵��нǹ�����Ϣ�趨Ϊ�û�ָ����ϵͳ�����˴˽Ǹ��ն˵��нǲ�����",handle);
//#endif
//		return TRUE;		//�Ǹ��ն��ֶ�ָ���н���Ϣ����������Ƽ���
//	}
	//���ܵ��������
	f3dPoint vertex[5];
	BOOL  inters_valid[5]={FALSE};
	if(Int3dlf(vertex[0],ber_line,face_pick,face_norm)==1)
	{
		if(ber_line.PtInLine(vertex[0])==2)
			inters_valid[0]=TRUE;
	}
	if(Int3dlf(vertex[1],wing_x_edge,face_pick,face_norm)==1)
	{
		if(wing_x_edge.PtInLine(vertex[1])==2)
			inters_valid[1]=TRUE;
	}
	if(Int3dlf(vertex[2],wing_y_edge,face_pick,face_norm)==1)
	{
		if(wing_y_edge.PtInLine(vertex[2])==2)
			inters_valid[2]=TRUE;
	}
	if(Int3dlf(vertex[3],wing_x_profile,face_pick,face_norm)==1)
	{
		if(wing_x_profile.PtInLine(vertex[3])==2)
			inters_valid[3]=TRUE;
	}
	if(Int3dlf(vertex[4],wing_y_profile,face_pick,face_norm)==1)
	{
		if(wing_y_profile.PtInLine(vertex[4])==2)
			inters_valid[4]=TRUE;
	}
	BOOL bCut=FALSE;
	double minCutLen=0;	//��С�и��
	double maxIgnoreCutSpace=0;
	const BYTE START_WING_X = 0x01;
	const BYTE START_WING_Y = 0x02;
	const BYTE END_WING_X   = 0x04;
	const BYTE END_WING_Y   = 0x08;
	if (library)
	{
		minCutLen=library->GetMinCutAngleLen();
		maxIgnoreCutSpace=library->GetMaxIgnoreCutAngleLen();
	}
	if(inters_valid[0]&&inters_valid[1]&&inters_valid[2])
	{
		if(is_start_end)
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0X%X angle's start need cutting largely,proposals to solve the collision problem by oddment,system ignores the cutting leg of this angle's start��",handle);
#else
			logerr.Log("0X%X�Ǹ�ʼ����Ҫ���и���̫��,����ͨ����ͷ�������ײ�������⣬ϵͳ�����˴˽Ǹ�ʼ�˵��нǲ�����",handle);
#endif
		else
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("0X%X angle's end need cutting largely,proposals to solve the collision problem by oddment,system ignores the cutting leg of this angle's end��",handle);
#else
			logerr.Log("0X%X�Ǹ��ն���Ҫ���и���̫��,����ͨ����ͷ�������ײ�������⣬ϵͳ�����˴˽Ǹ��ն˵��нǲ�����",handle);
#endif
		return FALSE;
	}
	else if(inters_valid[0]&&inters_valid[1])	//����X֫��֫
	{
		if(is_start_end)
		{
			cut_wing[0]=1;
			cut_wing_para[0][0]=ftol(DISTANCE(vertex[0],ber_line.startPt));
			cut_wing_para[0][1]=ftol(DISTANCE(vertex[4],ber_line.startPt));
			cut_wing_para[0][2]=ftol(DISTANCE(vertex[1],wing_x_edge.startPt));
			bCut|=VerifyAngleCuttingData(this,START_WING_X,minCutLen,maxIgnoreCutSpace);
		}
		else
		{
			cut_wing[1]=1;
			cut_wing_para[1][0]=ftol(DISTANCE(vertex[0],ber_line.endPt));
			cut_wing_para[1][1]=ftol(DISTANCE(vertex[4],ber_line.endPt));
			cut_wing_para[1][2]=ftol(DISTANCE(vertex[1],wing_x_edge.endPt));
			bCut|=VerifyAngleCuttingData(this,END_WING_X,minCutLen,maxIgnoreCutSpace);
		}
	}
	else if(inters_valid[0]&&inters_valid[2])	//����Y֫��֫
	{
		if(is_start_end)
		{
			cut_wing[0]=2;
			cut_wing_para[0][0]=ftol(DISTANCE(vertex[0],ber_line.startPt));
			cut_wing_para[0][1]=ftol(DISTANCE(vertex[3],ber_line.startPt));
			cut_wing_para[0][2]=ftol(DISTANCE(vertex[2],wing_y_edge.startPt));
			bCut|=VerifyAngleCuttingData(this,START_WING_Y,minCutLen,maxIgnoreCutSpace);
		}
		else
		{
			cut_wing[1]=2;
			cut_wing_para[1][0]=ftol(DISTANCE(vertex[0],ber_line.endPt));
			cut_wing_para[1][1]=ftol(DISTANCE(vertex[3],ber_line.endPt));
			cut_wing_para[1][2]=ftol(DISTANCE(vertex[2],wing_y_edge.endPt));
			bCut|=VerifyAngleCuttingData(this,END_WING_Y,minCutLen,maxIgnoreCutSpace);
		}
	}
	else	//��������֫����
	{
		if(inters_valid[1]&&inters_valid[3])	//X֫�н�
		{
			if(is_start_end)
			{
				cut_angle[0][0]=ftol(DISTANCE(vertex[1],wing_x_edge.startPt));
				cut_angle[0][1]=ftol(DISTANCE(vertex[3],wing_x_edge.startPt));
				bCut|=VerifyAngleCuttingData(this,START_WING_X,minCutLen,maxIgnoreCutSpace);
			}
			else
			{
				cut_angle[2][0]=ftol(DISTANCE(vertex[1],wing_x_edge.endPt));
				cut_angle[2][1]=ftol(DISTANCE(vertex[3],wing_x_edge.endPt));
				double fMinWingWidth=cut_angle[2][0]<cut_angle[2][1]?cut_angle[2][0]:cut_angle[2][1];
				bCut|=VerifyAngleCuttingData(this,END_WING_X,minCutLen,maxIgnoreCutSpace);
			}
		}
		if(inters_valid[2]&&inters_valid[4])	//Y֫�н�
		{
			if(is_start_end)
			{
				cut_angle[1][0]=ftol(DISTANCE(vertex[2],wing_y_edge.startPt));
				cut_angle[1][1]=ftol(DISTANCE(vertex[4],wing_y_edge.startPt));
				double fMinWingWidth=cut_angle[1][1];
				if(cut_angle[1][0]<cut_angle[1][1])
					fMinWingWidth =cut_angle[1][0];
				bCut|=VerifyAngleCuttingData(this,START_WING_Y,minCutLen,maxIgnoreCutSpace);
			}
			else
			{
				cut_angle[3][0]=ftol(DISTANCE(vertex[2],wing_y_edge.endPt));
				cut_angle[3][1]=ftol(DISTANCE(vertex[4],wing_y_edge.endPt));
				bCut|=VerifyAngleCuttingData(this,END_WING_Y,minCutLen,maxIgnoreCutSpace);
			}
		}
	}
	for(int i=0;i<4;i++)
		cut_angle[i][1]=min(cut_angle[i][1],size.wide-size.thick);
	for(int j=0;j<2;j++)
		cut_wing_para[j][1]=min(cut_wing_para[j][1],size.wide-size.thick);
	return bCut;
}
//</DEVELOP_PROCESS_MARK>
long CLDSLineAngle::get_hStartDatumRod()
{
	if(desStartPos.datum_jg_h>0x20)
		return desStartPos.datum_jg_h;
	else if(pStart)
		return pStart->hFatherPart;
	else
		return 0;
}
long CLDSLineAngle::get_hEndDatumRod()
{
	if(desEndPos.datum_jg_h>0x20)
		return desEndPos.datum_jg_h;
	else if(pEnd)
		return pEnd->hFatherPart;
	else
		return 0;
}
CLDSLinePart* CLDSLineAngle::GetStartDatumPart()
{
	CLDSPart *pPart=NULL;
	if(desStartPos.datum_jg_h>0x20)
		pPart=BelongModel()->FromPartHandle(desStartPos.datum_jg_h);
	if(pPart&&pPart->IsLinePart())
		return (CLDSLinePart*)pPart;
	else if(pStart)
	{
		if(pStart->hFatherPart==handle)
			return this;
		else
		{
			pPart=BelongModel()->FromPartHandle(pStart->hFatherPart);
			if(pPart&&pPart->IsLinePart())
				return (CLDSLinePart*)pPart;
			else
				return NULL;
		}
	}
	else
		return NULL;
}
CLDSLinePart* CLDSLineAngle::GetEndDatumPart()
{
	CLDSPart *pPart=NULL;
	if(desEndPos.datum_jg_h>0x20)
		pPart=BelongModel()->FromPartHandle(desEndPos.datum_jg_h);
	if(pPart&&pPart->IsLinePart())
		return (CLDSLinePart*)pPart;
	else if(pEnd)
	{
		if(pEnd->hFatherPart==handle)
			return this;
		else
		{
			pPart=BelongModel()->FromPartHandle(pEnd->hFatherPart);
			if(pPart&&pPart->IsLinePart())
				return (CLDSLinePart*)pPart;
			else
				return NULL;
		}
	}
	else
		return NULL;
}

// �ж�ʼ(true)��(false)��λ����Ʋ�������Ƿ�Ϸ�
bool CLDSLineAngle::IsValidCombinationDesPara(bool bStartEnd)
{
	if(bStartEnd)	//�ж�ʼ��λ����Ʋ����Ƿ�Ϸ�
	{
		if(desStartPos.jgber_cal_style!=1)
		{
			if(desStartPos.spatialOperationStyle>0&&desStartPos.spatialOperationStyle<15&&
				(pStart->hFatherPart==handle||desStartPos.datum_jg_h==handle))
				return false;	//ʼ�˴��ڻ�׼λ��ʱ������ƫ�ƣ�����ӦΪ�ڵ�
		}
	}
	else			//�ж�ʼ��λ����Ʋ����Ƿ�Ϸ�
	{
		if(desEndPos.jgber_cal_style!=1)
		{
			if(desEndPos.spatialOperationStyle>0&&desEndPos.spatialOperationStyle<15&&
				(pEnd->hFatherPart==handle||desEndPos.datum_jg_h==handle))
				return false;	//ʼ�˴��ڻ�׼λ��ʱ������ƫ�ƣ�����ӦΪ�ڵ�
		}
	}
	return true;
}

void CLDSLineAngle::RemoveKaiHeJiao()
{
	cal_x_wing_norm();
	cal_y_wing_norm();
	f3dPoint pivot_norm=norm_x_wing+norm_y_wing;
	f3dPoint line_vec=End()-Start();
	normalize(pivot_norm);
	normalize(line_vec);
	if((pivot_norm^norm_x_wing)*line_vec<0)	//�Ǹֿ��Ǵ���180��
		pivot_norm*=-1.0;
	f3dPoint vice_vec=line_vec^pivot_norm;
	normalize(vice_vec);
	norm_x_wing=pivot_norm+vice_vec;
	norm_y_wing=pivot_norm-vice_vec;
	normalize(norm_x_wing);
	normalize(norm_y_wing);
	des_norm_x.bSpecific=des_norm_y.bSpecific=TRUE;
	des_norm_x.spec_norm.norm_style=des_norm_y.spec_norm.norm_style=0;
	des_norm_x.spec_norm.vector = norm_x_wing;
	des_norm_y.spec_norm.vector = norm_y_wing;
}

int CLDSLineAngle::ValidCheck()
{
	if(pStart==NULL||pEnd==NULL)
	{
		if(desStartPos.datumPoint.datum_pos_style==1||desStartPos.datumPoint.datum_pos_style==7)	//�Ǹ���������������
		{
			if(BelongModel()->FromPartHandle(desStartPos.datumPoint.des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -11;
		}
		else if(desStartPos.datumPoint.datum_pos_style==2)	//�Ǹֽڵ㶨λ
		{
			if(BelongModel()->FromPartHandle(desStartPos.datumPoint.des_para.RODNODE.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -11;
			if(BelongModel()->FromNodeHandle(desStartPos.datumPoint.des_para.RODNODE.hNode)==NULL)
				return -13;
		}
		else if(desStartPos.datumPoint.datum_pos_style==3)	//�Ǹ����߽���
		{
			if(BelongModel()->FromPartHandle(desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -11;
			if(BelongModel()->FromPartHandle(desStartPos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -11;
		}
		if(desEndPos.datumPoint.datum_pos_style==1||desEndPos.datumPoint.datum_pos_style==7)	//�Ǹ���������������
		{
			if(BelongModel()->FromPartHandle(desEndPos.datumPoint.des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -12;
		}
		else if(desEndPos.datumPoint.datum_pos_style==2)	//�Ǹֽڵ㶨λ
		{
			if(BelongModel()->FromPartHandle(desEndPos.datumPoint.des_para.RODNODE.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -12;
			if(BelongModel()->FromNodeHandle(desEndPos.datumPoint.des_para.RODNODE.hNode)==NULL)
				return -14;
		}
		else if(desEndPos.datumPoint.datum_pos_style==3)	//�Ǹ����߽���
		{
			if(BelongModel()->FromPartHandle(desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum1,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -12;
			if(BelongModel()->FromPartHandle(desEndPos.datumPoint.des_para.AXIS_INTERS.hDatum2,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -12;
		}
		return -1;	//������̽Ǹ�ȱ��ʼ�ն˽ڵ�
	}

	/*if(pStart==NULL||pEnd==NULL)
		return -2;	//�Ǹ�ȱ��ʼ�ն˽ڵ�
	else */if(pLsRefList==NULL)
		return -3;	//��˨����Ϊ��
	else if(desStartPos.jgber_cal_style!=1)	//������ƫ��
	{
		if(desStartPos.spatialOperationStyle==16||desStartPos.spatialOperationStyle==17)
		{
			logto.Log("error");
			//if(BelongModel()->FromPartHandle(des_start_pos.start_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL||
			//	BelongModel()->FromPartHandle(des_start_pos.end_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -5;	//ʼ��ȱ�ٹ���λ�û�׼�Ǹ�
		}
		else if(desStartPos.spatialOperationStyle>6&&desStartPos.spatialOperationStyle<15
			&&BelongModel()->FromPartHandle(desStartPos.huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
			return -4;	//ʼ��ȱ�ٻ�����׼�Ǹ�

	}
	else if(desEndPos.jgber_cal_style!=1)	//������ƫ��
	{
		if(desEndPos.spatialOperationStyle==16||desEndPos.spatialOperationStyle==17)
		{
			logto.Log("error");
			//if(BelongModel()->FromPartHandle(des_end_pos.start_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL||
			//	BelongModel()->FromPartHandle(des_end_pos.end_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
				return -7;	//�ն�ȱ�ٹ���λ�û�׼�Ǹ�
		}
		if(desEndPos.spatialOperationStyle>6&&desEndPos.spatialOperationStyle<15
			&&BelongModel()->FromPartHandle(desEndPos.huoqu_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
			return -6;	//�ն�ȱ�ٻ�����׼�Ǹ�
	}
	else if(!des_norm_x.bByOtherWing&&!des_norm_x.bSpecific&&BelongModel()->FromPartHandle(des_norm_x.hViceJg,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
		return -8;		//X֫���߼������ȱ�ٲ��սǸ�
	else if(!des_norm_y.bByOtherWing&&!des_norm_y.bSpecific&&BelongModel()->FromPartHandle(des_norm_y.hViceJg,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
		return -9;		//Y֫���߼������ȱ�ٲ��սǸ�
	return 0;
}

void CLDSLineAngle::RetrieveKaoMeAngle(CKaoMeAngle* pKM,long* hBoltArr,int nBolts,CLDSLineAngle *pCoupleAngle/*=NULL*/)
{
	GEPOINT lenStdVec=baseline.End()-baseline.Start();
	normalize(lenStdVec);
	pKM->listHoles.Empty();
	if(pCoupleAngle)
	{	//���������Ǹ���ȡ��ģ�Ǹ�ʱ��ģ���Ű��������Ǹּ��� wht 15-11-06
		GEPOINT verfiyVec=0.5*(baseline.Start()+baseline.End())-0.5*(pCoupleAngle->Start()+pCoupleAngle->End());
		normalize(verfiyVec);
		if(lenStdVec*verfiyVec>0)
			pKM->sLabel.Printf("%s/%s",(char*)pCoupleAngle->GetPartNo(),(char*)GetPartNo());
		else
			pKM->sLabel.Printf("%s/%s",(char*)GetPartNo(),(char*)pCoupleAngle->GetPartNo());
	}
	else
		pKM->sLabel.Copy(GetPartNo());
	CMinDouble startx;
	CMaxDouble endx,max_g;
	for(int i=0;i<nBolts;i++)
	{
		CLsRef* pLsRef=pLsRefList->FromHandle(hBoltArr[i]);
		if(pCoupleAngle&&pLsRef==NULL)
			pLsRef=pCoupleAngle->FindLsByHandle(hBoltArr[i]);
		if(pLsRef==NULL)
			continue;
		CLDSBolt* pBolt=pLsRef->GetLsPtr();
		GEPOINT inters;
		int iYPosFlag=1;//��ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫
		if(fabs(norm_x_wing*pBolt->ucs.axis_z)>fabs(norm_y_wing*pBolt->ucs.axis_z ))
		{
			Int3dlf(inters,pBolt->ucs.origin,pBolt->ucs.axis_z,ucs.origin,norm_x_wing);
			iYPosFlag=-1;
		}
		else
			Int3dlf(inters,pBolt->ucs.origin,pBolt->ucs.axis_z,ucs.origin,norm_y_wing);
		GEPOINT length_vec=inters-baseline.Start();
		CKaoMeAngle::BOLTHOLE hole;
		hole.pos.x=ftoi(length_vec*lenStdVec);
		hole.pos.y=ftoi(GetLsG(pBolt))*iYPosFlag;
		hole.d = (short)(*pLsRef)->get_d();
		CKaoMeAngle::BOLTHOLE* pHole=pKM->listHoles.SetValue(hole.pos.key,hole);
		startx.Update(hole.pos.x,pHole);
		endx.Update(hole.pos.x,pHole);
		max_g.Update(abs(hole.pos.y),pHole);
	}
	pKM->length=ftoi(endx.number-startx.number);
	short startSpace=30,endSpace=30;
	if(startx.IsInited())
	{
		if(((CKaoMeAngle::BOLTHOLE*)startx.m_pRelaObj)->d==16)
			startSpace=25;
		else if(((CKaoMeAngle::BOLTHOLE*)startx.m_pRelaObj)->d==20)
			startSpace=30;
		else if(((CKaoMeAngle::BOLTHOLE*)startx.m_pRelaObj)->d==24)
			startSpace=40;
	}
	pKM->length+=startSpace;
	if(endx.IsInited())
	{
		if(((CKaoMeAngle::BOLTHOLE*)endx.m_pRelaObj)->d==16)
			endSpace=25;
		else if(((CKaoMeAngle::BOLTHOLE*)endx.m_pRelaObj)->d==20)
			endSpace=30;
		else if(((CKaoMeAngle::BOLTHOLE*)endx.m_pRelaObj)->d==24)
			endSpace=40;
	}
	pKM->length+=endSpace;
	for(CKaoMeAngle::BOLTHOLE* pHole=pKM->listHoles.GetFirst();pHole;pHole=pKM->listHoles.GetNext())
		pHole->pos.x-=(short)ftoi(startx.number-startSpace);
	if(max_g.IsInited())
		pKM->wide=ftoi(max_g+startSpace);
	//��ʼ����ģ�ǸֵĿ��Ͻ�
	double wing_angle = GetRadWingAngle();
	double fThreshold = library != NULL ? library->GetJgKaiHeJiaoThreshold() : 2;
	if(fabs(GetDecWingAngle()-90)> fThreshold)	//�н�ƫ�����2��Ҫ���Ͻ�
	{
		if(wing_angle>Pi/2)
			pKM->sKaiHeAngle.Printf("+%.1f��",DEGTORAD_COEF*(wing_angle-Pi/2));
		else
			pKM->sKaiHeAngle.Printf("-%.1f��",DEGTORAD_COEF*(Pi/2-wing_angle));
	}
}
void CLDSLineAngle::GetProcessStr(char *notes, size_t max_len,bool bByEnglish/*=false*/,bool bNumericKaiHeInfo/*=false*/)
{
	if(notes==NULL)
		return;
#ifdef AFX_TARG_ENU_ENGLISH
	bByEnglish=TRUE;
#endif
	for(int i=0;i<4;i++)
	{
		if(cut_angle[i][0]>EPS)
		{
			if(4<max_len)
			{
				if(bByEnglish)
					strcpy(notes,"cut leg");
				else
					strcpy(notes,"�н�");
			}
			break;
		}
	}
	if(cut_wing[0]>0||cut_wing[1]>0)
	{
		if(strlen(notes)+4<max_len)
		{
			if(bByEnglish)
				strcat(notes,"Cut leg");
			else
				strcat(notes,"��֫");
		}
	}
	double wing_angle = GetDecWingAngle();
	int kaihe_base_wing=GetKaiHeWingX0Y1();
	//����97��ͼ�涨���е��޸� WJH-2004/09/28
	if(bByEnglish)
	{
		if(kaihe_base_wing==0||kaihe_base_wing==1)
		{
			if(wing_angle>92&&(strlen(notes)+8<max_len))
				strcat(notes,"Open leg");
			else if(wing_angle<88&&(strlen(notes)+8<max_len))
				strcat(notes,"Close leg");
		}
		if(m_bCutRoot&&(strlen(notes)+4<max_len))	//�ٸ�
			strcat(notes,"Cut root");
		else if(m_bCutBer&&(strlen(notes)+4<max_len))	//��������
			strcat(notes,"Cut back");
		if(start_push_flat_x1_y2!=0||end_push_flat_x1_y2!=0||mid_push_flat_x1_y2!=0)
			strcat(notes,"Push flat");
		if(m_bWeldPart)
			strcat(notes,"Welding");
		if(IsHasFootNail())
			strcat(notes,"With climbing bolts");
	}
	else
	{
		CXhString sNotes(notes,max_len);
		if(kaihe_base_wing==0||kaihe_base_wing==1)
		{
			bool needKaiHe=true;
			if (CLDSPart::CustomerSerial == 6 || CLDSPart::CustomerSerial == 1)
			{	//�������������ڲ�������ʱ�ã�
				double MAX_KAIHE_CLEARANCE=library!=NULL?library->GetMaxKaiHeJiaoClearance():3;
				if(ConnectClearance()<MAX_KAIHE_CLEARANCE)
					needKaiHe=false;	//���Ͻ�һ�㲻ͬʱ������֫������Ӧ����ȫ���ǶȲwjh-2017.12.14
			}
			if(wing_angle>90+KAIHEJIAO_ANGLE_THRESHOLD&&needKaiHe)
				sNotes.Append("����",0);
			else if(wing_angle<90-KAIHEJIAO_ANGLE_THRESHOLD&&needKaiHe)
				sNotes.Append("�Ͻ�",0);
			else
				needKaiHe=false;
			if(needKaiHe&&bNumericKaiHeInfo)
				sNotes.AppendFormat("%g��",ftoi(wing_angle*10)*0.1);
			else if(needKaiHe)
				sNotes.Append("����",0);
		}
		if(m_bCutRoot&&(strlen(notes)+4<max_len))	//�ٸ�
			strcat(notes,"�ٸ�");
		else if(m_bCutBer&&(strlen(notes)+4<max_len))	//��������
			strcat(notes,"����");
		if(start_push_flat_x1_y2!=0||end_push_flat_x1_y2!=0||mid_push_flat_x1_y2!=0)
			strcat(notes,"ѹ��");
		if(m_bWeldPart)
			strcat(notes,"����");
		if(IsHasFootNail())
			strcat(notes,"���Ŷ�");
	}
	return;
}

double CLDSLineAngle::GetStartWingNormOffset(int x_wing0_y_wing1)
{
	JGZJ jgzj;
	int offset_dist_style;
	getjgzj(jgzj,size_wide);
	if(x_wing0_y_wing1==0)
	{
		if(m_bEnableTeG)
			jgzj=xWingYZhunJu;
		offset_dist_style=desStartPos.wing_x_offset.gStyle;//.offset_X_dist_style;
		//if(pStart&&pStart->hFatherPart==handle)
		//	offset_dist_style=pStart->xFatherAngleZhunJu.offset_X_dist_style;
	}
	else
	{
		if(m_bEnableTeG)
			jgzj=xWingXZhunJu;
		offset_dist_style=desStartPos.wing_y_offset.gStyle;//.offset_Y_dist_style;
		//if(pStart&&pStart->hFatherPart==handle)
		//	offset_dist_style=pStart->xFatherAngleZhunJu.offset_Y_dist_style;
	}
	if(offset_dist_style==0)
		return jgzj.g;
	else if(offset_dist_style==1)
		return jgzj.g1;
	else if(offset_dist_style==2)
		return jgzj.g2;
	else if(offset_dist_style==3)
		return jgzj.g3;
	else
	{
		if(x_wing0_y_wing1==0)
			return desStartPos.wing_x_offset.offsetDist;//.offset_X_dist;
		else //if(x_wing0_y_wing1==1)
			return desStartPos.wing_y_offset.offsetDist;//.offset_Y_dist;
	}
}

double CLDSLineAngle::GetArea()
{
	if(library)
	{
		if(size_idClassType!=0)
			return library->GetPartArea(size_wide,size_thick,0,size_idClassType,size_cSubClassType);
		else
			return library->GetPartArea(size_wide,size_thick,0,GetClassTypeId(),size_cSubClassType);
	}
	else	//�����趨��ֻ�ܼ������ֵ
		return size_wide*size_thick*2-size_thick*size_thick;
}

//�õ��򻯹���ַ����������������͡����ʵ���Ϣ������֫��֫�� wht12-04-27
void CLDSLineAngle::GetAngleSimpleSpec(char *simple_spec)
{
	CLDSLineAngle::GetAngleSimpleSpec(simple_spec,size_wide,size_height,size_thick);
}
void CLDSLineAngle::GetAngleSimpleSpec(char *simple_spec,double x_wing_wide,double y_wing_wide,double wide_thick)
{
	char sSpec[200]="";
	char sWide[100]="",sThick[100]="";
	_snprintf(sWide,100,"%f",x_wing_wide);
	SimplifiedNumString(sWide);
	_snprintf(sThick,100,"%f",wide_thick);
	SimplifiedNumString(sThick);
	if(IsUnequalAngle(x_wing_wide,y_wing_wide,wide_thick))
	{
		char sWideY[100]="";
		_snprintf(sWideY,100,"%f",y_wing_wide);
		SimplifiedNumString(sWideY);
		_snprintf(sSpec,200,"%sx%sx%s",sWide,sWideY,sThick);
	}
	else 
		_snprintf(sSpec,200,"%sx%s",sWide,sThick);
	if(simple_spec)
		StrCopy(simple_spec,sSpec,51);	//�Ǹֹ��һ�㲻�ᳬ��50���ַ����Ӵ������Է������ݻ��ҵ��½�һ���ڴ���� wjh-2019.9.29
}

int CLDSLineAngle::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	char steelmark[20]="";
	if(bIncMat&&cMaterial!='S')
		QuerySteelMatMark(cMaterial,steelmark);
	char simple_spec[200]="";
	GetAngleSimpleSpec(simple_spec,size_wide,size_height,size_thick);
	char tmp_spec[200]="";
	if(size.cSubClassType!='L'&&size.cSubClassType!=0)
		sprintf(tmp_spec,"%s%CL%s",steelmark,size_cSubClassType,simple_spec);
	else if(strlen(steelmark)>0)
		sprintf(tmp_spec,"%s L%s",steelmark,simple_spec);
	else
		sprintf(tmp_spec,"L%s",simple_spec);
	if(spec)
		strcpy(spec,tmp_spec);
	return strlen(tmp_spec);
}

double CLDSLineAngle::GetWeight(BOOL bSolidLength/*=TRUE*/)
{
	double unit_weight=0;
	if(library)
		unit_weight=library->GetWeightOf1Meter(size_wide,size_thick,size_height,size_idClassType,size_cSubClassType);
	else
		unit_weight=GetArea()*7.85e-3;
	double length;
	if(bSolidLength||pStart==NULL||pEnd==NULL)
		length=GetLength();
	else
		length=DISTANCE(pStart->Position(false),pEnd->Position(false));
	return length*0.001*unit_weight;
}

double CLDSLineAngle::GetEndWingNormOffset(int x_wing0_y_wing1)
{
	JGZJ jgzj;
	int offset_dist_style;
	getjgzj(jgzj,size_wide);
	if(x_wing0_y_wing1==0)
	{
		if(m_bEnableTeG)
			jgzj=xWingYZhunJu;
		offset_dist_style=desEndPos.wing_x_offset.gStyle;//.offset_X_dist_style;
		//if(pEnd&&pEnd->hFatherPart==handle)
		//	offset_dist_style=pEnd->xFatherAngleZhunJu.offset_X_dist_style;
	}
	else
	{
		if(m_bEnableTeG)
			jgzj=xWingXZhunJu;
		offset_dist_style=desEndPos.wing_y_offset.gStyle;//.offset_Y_dist_style;
		//if(pEnd&&pEnd->hFatherPart==handle)
		//	offset_dist_style=pEnd->xFatherAngleZhunJu.offset_Y_dist_style;
	}
	if(offset_dist_style==0)
		return jgzj.g;
	else if(offset_dist_style==1)
		return jgzj.g1;
	else if(offset_dist_style==2)
		return jgzj.g2;
	else if(offset_dist_style==3)
		return jgzj.g3;
	else
	{
		if(x_wing0_y_wing1==0)
			return desEndPos.wing_x_offset.offsetDist;
		else //if(x_wing0_y_wing1==1)
			return desEndPos.wing_y_offset.offsetDist;
	}
}
CLDSLineAngle::ANGLEPOINT::ANGLEPOINT()
{
	memset(this,0,sizeof(ANGLEPOINT));
}
bool CLDSLineAngle::GetMirMatchInfo(CLDSLineAngle* pToMirAngle,MIRMSG mirmsg,bool* pbReverseStartEnd/*=NULL*/,bool* pvReverseWingXnY/*=NULL*/)
{	//ԭ����Ϊ�ú����㷨�ȽϷ��������ڿ����ú���Ҳ�ɸ������ֱ���ڵ��õ�ֱ�������ж� wjh-2017.11.13
	if(pbReverseStartEnd)
	{
		GEPOINT lenVec=End()-Start();
		GEPOINT vDestMirLenVec=mirmsg.MirVector(lenVec);
		GEPOINT vMirLenVec=pToMirAngle->End()-pToMirAngle->Start();
		*pbReverseStartEnd=(vDestMirLenVec*vMirLenVec)<0;
	}
	if(pvReverseWingXnY)
	{
		GEPOINT dest_norm_x_wing=mirmsg.MirVector(norm_x_wing);
		//GEPOINT dest_norm_y_wing=mirmsg.MirVector(norm_y_wing);
		GEPOINT mir_norm_x_wing=pToMirAngle->vxWingNorm;
		GEPOINT mir_norm_y_wing=pToMirAngle->vyWingNorm;
		if(fabs(dest_norm_x_wing*mir_norm_x_wing)<fabs(dest_norm_x_wing*mir_norm_y_wing))
			*pvReverseWingXnY=true;	//��Ҫ�ߵ�X��Y֫�ķ��߲���
		else
			*pvReverseWingXnY=false;
	}
	return true;
}
bool CLDSLineAngle::GetMirAnglePoint(ANGLEPOINT from,ANGLEPOINT &to, CLDSLineAngle* pToMirAngle,MIRMSG mirmsg)
{
	CLDSLineAngle* pAngle=pToMirAngle;
	bool reverseStartEnd=false;
	bool reverseWingXnY=false;
	//TODO:�ų������Ǹֲ��ԳƵ����
	if(pStart && pAngle->pEnd)
	{
		f3dPoint pos=pStart->Position(false);
		f3dPoint mirpos=pAngle->pEnd->Position(false);
		f3dPoint destpos=mirmsg.MirPoint(pos);
		if(mirpos.IsEqual(destpos))
		{
			reverseStartEnd=true;	//�ԳƽǸ���Ҫ�ߵ�ʼĩ��
			f3dPoint berpos=Start();
			f3dPoint destberpos=pAngle->End();
			berpos=mirmsg.MirPoint(berpos);
			if(!berpos.IsEqual(destberpos))
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�Ǹ���0x%X�Ǹ�ʵ��λ�ò��Գ�!",handle,pAngle->handle);
		}
	}
	else if(pEnd && pAngle->pStart)
	{
		f3dPoint pos=pEnd->Position(false);
		f3dPoint mirpos=pAngle->pStart->Position(false);
		f3dPoint destpos=mirmsg.MirPoint(pos);
		if(mirpos.IsEqual(destpos))
		{
			reverseStartEnd=true;	//�ԳƽǸ���Ҫ�ߵ�ʼĩ��
			f3dPoint berpos=End();
			f3dPoint destberpos=pAngle->Start();
			berpos=mirmsg.MirPoint(berpos);
			if(!berpos.IsEqual(destberpos))
				Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�Ǹ���0x%X�Ǹ�ʵ��λ�ò��Գ�!",handle,pAngle->handle);
		}
	}
	if(!reverseStartEnd)
	{
		f3dPoint berpos=Start();
		f3dPoint destberpos=pAngle->Start();
		berpos=mirmsg.MirPoint(berpos);
		if(!berpos.IsEqual(destberpos))
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�Ǹ���0x%X�Ǹ�ʵ��λ�ò��Գ�!",handle,pAngle->handle);
		berpos=End();
		destberpos=pAngle->End();
		berpos=mirmsg.MirPoint(berpos);
		if(!berpos.IsEqual(destberpos))
			Log2File()->LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT,"0x%X�Ǹ���0x%X�Ǹ�ʵ��λ�ò��Գ�!",handle,pAngle->handle);
		to.direction=from.direction;
	}
	else
		to.direction=1-from.direction;
	to.len_offset_dist=from.len_offset_dist;

	f3dPoint dest_norm_x_wing=mirmsg.MirVector(norm_x_wing);
	f3dPoint dest_norm_y_wing=mirmsg.MirVector(norm_y_wing);
	f3dPoint mir_norm_x_wing=pAngle->vxWingNorm;
	f3dPoint mir_norm_y_wing=pAngle->vyWingNorm;
	GEPOINT vxWingVec=GetWingVecX();
	GEPOINT vyWingVec=GetWingVecY();
	GEPOINT vxDestWingVec=mirmsg.MirVector(vxWingVec);
	GEPOINT vyDestWingVec=mirmsg.MirVector(vyWingVec);
	GEPOINT vxMirWingVec=pAngle->GetWingVecX();
	GEPOINT vyMirWingVec=pAngle->GetWingVecY();

	CLDSNode* pNode=NULL,*pMirNode=NULL;
	if(from.hResideNode<=0x20)
		to.hResideNode=0;
	else
		pNode=BelongModel()->FromNodeHandle(from.hResideNode);
	if(from.wing_offset_style<4)
	{
		JGZJ jgzj=GetZhunJu(from.offset_wing==0?'X':'Y');
		double g=jgzj.g;
		if(pNode!=NULL)
		{	
			if((g=from.offset_wing==0?GetNodeWingXG(pNode):GetNodeWingYG(pNode))!=jgzj.g)
			{
				pMirNode=pNode->GetMirNode(mirmsg);
				to.hResideNode=pMirNode!=NULL?pMirNode->handle:0;
			}
		}
		if(from.wing_offset_style==0)
			from.wing_offset_dist=g;
		else if(from.wing_offset_style==1)
			from.wing_offset_dist=jgzj.g1;
		else if(from.wing_offset_style==2)
			from.wing_offset_dist=jgzj.g2;
		else if(from.wing_offset_style==3)
			from.wing_offset_dist=jgzj.g3;
	}
	//������ȷ������֫ʱ,ֻ��֫���߽���΢��,������֫����X/Y֫�����ȴ�ı䶯
	if(fabs(dest_norm_x_wing*mir_norm_x_wing)<fabs(dest_norm_x_wing*mir_norm_y_wing))
	{
		to.offset_wing=1-from.offset_wing;
		if(from.offset_wing==0)
			to.wing_offset_dist=(vxDestWingVec*vyMirWingVec>0)?from.wing_offset_dist:-from.wing_offset_dist;	//ԭʼ��X֫ƫ��
		else if(from.offset_wing==1)
			to.wing_offset_dist=(vyDestWingVec*vxMirWingVec>0)?from.wing_offset_dist:-from.wing_offset_dist;	//ԭʼ��X֫ƫ��
		reverseWingXnY=true;	//��Ҫ�ߵ�X��Y֫�ķ��߲���
	}
	else
	{
		to.offset_wing=from.offset_wing;
		if(from.offset_wing==0)
			to.wing_offset_dist=(vxDestWingVec*vxMirWingVec>0)?from.wing_offset_dist:-from.wing_offset_dist;	//ԭʼ��X֫ƫ��
		else if(from.offset_wing==1)
			to.wing_offset_dist=(vyDestWingVec*vyMirWingVec>0)?from.wing_offset_dist:-from.wing_offset_dist;	//ԭʼ��X֫ƫ��
		reverseWingXnY=false;
	}
	to.wing_offset_style=from.wing_offset_style;
	if(from.wing_offset_style<4)
	{
		JGZJ jgzj=pAngle->GetZhunJu(to.offset_wing==0?'X':'Y');
		double g=jgzj.g;
		if(pMirNode!=NULL)
			g=to.offset_wing==0?pAngle->GetNodeWingXG(pMirNode):pAngle->GetNodeWingYG(pMirNode);
		if(fabs(to.wing_offset_dist-g)<EPS)
			to.wing_offset_style=0;
		else if(fabs(to.wing_offset_dist-jgzj.g1)<EPS)
			to.wing_offset_style=1;
		else if(fabs(to.wing_offset_dist-jgzj.g2)<EPS)
			to.wing_offset_style=2;
		else if(fabs(to.wing_offset_dist-jgzj.g3)<EPS)
			to.wing_offset_style=3;
		else
			to.wing_offset_style=4;
	}
	//����ƫ����
	bool bReverseNormOffsetSign=false;
	if(reverseWingXnY)
	{
		if( (from.offset_wing==0&&dest_norm_x_wing*mir_norm_y_wing<0)||
			(from.offset_wing==1&&dest_norm_y_wing*mir_norm_x_wing<0))
			bReverseNormOffsetSign=true;
	}
	else
	{
		if( (from.offset_wing==0&&dest_norm_x_wing*mir_norm_x_wing<0)||
			(from.offset_wing==1&&dest_norm_y_wing*mir_norm_y_wing<0))
			bReverseNormOffsetSign=true;
	}
	if(bReverseNormOffsetSign)
	{	//����ƫ��������
		CParaThick paraThick(from.norm_offset);
		int index=0;
		for(THICK_ITEM item=paraThick.GetFirstItem();item.val!=0;item=paraThick.GetNextItem(),index++)
		{
			item.val*=-1;	//����
			paraThick.SetItem(index,item);
		}
		StrCopy(to.norm_offset,(char*)paraThick.ToThickStr(),51);
	}
	else
		StrCopy(to.norm_offset,from.norm_offset,51);
	return true;
}

MEMORY_BLOCK CLDSLineAngle::MPCL;	//Mirror Property Control List
PROPLIST_TYPE CLDSLineAngle::propMirSync;
bool CLDSLineAngle::IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr)
{
	if(pObj==NULL||propKeyStr==NULL||(pObj->GetClassTypeId()!=CLS_LINEANGLE&&pObj->GetClassTypeId()!=CLS_GROUPLINEANGLE))
		return false;
	if(group_father_jg_h>0x20)
	{	//��ϽǸֵ��ӽǸ�����ϽǸֿ���ʱ��������ͬ�����¹����ӽǸ�
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)BelongModel()->FromPartHandle(CLS_GROUPLINEANGLE);
		if(pGroupAngle&&(stricmp(propKeyStr,"Position")==0||stricmp(propKeyStr,"wingNormInfo")==0))
			return false;
	}
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pObj;
	MEMORY_BLOCK cl=mpcl;
	if(cl.IsZero())
		cl=MPCL;
	MEMORY_BLOCK mir_cl=pAngle->mpcl;
	if(mir_cl.IsZero())
		mir_cl=MPCL;
	PROPLIST_ITEM* pItem=propMirSync.GetPropItem(propKeyStr);
	if(pItem!=NULL)
		return cl.GetBitState((WORD)pItem->id-1)&&mir_cl.GetBitState((WORD)pItem->id-1);
	else
	{
		Log2File()->Log("%sͬ���Գ�������ȱʧ",propKeyStr);
		return false;
	}
}
bool CLDSLineAngle::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL)
		return false;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pDestObj;
	if(propKeyStr==NULL||strcmp(propKeyStr,"synchronize_scope")==0)
	{
		MEMORY_BLOCK cl=mpcl;
		if(cl.IsZero())
			cl=MPCL;
		PROPLIST_ITEM* pItem=propMirSync.GetPropItem("synchronize_scope");
		if(pItem!=NULL && cl.GetBitState((WORD)pItem->id-1))
			pDestObj->mpcl=mpcl;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//����
		pAngle->cMaterial=cMaterial;
		pAngle->cbVirginQualityLevel=cbVirginQualityLevel;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//���
		pAngle->size_wide=size_wide;
		pAngle->size_thick=size_thick;
		pAngle->size_idClassType=size_idClassType;
		pAngle->size_cSubClassType=size_cSubClassType;
		if(m_nClassTypeId==CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)pAngle)->jg_space=((CLDSGroupLineAngle*)this)->jg_space;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//�ֶκ�
		pAngle->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//��ĺ�
		pAngle->cfgword=cfgword;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"m_bDisplayHole")==0)&&IsSyncMirPropWith(pDestObj,"m_bDisplayHole"))
	{	//��ʾ��
		pAngle->m_bDisplayHole=m_bDisplayHole;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"crMaterial")==0)&&IsSyncMirPropWith(pDestObj,"crMaterial"))
	{	//������ɫ
		pAngle->crMaterial=crMaterial;
	}
#ifdef __PART_DESIGN_INC_
	if((propKeyStr==NULL||strcmp(propKeyStr,"DesignInfo")==0))//&&IsSyncMirPropWith(pDestObj,"DesignInfo"))
	{
		pAngle->CalLenCoef.iTypeNo=CalLenCoef.iTypeNo;
		pAngle->CalLenCoef.minR.coef=CalLenCoef.minR.coef;
		pAngle->CalLenCoef.paraR.coef=CalLenCoef.paraR.coef;
		pAngle->m_bAutoCalHoleOutCoef=m_bAutoCalHoleOutCoef;
		pAngle->hole_out=hole_out;
		pAngle->sel_mat_method=sel_mat_method;
		pAngle->start_force_type=start_force_type;
		pAngle->start_joint_type=start_joint_type;
		pAngle->end_force_type=end_force_type;
		pAngle->end_joint_type=end_joint_type;
		pAngle->m_iElemType=m_iElemType;
		pAngle->m_bNeedSelSize=m_bNeedSelSize;
		pAngle->m_fWeightRaiseCoef=m_fWeightRaiseCoef;
	}
#endif
	bool reverseStartEnd=false;
	bool reverseWingXnY=false;
	if( (propKeyStr==NULL||strcmp(propKeyStr,"oddStart")==0)&&(IsSyncMirPropWith(pDestObj,"oddStart")||
		(sPartNo.EqualNoCase(pAngle->GetPartNo())&&IsSyncMirPropWith(pDestObj,"oddStartOnSameLabel"))))
	{	//ʼ������ͷ
		pAngle->m_blOddByBoltCanAcrossMiddleSect=m_blOddByBoltCanAcrossMiddleSect;
		DESIGNODDMENT* pDesOdd=&pAngle->desStartOdd;
		bool modified=false;
		if(!reverseStartEnd)
		{
			modified=fabs(pAngle->startOdd()-startOdd())>0.5;
			pAngle->SetStartOdd(startOdd());
		}
		else
		{
			modified=fabs(pAngle->endOdd()-startOdd())>0.5;
			pAngle->SetEndOdd(startOdd());
			pDesOdd=&pAngle->desEndOdd;
		}
		if(modified)
		{
			AppendModifiedAngles(pAngle);
			pAngle->SetModified(modified);
		}
		pDesOdd->m_fCollideDist=desStartOdd.m_fCollideDist;
		pDesOdd->m_iOddCalStyle=desStartOdd.m_iOddCalStyle;
		f3dPoint pick=Start();
		//if(desStartOdd.m_hRefPart1>0x20)
		//	MirPartHandle(BelongModel(),&pDesOdd->m_hRefPart1,desStartOdd.m_hRefPart1,mirmsg,&pick);
		//if(desStartOdd.m_hRefPart2>0x20)
		//	MirPartHandle(BelongModel(),&pDesOdd->m_hRefPart2,desStartOdd.m_hRefPart2,mirmsg,&pick);
	}
	if( (propKeyStr==NULL||strcmp(propKeyStr,"oddEnd")==0)&&(IsSyncMirPropWith(pDestObj,"oddEnd")||
		(sPartNo.EqualNoCase(pAngle->GetPartNo())&&IsSyncMirPropWith(pDestObj,"oddEndOnSameLabel"))))
	{	//�ն�����ͷ
		pAngle->m_blOddByBoltCanAcrossMiddleSect=m_blOddByBoltCanAcrossMiddleSect;
		DESIGNODDMENT* pDesOdd=&pAngle->desEndOdd;
		bool modified=false;
		if(!reverseStartEnd)
		{
			modified=fabs(pAngle->endOdd()-endOdd())>0.5;
			pAngle->SetEndOdd(endOdd());
		}
		else
		{
			modified=fabs(pAngle->startOdd()-endOdd())>0.5;
			pAngle->SetStartOdd(endOdd());
			pDesOdd=&pAngle->desStartOdd;
		}
		if(modified)
		{
			AppendModifiedAngles(pAngle);
			pAngle->SetModified(modified);
		}
		pDesOdd->m_fCollideDist=desEndOdd.m_fCollideDist;
		pDesOdd->m_iOddCalStyle=desEndOdd.m_iOddCalStyle;
		f3dPoint pick=End();
		//if(desStartOdd.m_hRefPart1>0x20)
		//	MirPartHandle(BelongModel(),&pDesOdd->m_hRefPart1,desEndOdd.m_hRefPart1,mirmsg,&pick);
		//if(desStartOdd.m_hRefPart2>0x20)
		//	MirPartHandle(BelongModel(),&pDesOdd->m_hRefPart2,desEndOdd.m_hRefPart2,mirmsg,&pick);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"linkInfo")==0)&&IsSyncMirPropWith(pDestObj,"linkInfo"))
	{	//������˨��Ϣ
		if(!reverseStartEnd)
		{
			pAngle->connectStart=connectStart;
			pAngle->connectEnd=connectEnd;
		}
		else
		{
			pAngle->connectStart=connectEnd;
			pAngle->connectEnd=connectStart;
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"kaihe_base_wing_x0_y1")==0)&&IsSyncMirPropWith(pDestObj,"kaihe_base_wing_x0_y1"))
	{	//����֫
		pAngle->kaihe_base_wing_x0_y1=kaihe_base_wing_x0_y1;
		if(reverseWingXnY && kaihe_base_wing_x0_y1<=1)	//X��Y֫���Ͻ�
			pAngle->kaihe_base_wing_x0_y1=1-kaihe_base_wing_x0_y1;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"m_bEnableTeG")==0)&&IsSyncMirPropWith(pDestObj,"m_bEnableTeG"))
	{	//����׼��
		pAngle->m_bEnableTeG=m_bEnableTeG;
		pAngle->xWingXZhunJu=xWingXZhunJu;
		pAngle->xWingYZhunJu=xWingYZhunJu;
		if(m_bEnableTeG && reverseWingXnY)
		{
			pAngle->xWingXZhunJu=xWingYZhunJu;
			pAngle->xWingYZhunJu=xWingXZhunJu;
		}
	}
	if( propKeyStr!=NULL&&
		strcmp(propKeyStr,"oddStartOnSameLabel")==0&&IsSyncMirPropWith(pDestObj,"oddStartOnSameLabel")&&
		sPartNo.EqualNoCase(pAngle->Label)&&(LABEL_PROP::ANGLE_CUT_START&dwPropFilterFlag)>0)
	{
		bool modified=false;
		if(reverseStartEnd)
		{	//ʼĩ�˵ߵ�
			modified|=fabs(pAngle->cut_angle[2][0]-cut_angle[0][0])>0.5;
			modified|=fabs(pAngle->cut_angle[2][1]-cut_angle[0][1])>0.5;
			modified|=fabs(pAngle->cut_angle[3][0]-cut_angle[1][0])>0.5;
			modified|=fabs(pAngle->cut_angle[3][1]-cut_angle[1][1])>0.5;
			modified|=(pAngle->cut_wing[1]!=cut_wing[0]);
			modified|=fabs(pAngle->cut_wing_para[1][0]-cut_wing_para[0][0])>0.5;
			modified|=fabs(pAngle->cut_wing_para[1][1]-cut_wing_para[0][1])>0.5;
			modified|=fabs(pAngle->cut_wing_para[1][2]-cut_wing_para[0][2])>0.5;

			pAngle->m_bUserSpecEndCutAngle=m_bUserSpecStartCutAngle;
			memcpy(&pAngle->cut_angle[2][0],&cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],&cut_angle[1][0],sizeof(double)*2);
			pAngle->cut_wing[1]=cut_wing[0];
			memcpy(&pAngle->cut_wing_para[1][0],&cut_wing_para[0][0],sizeof(double)*3);
		}
		else
		{
			modified|=fabs(pAngle->cut_angle[0][0]-cut_angle[0][0])>0.5;
			modified|=fabs(pAngle->cut_angle[0][1]-cut_angle[0][1])>0.5;
			modified|=fabs(pAngle->cut_angle[1][0]-cut_angle[1][0])>0.5;
			modified|=fabs(pAngle->cut_angle[1][1]-cut_angle[1][1])>0.5;
			modified|=(pAngle->cut_wing[0]!=cut_wing[0]);
			modified|=fabs(pAngle->cut_wing_para[0][0]-cut_wing_para[0][0])>0.5;
			modified|=fabs(pAngle->cut_wing_para[0][1]-cut_wing_para[0][1])>0.5;
			modified|=fabs(pAngle->cut_wing_para[0][2]-cut_wing_para[0][2])>0.5;

			pAngle->m_bUserSpecStartCutAngle=m_bUserSpecStartCutAngle;
			memcpy(&pAngle->cut_angle[0][0],&cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],&cut_angle[1][0],sizeof(double)*2);
			pAngle->cut_wing[0]=cut_wing[0];
			memcpy(&pAngle->cut_wing_para[0][0],&cut_wing_para[0][0],sizeof(double)*3);
		}
		if(reverseWingXnY)
		{	//X��Y֫�ߵ�
			double tmparr[3];
			memcpy(tmparr,&pAngle->cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[0][0],&pAngle->cut_angle[1][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],tmparr,sizeof(double)*2);

			if(pAngle->cut_wing[0]==1)
				pAngle->cut_wing[0]=2;
			else if(pAngle->cut_wing[0]==2)
				pAngle->cut_wing[0]=1;
		}
		if(modified)
		{
			pAngle->SetModified(modified);
			AppendModifiedAngles(pAngle);
		}
	}
	if( propKeyStr!=NULL&&
		strcmp(propKeyStr,"oddEndOnSameLabel")==0&&IsSyncMirPropWith(pDestObj,"oddEndOnSameLabel")&&
		sPartNo.EqualNoCase(pAngle->Label)&&(LABEL_PROP::ANGLE_CUT_END&dwPropFilterFlag)>0)
	{
		bool modified=false;
		if(reverseStartEnd)
		{	//ʼĩ�˵ߵ�
			modified|=fabs(pAngle->cut_angle[0][0]-cut_angle[2][0])>0.5;
			modified|=fabs(pAngle->cut_angle[0][1]-cut_angle[2][1])>0.5;
			modified|=fabs(pAngle->cut_angle[1][0]-cut_angle[3][0])>0.5;
			modified|=fabs(pAngle->cut_angle[1][1]-cut_angle[3][1])>0.5;
			modified|=(pAngle->cut_wing[0]!=cut_wing[1]);
			modified|=fabs(pAngle->cut_wing_para[0][0]-cut_wing_para[1][0])>0.5;
			modified|=fabs(pAngle->cut_wing_para[0][1]-cut_wing_para[1][1])>0.5;
			modified|=fabs(pAngle->cut_wing_para[0][2]-cut_wing_para[1][2])>0.5;

			pAngle->m_bUserSpecStartCutAngle=m_bUserSpecEndCutAngle;
			memcpy(&pAngle->cut_angle[0][0],&cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],&cut_angle[3][0],sizeof(double)*2);
			pAngle->cut_wing[0]=cut_wing[1];
			memcpy(&pAngle->cut_wing_para[0][0],&cut_wing_para[1][0],sizeof(double)*3);
		}
		else
		{
			modified|=fabs(pAngle->cut_angle[2][0]-cut_angle[2][0])>0.5;
			modified|=fabs(pAngle->cut_angle[2][1]-cut_angle[2][1])>0.5;
			modified|=fabs(pAngle->cut_angle[3][0]-cut_angle[3][0])>0.5;
			modified|=fabs(pAngle->cut_angle[3][1]-cut_angle[3][1])>0.5;
			modified|=(pAngle->cut_wing[1]!=cut_wing[1]);
			modified|=fabs(pAngle->cut_wing_para[1][0]-cut_wing_para[1][0])>0.5;
			modified|=fabs(pAngle->cut_wing_para[1][1]-cut_wing_para[1][1])>0.5;
			modified|=fabs(pAngle->cut_wing_para[1][2]-cut_wing_para[1][2])>0.5;

			pAngle->m_bUserSpecEndCutAngle=m_bUserSpecEndCutAngle;
			memcpy(&pAngle->cut_angle[2][0],&cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],&cut_angle[3][0],sizeof(double)*2);
			pAngle->cut_wing[1]=cut_wing[1];
			memcpy(&pAngle->cut_wing_para[1][0],&cut_wing_para[1][0],sizeof(double)*3);
		}
		if(reverseWingXnY)
		{	//X��Y֫�ߵ�
			double tmparr[3];
			memcpy(tmparr,&pAngle->cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[2][0],&pAngle->cut_angle[3][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],tmparr,sizeof(double)*2);

			if(pAngle->cut_wing[1]==1)
				pAngle->cut_wing[1]=2;
			else if(pAngle->cut_wing[1]==2)
				pAngle->cut_wing[1]=1;
		}
		if(modified)
		{
			pAngle->SetModified(modified);
			AppendModifiedAngles(pAngle);
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cutAngleWing")==0)&&IsSyncMirPropWith(pDestObj,"cutAngleWing"))
	{	//ʼ���н���֫
		memcpy(pAngle->cut_angle,cut_angle,sizeof(double)*8);
		memcpy(pAngle->cut_wing,cut_wing,sizeof(long)*2);
		memcpy(pAngle->cut_wing_para,cut_wing_para,sizeof(double)*6);
		pAngle->m_bUserSpecStartCutAngle=m_bUserSpecStartCutAngle;
		pAngle->m_bUserSpecEndCutAngle=m_bUserSpecEndCutAngle;
		if(reverseStartEnd)
		{	//ʼĩ�˵ߵ�
			pAngle->m_bUserSpecStartCutAngle=m_bUserSpecEndCutAngle;
			pAngle->m_bUserSpecEndCutAngle=m_bUserSpecStartCutAngle;
			memcpy(&pAngle->cut_angle[0][0],&cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],&cut_angle[3][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[2][0],&cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],&cut_angle[1][0],sizeof(double)*2);
			pAngle->cut_wing[0]=cut_wing[1];
			pAngle->cut_wing[1]=cut_wing[0];
			memcpy(&pAngle->cut_wing_para[0][0],&cut_wing_para[1][0],sizeof(double)*3);
			memcpy(&pAngle->cut_wing_para[1][0],&cut_wing_para[0][0],sizeof(double)*3);
		}
		if(reverseWingXnY)
		{	//X��Y֫�ߵ�
			double tmparr[3];
			memcpy(tmparr,&pAngle->cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[0][0],&pAngle->cut_angle[1][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],tmparr,sizeof(double)*2);
			memcpy(tmparr,&pAngle->cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[2][0],&pAngle->cut_angle[3][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],tmparr,sizeof(double)*2);

			if(pAngle->cut_wing[0]==1)
				pAngle->cut_wing[0]=2;
			else if(pAngle->cut_wing[0]==2)
				pAngle->cut_wing[0]=1;
			if(pAngle->cut_wing[1]==1)
				pAngle->cut_wing[1]=2;
			else if(pAngle->cut_wing[1]==2)
				pAngle->cut_wing[1]=1;
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"push_flat")==0)&&IsSyncMirPropWith(pDestObj,"push_flat"))
	{	//ѹ����Ϣ
		pAngle->start_push_flat_length=start_push_flat_length;
		pAngle->end_push_flat_length=end_push_flat_length;
		pAngle->start_push_flat_x1_y2=start_push_flat_x1_y2;
		pAngle->end_push_flat_x1_y2=end_push_flat_x1_y2;
		if(reverseStartEnd)
		{	//ʼĩ�˵ߵ�
			pAngle->start_push_flat_length=end_push_flat_length;
			pAngle->end_push_flat_length=start_push_flat_length;
			pAngle->start_push_flat_x1_y2=end_push_flat_x1_y2;
			pAngle->end_push_flat_x1_y2=start_push_flat_x1_y2;
		}
		if(reverseWingXnY)
		{	//X��Y֫�ߵ�
			if(pAngle->start_push_flat_x1_y2==1)
				pAngle->start_push_flat_x1_y2=2;
			if(pAngle->start_push_flat_x1_y2==2)
				pAngle->start_push_flat_x1_y2=1;
			if(pAngle->end_push_flat_x1_y2==1)
				pAngle->end_push_flat_x1_y2=2;
			if(pAngle->end_push_flat_x1_y2==2)
				pAngle->end_push_flat_x1_y2=1;
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"manuProcess")==0)&&IsSyncMirPropWith(pDestObj,"manuProcess"))
	{	//���๤����Ϣ","��������������ӵȹ���
		pAngle->m_blScatterBoltLocaTolerance=m_blScatterBoltLocaTolerance;
		pAngle->m_bCutRoot=m_bCutRoot;
		pAngle->m_bCutBer=m_bCutBer;
		pAngle->m_bWeldPart=m_bWeldPart;
	}
	pDestObj->SetModified();
	return true;
}
bool CLDSLineAngle::SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL)
		return false;
	CLDSLineAngle* pAngle=(CLDSLineAngle*)pDestObj;
	if(propKeyStr==NULL||strcmp(propKeyStr,"synchronize_scope")==0)
	{
		MEMORY_BLOCK cl=mpcl;
		if(cl.IsZero())
			cl=MPCL;
		PROPLIST_ITEM* pItem=propMirSync.GetPropItem("synchronize_scope");
		if(pItem!=NULL && cl.GetBitState((WORD)pItem->id-1))
			pDestObj->mpcl=mpcl;
	}
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__TSA_)
	pAngle->m_cbRodFlag=m_cbRodFlag;
#endif
	if((propKeyStr==NULL||strcmp(propKeyStr,"layer")==0)&&IsSyncMirPropWith(pDestObj,"layer"))
	{	//ͼ����ͬ���Գ�
		MirObjLayer(layer(),pAngle->layer(),mirmsg);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//����
		pAngle->cMaterial=cMaterial;
		pAngle->cbVirginQualityLevel=cbVirginQualityLevel;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//���
		pAngle->size_wide=size_wide;
		pAngle->size_height=size.height;
		pAngle->size_thick=size_thick;
		pAngle->size_idClassType=size_idClassType;
		pAngle->size_cSubClassType=size_cSubClassType;
		if(m_nClassTypeId==CLS_GROUPLINEANGLE)
		{
			((CLDSGroupLineAngle*)pAngle)->jg_space=((CLDSGroupLineAngle*)this)->jg_space;
			((CLDSGroupLineAngle*)pAngle)->ciMasterIndex=((CLDSGroupLineAngle*)this)->ciMasterIndex;
		}
		if( GetClassTypeId()==CLS_GROUPLINEANGLE&&size.cSubClassType=='T'&&
			(mirmsg.axis_flag==0x01||mirmsg.axis_flag==0x02||mirmsg.axis_flag==0x10))
		{	//T����ϽǸ�X/Y/����Գ�ʱӦ���������ӽǸֵ�������
			((CLDSGroupLineAngle*)pAngle)->ciMasterIndex=1-((CLDSGroupLineAngle*)this)->ciMasterIndex;
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//�ֶκ�
		pAngle->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"sPartNo")==0)&&IsSyncMirPropWith(pDestObj,"sPartNo"))
	{	//�������
		pAngle->SetPartNo(sPartNo);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//��ĺ�
		pAngle->cfgword=cfgword;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"m_bDisplayHole")==0)&&IsSyncMirPropWith(pDestObj,"m_bDisplayHole"))
	{	//��ʾ��
		pAngle->m_bDisplayHole=m_bDisplayHole;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"crMaterial")==0)&&IsSyncMirPropWith(pDestObj,"crMaterial"))
	{	//������ɫ
		pAngle->crMaterial=crMaterial;
	}
	bool reverseStartEnd=false;
	bool reverseWingXnY=false;
	if(pStart && pAngle->pEnd)
	{
		f3dPoint pos=pStart->Position(false);
		f3dPoint mirpos=pAngle->pEnd->Position(false);
		f3dPoint destpos=mirmsg.MirPoint(pos);
		if(mirpos.IsEqual(destpos))
			reverseStartEnd=true;	//�ԳƽǸ���Ҫ�ߵ�ʼĩ��
	}
	else if(pEnd && pAngle->pStart)
	{
		f3dPoint pos=pEnd->Position(false);
		f3dPoint mirpos=pAngle->pStart->Position(false);
		f3dPoint destpos=mirmsg.MirPoint(pos);
		if(mirpos.IsEqual(destpos))
			reverseStartEnd=true;	//�ԳƽǸ���Ҫ�ߵ�ʼĩ��
	}
	//T����ϽǸֶԳƺ�ʼĩ�˵�ͷ��Ҳ����������ӽǸֵ������� wjh-2018.7.1
	if(reverseStartEnd&&GetClassTypeId()==CLS_GROUPLINEANGLE&&size.cSubClassType=='T')
		((CLDSGroupLineAngle*)pAngle)->ciMasterIndex=1-((CLDSGroupLineAngle*)this)->ciMasterIndex;
#ifdef __PART_DESIGN_INC_
	if((propKeyStr==NULL||strcmp(propKeyStr,"DesignInfo")==0))//&&IsSyncMirPropWith(pDestObj,"DesignInfo"))
	{
		pAngle->CalLenCoef.iTypeNo=CalLenCoef.iTypeNo;
		pAngle->CalLenCoef.minR.coef=CalLenCoef.minR.coef;
		CLDSNode* pNode,*pMirNode=NULL;
		pNode=CalLenCoef.minR.hStartNode>0x20?BelongModel()->FromNodeHandle(CalLenCoef.minR.hStartNode):NULL;
		pMirNode=pNode!=NULL?pNode->GetMirNode(mirmsg):NULL;
		if(pMirNode)
			pAngle->CalLenCoef.minR.hStartNode=pMirNode->handle;
		else
			pAngle->CalLenCoef.minR.hStartNode=0;
		pNode=CalLenCoef.minR.hEndNode>0x20?BelongModel()->FromNodeHandle(CalLenCoef.minR.hEndNode):NULL;
		pMirNode=pNode!=NULL?pNode->GetMirNode(mirmsg):NULL;
		if(pMirNode)
			pAngle->CalLenCoef.minR.hEndNode=pMirNode->handle;
		else
			pAngle->CalLenCoef.minR.hEndNode=0;

		pAngle->CalLenCoef.paraR.coef=CalLenCoef.paraR.coef;
		pNode=CalLenCoef.paraR.hStartNode>0x20?BelongModel()->FromNodeHandle(CalLenCoef.paraR.hStartNode):NULL;
		pMirNode=pNode!=NULL?pNode->GetMirNode(mirmsg):NULL;
		if(pMirNode)
			pAngle->CalLenCoef.paraR.hStartNode=pMirNode->handle;
		else
			pAngle->CalLenCoef.paraR.hStartNode=0;
		pNode=CalLenCoef.paraR.hEndNode>0x20?BelongModel()->FromNodeHandle(CalLenCoef.paraR.hEndNode):NULL;
		pMirNode=pNode!=NULL?pNode->GetMirNode(mirmsg):NULL;
		if(pMirNode)
			pAngle->CalLenCoef.paraR.hEndNode=pMirNode->handle;
		else
			pAngle->CalLenCoef.paraR.hEndNode=0;
		MirPartHandle(BelongModel(),&pAngle->CalLenCoef.hHorizPole,CalLenCoef.hHorizPole,mirmsg);
		MirPartHandle(BelongModel(),&pAngle->CalLenCoef.hRefPole,CalLenCoef.hRefPole,mirmsg);
		pAngle->m_bAutoCalHoleOutCoef=m_bAutoCalHoleOutCoef;
		pAngle->hole_out=hole_out;
		pAngle->sel_mat_method=sel_mat_method;
		if(!reverseStartEnd)
		{
			pAngle->start_force_type=start_force_type;
			pAngle->start_joint_type=start_joint_type;
			pAngle->end_force_type=end_force_type;
			pAngle->end_joint_type=end_joint_type;
		}
		else
		{
			pAngle->start_force_type=end_force_type;
			pAngle->start_joint_type=end_joint_type;
			pAngle->end_force_type=start_force_type;
			pAngle->end_joint_type=start_joint_type;
		}
		pAngle->m_iElemType=m_iElemType;
		pAngle->m_bNeedSelSize=m_bNeedSelSize;
		pAngle->m_fWeightRaiseCoef=m_fWeightRaiseCoef;
	}
#endif
	f3dPoint dest_norm_x_wing=mirmsg.MirVector(norm_x_wing);
	f3dPoint dest_norm_y_wing=mirmsg.MirVector(norm_y_wing);
	f3dPoint mir_norm_x_wing=pAngle->get_norm_x_wing();
	f3dPoint mir_norm_y_wing=pAngle->get_norm_y_wing();
	char cWorkWing=0;	//Ĭ���޷�����ȷ������֫
	//TODO:���SyncMirPropTo�����Ƿ�ȱ��'mirmsg.axis_flag==0x08'�ж�
	//if((mirmsg.axis_flag&0x04)==0x04||mirmsg.axis_flag==0x08)	//Z��(�����ת)�Գ�,ʼĩ�˽ڵ�ߵ�,X/Y֫Ҳ����
	if((mirmsg.axis_flag&0x04)==0x04)	//Z��(�����ת)�Գ�,ʼĩ�˽ڵ�ߵ�,X/Y֫Ҳ����
	{
		cWorkWing=1;
		reverseWingXnY=reverseStartEnd;
	}
	else if(mirmsg.axis_flag&0x13)
	{
		if(des_norm_x.bByOtherWing)
			cWorkWing='y';	//Y֫Ϊ����֫
		else if(des_norm_y.bByOtherWing)
			cWorkWing='x';	//X֫Ϊ����֫
		else if(des_norm_x.bSpecific&&des_norm_x.spec_norm.norm_style==0&&des_norm_y.hViceJg>0x20)
			cWorkWing='y';	//Y֫Ϊ����֫
		else if(des_norm_y.bSpecific&&des_norm_y.spec_norm.norm_style==0&&des_norm_x.hViceJg>0x20)
			cWorkWing='x';	//X֫Ϊ����֫
		else if(desStartPos.cWorkWingStyle==1||desEndPos.cWorkWingStyle==1)
			cWorkWing='x';
		else if(desStartPos.cWorkWingStyle==2||desEndPos.cWorkWingStyle==2)
			cWorkWing='y';

		if(MirLayStyle==CLDSLineAngle::MIRXorY_SAMEDIRECT)	//X/Y�Գ�ʱ����ͬ��
			reverseWingXnY=!reverseStartEnd;//true;
		else if(cWorkWing==0||MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
		{	//������ȷ������֫ʱ,ֻ��֫���߽���΢��,������֫����X/Y֫�����ȴ�ı䶯
			if(fabs(dest_norm_x_wing*mir_norm_x_wing)<fabs(dest_norm_x_wing*mir_norm_y_wing))
				reverseWingXnY=true;	//��Ҫ�ߵ�X��Y֫�ķ��߲���
			else
				reverseWingXnY=false;
		}
		else if(MirLayStyle==CLDSLineAngle::MIRXorY_INVERT)
		{	//X/Y�Գ�ʱ���ⷴ��
			cWorkWing=toupper(cWorkWing);	//תΪ��д��ĸ, ��ʾ����֫���߷���Ҫ����
			reverseWingXnY=reverseStartEnd;
		}
	}
	if(size.height>0&&size.wide!=size.height&&(propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&
		IsSyncMirPropWith(pDestObj,"specification"))
	{	//�ԳƲ��ȱ߽Ǹֵ�֫������
		if(!reverseWingXnY)
			pAngle->size_height=size.height;
		else
		{
			pAngle->size_wide=size.height;
			pAngle->size_height=size.wide;
		}
	}
	//��֫����ͬ���ԳƸ�������λ�öԳƸ���֮ǰ����Ϊ����Ӧ����ʱ����֫���߳����Ӧ�涯����֫����ƫ���� wjh-2015.6.24
	char ciStateDestWingNormX=0,ciStateDestWingNormY=0;	//0.��ʾ���䣬��1��ʾ��֫����2��ʾ��֫
	GEPOINT vxWingNormBackup=pAngle->vxWingNorm,vyWingNormBackup=pAngle->vyWingNorm;
	if((propKeyStr==NULL||strcmp(propKeyStr,"wingNormInfo")==0)&&IsSyncMirPropWith(pDestObj,"wingNormInfo"))
	{	//֫������Ϣ
		CLDSGroupLineAngle* pGroupAngle=NULL;
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			pGroupAngle=(CLDSGroupLineAngle*)pAngle;
			pGroupAngle->m_bSpecAngleWingNorm=((CLDSGroupLineAngle*)this)->m_bSpecAngleWingNorm;
		}
		int signx=1,signy=1;
		if(cWorkWing=='X')	//X֫Ϊ��ǰ����֫,�ҶԳ�ʱ��Ҫ����
			signx=-1;
		else if(cWorkWing=='Y')
			signy=-1;
		pAngle->MirLayStyle=MirLayStyle;
		//�������д����ֹ��Ϊ��ǰ���Ʒ��ߴ����¶Գƺ���Գ����½Ǹ�֫����ͬ������ wjh-2018.12.24
		if(this->des_norm_x.IsByViceRodActual()&&fabs(norm_x_wing*des_norm_x.near_norm)<0.2)
			des_norm_x.near_norm=norm_x_wing;	//���Ʒ�����ʵ��֫���߲�ƥ�䣨�ӽ���ֱ����ֱ���õ�ǰʵ�ʷ��ߴ�����Ʒ��� 18-12-24
		if(this->des_norm_y.IsByViceRodActual()&&fabs(norm_y_wing*des_norm_y.near_norm)<0.2)
			des_norm_y.near_norm=norm_y_wing;	//���Ʒ�����ʵ��֫���߲�ƥ�䣨�ӽ���ֱ����ֱ���õ�ǰʵ�ʷ��ߴ�����Ʒ��� 18-12-24
		f3dPoint dest_norm_x_wing=mirmsg.MirVector(signx*norm_x_wing);
		f3dPoint dest_norm_y_wing=mirmsg.MirVector(signy*norm_y_wing);
		f3dPoint mir_norm_x_wing=pAngle->get_norm_x_wing();
		f3dPoint mir_norm_y_wing=pAngle->get_norm_y_wing();
		if(!reverseWingXnY)
		{
			pAngle->des_norm_x.bByOtherWing=des_norm_x.bByOtherWing;
			pAngle->des_norm_x.bSpecific=des_norm_x.bSpecific;
			MirPartHandle(BelongModel(),&pAngle->des_norm_x.hViceJg,des_norm_x.hViceJg,mirmsg);
			pAngle->des_norm_x.near_norm=mirmsg.MirVector(des_norm_x.near_norm);
			des_norm_x.spec_norm.MirToVector(BelongModel(),pAngle->des_norm_x.spec_norm,mirmsg);
			if(cWorkWing=='X'){
				pAngle->des_norm_x.near_norm*=-1.0;
				pAngle->des_norm_x.spec_norm.ReverseDirection();
			}
			if(MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			{
				if(mir_norm_x_wing*dest_norm_x_wing>0)
					pAngle->norm_x_wing=dest_norm_x_wing;
					//pAngle->set_norm_x_wing( dest_norm_x_wing,FALSE);
				else
				{
					pAngle->norm_x_wing=-dest_norm_x_wing;
					//pAngle->set_norm_x_wing(-dest_norm_x_wing,FALSE);
					pAngle->des_norm_x.near_norm*=-1;
					pAngle->des_norm_x.spec_norm.ReverseDirection();
				}
			}
			else
				pAngle->norm_x_wing=dest_norm_x_wing;
				//pAngle->set_norm_x_wing( dest_norm_x_wing,FALSE);
			/*	if(mir_norm_x_wing*dest_norm_x_wing>0 || cWorkWing!=0)
				pAngle->set_norm_x_wing( dest_norm_x_wing,FALSE);
			else
			{
				pAngle->set_norm_x_wing(-dest_norm_x_wing,FALSE);
				pAngle->des_norm_x.near_norm*=-1;
				pAngle->des_norm_x.spec_norm.ReverseDirection();
			}*/

			pAngle->des_norm_y.bByOtherWing=des_norm_y.bByOtherWing;
			pAngle->des_norm_y.bSpecific=des_norm_y.bSpecific;
			MirPartHandle(BelongModel(),&pAngle->des_norm_y.hViceJg,des_norm_y.hViceJg,mirmsg);
			pAngle->des_norm_y.near_norm=mirmsg.MirVector(des_norm_y.near_norm);
			des_norm_y.spec_norm.MirToVector(BelongModel(),pAngle->des_norm_y.spec_norm,mirmsg);
			if(cWorkWing=='Y'){
				pAngle->des_norm_y.near_norm*=-1.0;
				pAngle->des_norm_y.spec_norm.ReverseDirection();
			}
			if(MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			{
				if(mir_norm_y_wing*dest_norm_y_wing>0)
					pAngle->norm_y_wing=dest_norm_y_wing;
					//pAngle->set_norm_y_wing( dest_norm_y_wing,FALSE);
				else
				{
					pAngle->norm_y_wing=-dest_norm_y_wing;
					//pAngle->set_norm_y_wing(-dest_norm_y_wing,FALSE);
					pAngle->des_norm_y.near_norm*=-1;
					pAngle->des_norm_y.spec_norm.ReverseDirection();
				}
			}
			else
				pAngle->norm_y_wing=dest_norm_y_wing;
				//pAngle->set_norm_y_wing( dest_norm_y_wing,FALSE);
			/*if(mir_norm_y_wing*dest_norm_y_wing>0 || cWorkWing!=0)
				pAngle->set_norm_y_wing( dest_norm_y_wing,FALSE);
			else
			{
				pAngle->set_norm_y_wing(-dest_norm_y_wing,FALSE);
				pAngle->des_norm_y.near_norm*=-1;
				pAngle->des_norm_y.spec_norm.ReverseDirection();
			}*/
			//pAngle->des_norm_x.near_norm �ǽ��Ʒ���,����������ԭ��һ���Գ�,�ʱ���ԭֵ����
		}
		else
		{
			pAngle->des_norm_x.bByOtherWing=des_norm_y.bByOtherWing;
			pAngle->des_norm_x.bSpecific=des_norm_y.bSpecific;
			MirPartHandle(BelongModel(),&pAngle->des_norm_x.hViceJg,des_norm_y.hViceJg,mirmsg);
			pAngle->des_norm_x.near_norm=mirmsg.MirVector(des_norm_y.near_norm);
			des_norm_y.spec_norm.MirToVector(BelongModel(),pAngle->des_norm_x.spec_norm,mirmsg);
			if(cWorkWing=='X'){
				pAngle->des_norm_x.near_norm*=-1.0;
				pAngle->des_norm_x.spec_norm.ReverseDirection();
			}
			if(MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			{
				if(mir_norm_x_wing*dest_norm_y_wing>0)
					pAngle->norm_x_wing=dest_norm_y_wing;
					//pAngle->set_norm_x_wing( dest_norm_y_wing,FALSE);
				else
				{
					pAngle->norm_x_wing=-dest_norm_y_wing;
					//pAngle->set_norm_x_wing(-dest_norm_y_wing,FALSE);
					pAngle->des_norm_x.near_norm*=-1;
					pAngle->des_norm_x.spec_norm.ReverseDirection();
				}
			}
			else if(MirLayStyle==CLDSLineAngle::MIRXorY_SAMEDIRECT)
				pAngle->norm_x_wing=dest_norm_y_wing;
			//TODO:�����뻹��Ҫ��ϸ��� wjh-2018.5.11
			else if(mir_norm_x_wing*dest_norm_y_wing>0 || cWorkWing!=0)
				pAngle->norm_x_wing=dest_norm_y_wing;
				//pAngle->set_norm_x_wing( dest_norm_y_wing,FALSE);
			else
			{
				pAngle->norm_x_wing=-dest_norm_y_wing;
				//pAngle->set_norm_x_wing(-dest_norm_y_wing,FALSE);
				pAngle->des_norm_x.near_norm*=-1;
				pAngle->des_norm_x.spec_norm.ReverseDirection();
			}

			pAngle->des_norm_y.bByOtherWing=des_norm_x.bByOtherWing;
			pAngle->des_norm_y.bSpecific=des_norm_x.bSpecific;
			MirPartHandle(BelongModel(),&pAngle->des_norm_y.hViceJg,des_norm_x.hViceJg,mirmsg);
			pAngle->des_norm_y.near_norm=mirmsg.MirVector(des_norm_x.near_norm);
			des_norm_x.spec_norm.MirToVector(BelongModel(),pAngle->des_norm_y.spec_norm,mirmsg);
			if(cWorkWing=='Y'){
				pAngle->des_norm_y.near_norm*=-1.0;
				pAngle->des_norm_y.spec_norm.ReverseDirection();
			}
			if(MirLayStyle==CLDSLineAngle::MIRXorY_UNCHAHNGE)
			{
				if(mir_norm_y_wing*dest_norm_x_wing>0)
					pAngle->norm_y_wing = dest_norm_x_wing;
					//pAngle->set_norm_y_wing( dest_norm_x_wing,FALSE);
				else
				{
					pAngle->norm_y_wing =-dest_norm_x_wing;
					//pAngle->set_norm_y_wing(-dest_norm_x_wing,FALSE);
					pAngle->des_norm_y.near_norm*=-1;
					pAngle->des_norm_y.spec_norm.ReverseDirection();
				}
			}
			else if(MirLayStyle==CLDSLineAngle::MIRXorY_SAMEDIRECT)
				pAngle->norm_y_wing=dest_norm_x_wing;
			//TODO:�����뻹��Ҫ��ϸ��� wjh-2018.5.11
			else if(mir_norm_y_wing*dest_norm_x_wing>0 || cWorkWing!=0)
				pAngle->norm_y_wing = dest_norm_x_wing;
				//pAngle->set_norm_y_wing( dest_norm_x_wing,FALSE);
			else
			{
				pAngle->norm_y_wing =-dest_norm_x_wing;
				//pAngle->set_norm_y_wing(-dest_norm_x_wing,FALSE);
				pAngle->des_norm_y.near_norm*=-1;
				pAngle->des_norm_y.spec_norm.ReverseDirection();
			}
		}
		double jxx=pAngle->vxWingNorm*vxWingNormBackup;
		double jyy=pAngle->vyWingNorm*vyWingNormBackup;
		double jxy=pAngle->vxWingNorm*vyWingNormBackup;
		double jyx=pAngle->vyWingNorm*vxWingNormBackup;
		if(fabs(jxx)>fabs(jxy))
		{
			if(jxx>0)
				ciStateDestWingNormX= 1;
			else //if(jxx<0)
				ciStateDestWingNormX=-1;
		}
		else
		{
			if(jxy>0)
				ciStateDestWingNormX= 2;
			else //if(jxy<0)
				ciStateDestWingNormX=-2;
		}
		if(fabs(jyy)>fabs(jyx))
		{
			if(jyy>0)
				ciStateDestWingNormY= 2;
			else //if(jyy<0)
				ciStateDestWingNormY=-2;
		}
		else
		{
			if(jyx>0)
				ciStateDestWingNormY= 1;
			else //if(jyx<0)
				ciStateDestWingNormY=-1;
		}
		if (pGroupAngle)
			pGroupAngle->UpdateSonJgWingNorm();
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"Position")==0)&&IsSyncMirPropWith(pDestObj,"Position"))
	{	//��������������
		if(mirmsg.axis_flag==0x01||mirmsg.axis_flag==0x02||mirmsg.axis_flag==0x04)
			SetArcLift(m_hArcLift);	//������ΰ��������Բ��Ԥ��������ͬ������
		pAngle->m_cPosCtrlDatumLineType=m_cPosCtrlDatumLineType;//'X':X֫��׼���߿���;'Y':Y֫��׼���߿���;�������߿���
		if(reverseWingXnY&&(m_cPosCtrlDatumLineType=='X'||m_cPosCtrlDatumLineType=='Y'))
			pAngle->m_cPosCtrlDatumLineType='X'+'Y'-m_cPosCtrlDatumLineType;
		pAngle->m_cPosCtrlType=m_cPosCtrlType;	//λ�ÿ�������:0.����ͶӰ;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
		for(int i=0;i<2;i++)
		{
			CMultiOffsetPos* pDestPos,*pSrcPos;
			if(i==0)
			{
				pSrcPos=&desStartPos;
				pDestPos=&pAngle->desStartPos;
				if(reverseStartEnd)
					pDestPos=&pAngle->desEndPos;
			}
			else
			{
				pSrcPos=&desEndPos;
				pDestPos=&pAngle->desEndPos;
				if(reverseStartEnd)
					pDestPos=&pAngle->desStartPos;
			}
			pDestPos->jgber_cal_style=pSrcPos->jgber_cal_style;
			pDestPos->origin_pos_style=pSrcPos->origin_pos_style;	//��ֹ��̳о��ļ�ʱorigin_pos_style����ʱ�޷����¸����ԡ�wjh-2016.11.18
			if(pDestPos->jgber_cal_style==2)	//�򵥶�λʱ���Ӵ����Կ���
				pDestPos->cbSimpleUdfFlag=pSrcPos->cbSimpleUdfFlag;
			f3dPoint pickpos=Start();
			if(i==1)
				pickpos=End();
			if(pSrcPos->datum_jg_h>0x20)
				MirPartHandle(BelongModel(),&pDestPos->datum_jg_h,pSrcPos->datum_jg_h,mirmsg,&pickpos);
			CLDSLineAngle* pDependAngle=NULL,*pMirDependAngle=NULL;
			bool reverseDependXnY=false;
			bool reverseDependStartEnd=false;
			if(pSrcPos->spatialOperationStyle>=1&&pSrcPos->spatialOperationStyle<=14)
			{
				if(pSrcPos->datum_jg_h>0x20)
					pDependAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pSrcPos->datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pDependAngle==NULL && (i==0&&pStart!=NULL))
					pDependAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pDependAngle==NULL && (i==1&&pEnd!=NULL))
					pDependAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pDestPos->datum_jg_h>0x20)
					pMirDependAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pDestPos->datum_jg_h,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				if(pMirDependAngle==NULL)
				{
					if(i==0)
					{
						if(!reverseStartEnd && pAngle->pStart!=NULL)
							pMirDependAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pAngle->pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
						else if(reverseStartEnd && pAngle->pEnd!=NULL)
							pMirDependAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pAngle->pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					}
					else
					{
						if(!reverseStartEnd && pAngle->pEnd!=NULL)
							pMirDependAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pAngle->pEnd->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
						else if(reverseStartEnd && pAngle->pStart!=NULL)
							pMirDependAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pAngle->pStart->hFatherPart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					}
				}
				if(pDependAngle!=NULL && pMirDependAngle!=NULL)
				{
					f3dPoint dest_norm_x_wing_tmp=mirmsg.MirVector(pDependAngle->get_norm_x_wing());
					f3dPoint dest_norm_y_wing_tmp=mirmsg.MirVector(pDependAngle->get_norm_y_wing());
					f3dPoint mir_norm_x_wing_tmp=pMirDependAngle->get_norm_x_wing();
					f3dPoint mir_norm_y_wing_tmp=pMirDependAngle->get_norm_y_wing();
					if(fabs(dest_norm_x_wing_tmp*mir_norm_x_wing_tmp)<fabs(dest_norm_x_wing_tmp*mir_norm_y_wing_tmp))
						reverseDependXnY=true;//�����Ǹ�����ԳƽǸֵߵ�X��Y֫�ķ��߲���
					if(pDependAngle->pStart && pMirDependAngle->pEnd)
					{
						f3dPoint pos=pDependAngle->pStart->Position(false);
						f3dPoint mirpos=pMirDependAngle->pEnd->Position(false);
						f3dPoint destpos=mirmsg.MirPoint(pos);
						if(mirpos.IsEqual(destpos))
							reverseDependStartEnd=true;	//�ԳƽǸ���Ҫ�ߵ�ʼĩ��
					}
					else if(pDependAngle->pEnd && pMirDependAngle->pStart)
					{
						f3dPoint pos=pDependAngle->pEnd->Position(false);
						f3dPoint mirpos=pMirDependAngle->pStart->Position(false);
						f3dPoint destpos=mirmsg.MirPoint(pos);
						if(mirpos.IsEqual(destpos))
							reverseDependStartEnd=true;	//�ԳƽǸ���Ҫ�ߵ�ʼĩ��
					}
				}
			}
			else if (pSrcPos->spatialOperationStyle>=15)	//���Ƶ㼰����ͶӰ/ƽ��Ҳ����·�ʽ���� wjh-2020.2.10
				pDestPos->spatialOperationStyle=pSrcPos->spatialOperationStyle;
			//����ֱ��ָ�����������ʽ�ĶԳ�ͬ������
			if(i==0&&desStartPos.jgber_cal_style==1)
			{
				f3dPoint destStart=mirmsg.MirPoint(Start());
				if(reverseDependStartEnd)
				{
					pAngle->desEndPos.jgber_cal_style=1;
					pAngle->SetEnd(destStart);
				}
				else
				{
					pAngle->desStartPos.jgber_cal_style=1;
					pAngle->SetStart(destStart);
				}
				continue;
			}
			else if(i==1&&desEndPos.jgber_cal_style==1)
			{
				f3dPoint destEnd=mirmsg.MirPoint(End());
				if(reverseDependStartEnd)
				{
					pAngle->desStartPos.jgber_cal_style=1;
					pAngle->SetStart(destEnd);
				}
				else
				{
					pAngle->desEndPos.jgber_cal_style=1;
					pAngle->SetEnd(destEnd);
				}
				continue;
			}
			if(pSrcPos->spatialOperationStyle==0)
			{
				pDestPos->spatialOperationStyle=0;//pSrcPos->spatialOperationStyle;
				pSrcPos->datumPoint.MirToPoint(BelongModel(),pDestPos->datumPoint,mirmsg);
				if(reverseWingXnY)
				{
					pDestPos->wing_x_offset=pSrcPos->wing_y_offset;
					pDestPos->wing_y_offset=pSrcPos->wing_x_offset;
				}
				else
				{
					pDestPos->wing_x_offset=pSrcPos->wing_x_offset;
					pDestPos->wing_y_offset=pSrcPos->wing_y_offset;
				}
			}
			else if(pSrcPos->spatialOperationStyle<=14)
			{
				pDestPos->origin_pos_style=pSrcPos->origin_pos_style;
				if(pSrcPos->IsFaceOffset())
					pDestPos->face_offset_norm=mirmsg.MirVector(pSrcPos->face_offset_norm);
				pDestPos->spatialOperationStyle=pSrcPos->spatialOperationStyle;
				pDestPos->len_offset_dist=pSrcPos->len_offset_dist;
				pDestPos->len_offset_type=pSrcPos->len_offset_type;
				//�������Ǹ���β�ߵ����ʴ�������ƫ��ҲӦ����
				if(pSrcPos->len_offset_type==0&&fabs(pSrcPos->len_offset_dist)>0&&reverseDependStartEnd)//&&(mirmsg.axis_flag&0x04)==0)
					pDestPos->len_offset_dist=-pSrcPos->len_offset_dist;
				//else if(pSrcPos->len_offset_type==0&&fabs(pSrcPos->len_offset_dist)>0&&!reverseDependStartEnd&&(mirmsg.axis_flag&0x04)>0)
				//	pDestPos->len_offset_dist=-pSrcPos->len_offset_dist;
				else if(pSrcPos->len_offset_type=='X'&&(mirmsg.axis_flag==0x02||mirmsg.axis_flag==0x04))
					pDestPos->len_offset_dist=-pSrcPos->len_offset_dist;
				else if(pSrcPos->len_offset_type=='Y'&&(mirmsg.axis_flag==0x01||mirmsg.axis_flag==0x04))
					pDestPos->len_offset_dist=-pSrcPos->len_offset_dist;
				pDestPos->datum_offset_dist=pSrcPos->datum_offset_dist;
				pDestPos->fEccentricDist=pSrcPos->fEccentricDist;
				if(pSrcPos->IsInHuoQuLine())
				{
					MirPartHandle(BelongModel(),&pDestPos->huoqu_jg_h,pSrcPos->huoqu_jg_h,mirmsg);
					pDestPos->huoqu_dist=pSrcPos->huoqu_dist;
					if(pSrcPos->huoqu_pos_auto_0_x_wing1_y_wing2==0)
						pDestPos->huoqu_pos_auto_0_x_wing1_y_wing2=pSrcPos->huoqu_pos_auto_0_x_wing1_y_wing2;
					else
						Log2File()->Log("��δ��������߻�׼֫�ĶԳ��ж�");
				}
				if(pSrcPos->IsInOffsetZhunLine())
					pDestPos->offset_zhun=pSrcPos->offset_zhun;
				if(reverseDependXnY)
				{	//�����Ǹ�����ԳƽǸֵߵ�X��Y֫�ķ��߲���
					if(pSrcPos->spatialOperationStyle==2||pSrcPos->spatialOperationStyle==3)
						pDestPos->spatialOperationStyle=5-pSrcPos->spatialOperationStyle;
					else if(pSrcPos->spatialOperationStyle==5||pSrcPos->spatialOperationStyle==6)
						pDestPos->spatialOperationStyle=11-pSrcPos->spatialOperationStyle;
					else if(pSrcPos->spatialOperationStyle==8||pSrcPos->spatialOperationStyle==9)
						pDestPos->spatialOperationStyle=17-pSrcPos->spatialOperationStyle;
					else if(pSrcPos->spatialOperationStyle==11||pSrcPos->spatialOperationStyle==12)
						pDestPos->spatialOperationStyle=23-pSrcPos->spatialOperationStyle;
					else if(pSrcPos->spatialOperationStyle==13||pSrcPos->spatialOperationStyle==14)
						pDestPos->spatialOperationStyle=27-pSrcPos->spatialOperationStyle;
				}
				pDestPos->cWorkWingStyle=pSrcPos->cWorkWingStyle;
				//�ж�֫���߶Գ��ԣ��������Գƺ���ʱ����ͬ������֫����ƫ���� wjh-2015.5.19
				f3dPoint dest_norm_x_wing=mirmsg.MirVector(norm_x_wing);
				f3dPoint dest_norm_y_wing=mirmsg.MirVector(norm_y_wing);
				f3dPoint mir_norm_x_wing=pAngle->get_norm_x_wing();
				f3dPoint mir_norm_y_wing=pAngle->get_norm_y_wing();
				double dd_x2x=dest_norm_x_wing*mir_norm_x_wing;
				double dd_x2y=dest_norm_x_wing*mir_norm_y_wing;
				double dd_y2x=dest_norm_y_wing*mir_norm_x_wing;
				double dd_y2y=dest_norm_y_wing*mir_norm_y_wing;
				WING_OFFSET_STYLE wing_x_offset_backup=pDestPos->wing_x_offset,wing_y_offset_backup=pDestPos->wing_y_offset;
				if(fabs(dd_x2x)<fabs(dd_x2y))
				{	//��Ҫ�ߵ�X��Y֫�ķ��߲���
					if(cWorkWing=='x'||cWorkWing=='X')
					{	//������Ŀ��Ǹֹ���֫����ƫ������ԭֵ����һ��(��������б��)��wjh-2018.1.3
						if(ciStateDestWingNormX==2||ciStateDestWingNormX==-2)
							pDestPos->wing_x_offset=wing_y_offset_backup;
						//else if(ciStateDestWingNormX==1)	//X->X
						if(ciStateDestWingNormX==-2)
							pDestPos->wing_x_offset.offsetDist*=-1;
					}
					else if(dd_y2x>0)
						pDestPos->wing_x_offset=pSrcPos->wing_y_offset;
					if(cWorkWing=='y'||cWorkWing=='Y')
					{
						if(ciStateDestWingNormY==1||ciStateDestWingNormY==-1)
							pDestPos->wing_y_offset=wing_x_offset_backup;
						//else if(ciStateDestWingNormX==1)	//Y->Y
						if(ciStateDestWingNormY==-1)
							pDestPos->wing_y_offset.offsetDist*=-1;
					}
					else if(dd_x2y>0)	//������֫�Գƺ�ͬ��ʱ�Ÿ��¹�������ƫ��
						pDestPos->wing_y_offset=pSrcPos->wing_x_offset;
					if(pDestPos->cWorkWingStyle==1||pDestPos->cWorkWingStyle==2)
						pDestPos->cWorkWingStyle=3-pDestPos->cWorkWingStyle;
				}
				else
				{
					if(cWorkWing=='x'||cWorkWing=='X')
					{
						if(ciStateDestWingNormX==2||ciStateDestWingNormX==-2)
							pDestPos->wing_x_offset=wing_y_offset_backup;
						//else if(ciStateDestWingNormX==1)	//X->X
						if(ciStateDestWingNormX==-2)
							pDestPos->wing_x_offset.offsetDist*=-1;
					}
					else if(dd_x2x>0)
						pDestPos->wing_x_offset=pSrcPos->wing_x_offset;
					if(cWorkWing=='y'||cWorkWing=='Y')
					{
						if(ciStateDestWingNormY==1||ciStateDestWingNormY==-1)
							pDestPos->wing_y_offset=wing_x_offset_backup;
						//else if(ciStateDestWingNormX==1)	//Y->Y
						if(ciStateDestWingNormY==-1)
							pDestPos->wing_y_offset.offsetDist*=-1;
					}
					else if(dd_y2y>0)
						pDestPos->wing_y_offset=pSrcPos->wing_y_offset;
				}
			}
			else if(pSrcPos->spatialOperationStyle==15)
			{	//��֫�������
				pDestPos->spatialOperationStyle=pSrcPos->spatialOperationStyle;
				pDestPos->cMainCoordCtrlWing=pSrcPos->cMainCoordCtrlWing;	//����֫
				pDestPos->coordCtrlType=pSrcPos->coordCtrlType;	//������������
				pDestPos->ctrlWing=pSrcPos->ctrlWing;
				pDestPos->otherWing=pSrcPos->otherWing;
				if( (pDestPos->ctrlWing.operStyle==0&&pDestPos->otherWing.operStyle!=0)||
					(pDestPos->ctrlWing.operStyle!=0&&pDestPos->otherWing.operStyle==0))
					pDestPos->fLenOffsetOfCoordWing=pSrcPos->fLenOffsetOfCoordWing;
				if(reverseWingXnY)
				{
					pDestPos->wing_x_offset=pSrcPos->wing_y_offset;
					pDestPos->wing_y_offset=pSrcPos->wing_x_offset;
					if(pSrcPos->cMainCoordCtrlWing=='X'||pSrcPos->cMainCoordCtrlWing=='Y')
						pDestPos->cMainCoordCtrlWing='X'+'Y'-pSrcPos->cMainCoordCtrlWing;
				}
				else
				{
					pDestPos->wing_x_offset=pSrcPos->wing_x_offset;
					pDestPos->wing_y_offset=pSrcPos->wing_y_offset;
				}
			}
			else if(pSrcPos->spatialOperationStyle==16||pSrcPos->spatialOperationStyle==17)
			{
				pDestPos->spatialOperationStyle=pSrcPos->spatialOperationStyle;
				if(pSrcPos->spatialOperationStyle==17)
					pDestPos->face_offset_norm=mirmsg.MirVector(pSrcPos->face_offset_norm);
				CLDSLine* pDatumLine=pSrcPos->hDatumLine>0x20?BelongModel()->FromLineHandle(pSrcPos->hDatumLine):NULL;
				if(pDatumLine&&(mirmsg.axis_flag==0x01||mirmsg.axis_flag==0x02||mirmsg.axis_flag==0x04))
				{
					pDestPos->hDatumLine=pSrcPos->hDatumLine;
					//�Գƻ�׼�涨λ�Ķ�λ������Ϣ
					if(mirmsg.axis_flag==0x01)	//X��Գ�
					{
						if(pSrcPos->cMirTransType==0)
							pDestPos->cMirTransType='X';
						else if(pSrcPos->cMirTransType=='X')
							pDestPos->cMirTransType=0;
						else if(pSrcPos->cMirTransType=='Y')
							pDestPos->cMirTransType='Z';
						else if(pSrcPos->cMirTransType=='Z')
							pDestPos->cMirTransType='Y';
					}
					else if(mirmsg.axis_flag==0x02)	//Y��Գ�
					{
						if(pSrcPos->cMirTransType==0)
							pDestPos->cMirTransType='Y';
						else if(pSrcPos->cMirTransType=='Y')
							pDestPos->cMirTransType=0;
						else if(pSrcPos->cMirTransType=='X')
							pDestPos->cMirTransType='Z';
						else if(pSrcPos->cMirTransType=='Z')
							pDestPos->cMirTransType='X';
					}
					else if(mirmsg.axis_flag==0x04)	//Z��Գ�
					{
						if(pSrcPos->cMirTransType==0)
							pDestPos->cMirTransType='Z';
						else if(pSrcPos->cMirTransType=='Z')
							pDestPos->cMirTransType=0;
						else if(pSrcPos->cMirTransType=='X')
							pDestPos->cMirTransType='Y';
						else if(pSrcPos->cMirTransType=='Y')
							pDestPos->cMirTransType='X';
					}
				}
				else
				{
					pSrcPos->datumLineStart.MirToPoint(BelongModel(),pDestPos->datumLineStart,mirmsg);
					pSrcPos->datumLineEnd.MirToPoint(BelongModel(),pDestPos->datumLineEnd,mirmsg);
				}
				pDestPos->cFaceOffsetDatumLine=pSrcPos->cFaceOffsetDatumLine;
				if(reverseWingXnY)
				{
					pDestPos->wing_x_offset=pSrcPos->wing_y_offset;
					pDestPos->wing_y_offset=pSrcPos->wing_x_offset;
					if(pSrcPos->cFaceOffsetDatumLine=='X'||pSrcPos->cFaceOffsetDatumLine=='Y')
						pDestPos->cFaceOffsetDatumLine='X'+'Y'-pSrcPos->cFaceOffsetDatumLine;
				}
				else
				{
					pDestPos->wing_x_offset=pSrcPos->wing_x_offset;
					pDestPos->wing_y_offset=pSrcPos->wing_y_offset;
				}
			}
			else if(pSrcPos->spatialOperationStyle==18||pSrcPos->spatialOperationStyle==19)
			{
				pDestPos->spatialOperationStyle=pSrcPos->spatialOperationStyle;
				if(pSrcPos->spatialOperationStyle==19)
					pDestPos->face_offset_norm=mirmsg.MirVector(pSrcPos->face_offset_norm);
				pDestPos->cFaceOffsetDatumLine=pSrcPos->cFaceOffsetDatumLine;
				if(reverseWingXnY)
				{
					pDestPos->wing_x_offset=pSrcPos->wing_y_offset;
					pDestPos->wing_y_offset=pSrcPos->wing_x_offset;
					if(pSrcPos->cFaceOffsetDatumLine=='X'||pSrcPos->cFaceOffsetDatumLine=='Y')
						pDestPos->cFaceOffsetDatumLine='X'+'Y'-pSrcPos->cFaceOffsetDatumLine;
				}
				else
				{
					pDestPos->wing_x_offset=pSrcPos->wing_x_offset;
					pDestPos->wing_y_offset=pSrcPos->wing_y_offset;
				}
			}
		}
		pAngle->ClearFlag();
		pAngle->CalPosition();
	}
	if( (propKeyStr==NULL||strcmp(propKeyStr,"oddStart")==0)&&(IsSyncMirPropWith(pDestObj,"oddStart")||
		(sPartNo.EqualNoCase(pAngle->GetPartNo())&&IsSyncMirPropWith(pDestObj,"oddStartOnSameLabel")&&
		(LABEL_PROP::ROD_ODDMENT_START&dwPropFilterFlag)>0)))
	{	//ʼ������ͷ
		pAngle->m_blOddByBoltCanAcrossMiddleSect=m_blOddByBoltCanAcrossMiddleSect;
		DESIGNODDMENT* pDesOdd=&pAngle->desStartOdd;
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)pAngle)->m_bUpdateSonAngleOddment=((CLDSGroupLineAngle*)this)->m_bUpdateSonAngleOddment;
		bool modified=false;
		if(!reverseStartEnd)
		{
			modified=fabs(pAngle->startOdd()-startOdd())>0.5;
			pAngle->SetStartOdd(startOdd());
		}
		else
		{
			modified=fabs(pAngle->endOdd()-startOdd())>0.5;
			pAngle->SetEndOdd(startOdd());
			pDesOdd=&pAngle->desEndOdd;
		}
		if(modified)
		{
			AppendModifiedAngles(pAngle);
			pAngle->SetModified(modified);
		}
		pDesOdd->m_fCollideDist=desStartOdd.m_fCollideDist;
		pDesOdd->m_iOddCalStyle=desStartOdd.m_iOddCalStyle;
		f3dPoint pick=Start();
		if(desStartOdd.m_hRefPart1>0x20)
			MirPartHandle(BelongModel(),&pDesOdd->m_hRefPart1,desStartOdd.m_hRefPart1,mirmsg,&pick);
		if(desStartOdd.m_hRefPart2>0x20)
			MirPartHandle(BelongModel(),&pDesOdd->m_hRefPart2,desStartOdd.m_hRefPart2,mirmsg,&pick);
	}
	if( (propKeyStr==NULL||strcmp(propKeyStr,"oddEnd")==0)&&(IsSyncMirPropWith(pDestObj,"oddEnd")||
		(sPartNo.EqualNoCase(pAngle->GetPartNo())&&IsSyncMirPropWith(pDestObj,"oddEndOnSameLabel")&&
		(LABEL_PROP::ROD_ODDMENT_END&dwPropFilterFlag)>0)))
	{	//�ն�����ͷ
		pAngle->m_blOddByBoltCanAcrossMiddleSect=m_blOddByBoltCanAcrossMiddleSect;
		DESIGNODDMENT* pDesOdd=&pAngle->desEndOdd;
		if(pAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			((CLDSGroupLineAngle*)pAngle)->m_bUpdateSonAngleOddment=((CLDSGroupLineAngle*)this)->m_bUpdateSonAngleOddment;
		bool modified=false;
		if(!reverseStartEnd)
		{
			modified=fabs(pAngle->endOdd()-endOdd())>0.5;
			pAngle->SetEndOdd(endOdd());
		}
		else
		{
			modified=fabs(pAngle->startOdd()-endOdd())>0.5;
			pAngle->SetStartOdd(endOdd());
			pDesOdd=&pAngle->desStartOdd;
		}
		if(modified)
		{
			AppendModifiedAngles(pAngle);
			pAngle->SetModified(modified);
		}
		pDesOdd->m_fCollideDist=desEndOdd.m_fCollideDist;
		pDesOdd->m_iOddCalStyle=desEndOdd.m_iOddCalStyle;
		f3dPoint pick=End();
		if(desStartOdd.m_hRefPart1>0x20)
			MirPartHandle(BelongModel(),&pDesOdd->m_hRefPart1,desEndOdd.m_hRefPart1,mirmsg,&pick);
		if(desStartOdd.m_hRefPart2>0x20)
			MirPartHandle(BelongModel(),&pDesOdd->m_hRefPart2,desEndOdd.m_hRefPart2,mirmsg,&pick);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"linkInfo")==0)&&IsSyncMirPropWith(pDestObj,"linkInfo"))
	{	//������˨��Ϣ
		if(!reverseStartEnd)
		{
			pAngle->connectStart=connectStart;
			pAngle->connectEnd=connectEnd;
		}
		else
		{
			pAngle->connectStart=connectEnd;
			pAngle->connectEnd=connectStart;
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"kaihe_base_wing_x0_y1")==0)&&IsSyncMirPropWith(pDestObj,"kaihe_base_wing_x0_y1"))
	{	//����֫
		pAngle->kaihe_base_wing_x0_y1=kaihe_base_wing_x0_y1;
		if(reverseWingXnY && kaihe_base_wing_x0_y1<=1)	//X��Y֫���Ͻ�
			pAngle->kaihe_base_wing_x0_y1=1-kaihe_base_wing_x0_y1;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"m_bEnableTeG")==0)&&IsSyncMirPropWith(pDestObj,"m_bEnableTeG"))
	{	//����׼��
		pAngle->m_bEnableTeG=m_bEnableTeG;
		pAngle->xWingXZhunJu=xWingXZhunJu;
		pAngle->xWingYZhunJu=xWingYZhunJu;
		if(m_bEnableTeG && reverseWingXnY)
		{
			pAngle->xWingXZhunJu=xWingYZhunJu;
			pAngle->xWingYZhunJu=xWingXZhunJu;
		}
	}
	if( propKeyStr!=NULL&&
		strcmp(propKeyStr,"oddStartOnSameLabel")==0&&IsSyncMirPropWith(pDestObj,"oddStartOnSameLabel")&&
		sPartNo.EqualNoCase(pAngle->Label)&&(LABEL_PROP::ANGLE_CUT_START&dwPropFilterFlag)>0)
	{
		bool modified=false;
		if(reverseStartEnd)
		{	//ʼĩ�˵ߵ�
			modified|=fabs(pAngle->cut_angle[2][0]-cut_angle[0][0])>0.5;
			modified|=fabs(pAngle->cut_angle[2][1]-cut_angle[0][1])>0.5;
			modified|=fabs(pAngle->cut_angle[3][0]-cut_angle[1][0])>0.5;
			modified|=fabs(pAngle->cut_angle[3][1]-cut_angle[1][1])>0.5;
			modified|=(pAngle->cut_wing[1]!=cut_wing[0]);
			modified|=fabs(pAngle->cut_wing_para[1][0]-cut_wing_para[0][0])>0.5;
			modified|=fabs(pAngle->cut_wing_para[1][1]-cut_wing_para[0][1])>0.5;
			modified|=fabs(pAngle->cut_wing_para[1][2]-cut_wing_para[0][2])>0.5;

			pAngle->m_bUserSpecEndCutAngle=m_bUserSpecStartCutAngle;
			memcpy(&pAngle->cut_angle[2][0],&cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],&cut_angle[1][0],sizeof(double)*2);
			pAngle->cut_wing[1]=cut_wing[0];
			memcpy(&pAngle->cut_wing_para[1][0],&cut_wing_para[0][0],sizeof(double)*3);
		}
		else
		{
			modified|=fabs(pAngle->cut_angle[0][0]-cut_angle[0][0])>0.5;
			modified|=fabs(pAngle->cut_angle[0][1]-cut_angle[0][1])>0.5;
			modified|=fabs(pAngle->cut_angle[1][0]-cut_angle[1][0])>0.5;
			modified|=fabs(pAngle->cut_angle[1][1]-cut_angle[1][1])>0.5;
			modified|=(pAngle->cut_wing[0]!=cut_wing[0]);
			modified|=fabs(pAngle->cut_wing_para[0][0]-cut_wing_para[0][0])>0.5;
			modified|=fabs(pAngle->cut_wing_para[0][1]-cut_wing_para[0][1])>0.5;
			modified|=fabs(pAngle->cut_wing_para[0][2]-cut_wing_para[0][2])>0.5;

			pAngle->m_bUserSpecStartCutAngle=m_bUserSpecStartCutAngle;
			memcpy(&pAngle->cut_angle[0][0],&cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],&cut_angle[1][0],sizeof(double)*2);
			pAngle->cut_wing[0]=cut_wing[0];
			memcpy(&pAngle->cut_wing_para[0][0],&cut_wing_para[0][0],sizeof(double)*3);
		}
		if(reverseWingXnY)
		{	//X��Y֫�ߵ�
			double tmparr[3];
			memcpy(tmparr,&pAngle->cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[0][0],&pAngle->cut_angle[1][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],tmparr,sizeof(double)*2);

			if(pAngle->cut_wing[0]==1)
				pAngle->cut_wing[0]=2;
			else if(pAngle->cut_wing[0]==2)
				pAngle->cut_wing[0]=1;
		}
		if(modified)
		{
			pAngle->SetModified(modified);
			AppendModifiedAngles(pAngle);
		}
	}
	if( propKeyStr!=NULL&&
		strcmp(propKeyStr,"oddEndOnSameLabel")==0&&IsSyncMirPropWith(pDestObj,"oddEndOnSameLabel")&&
		sPartNo.EqualNoCase(pAngle->Label)&&(LABEL_PROP::ANGLE_CUT_END&dwPropFilterFlag)>0)
	{
		bool modified=false;
		if(reverseStartEnd)
		{	//ʼĩ�˵ߵ�
			modified|=fabs(pAngle->cut_angle[0][0]-cut_angle[2][0])>0.5;
			modified|=fabs(pAngle->cut_angle[0][1]-cut_angle[2][1])>0.5;
			modified|=fabs(pAngle->cut_angle[1][0]-cut_angle[3][0])>0.5;
			modified|=fabs(pAngle->cut_angle[1][1]-cut_angle[3][1])>0.5;
			modified|=(pAngle->cut_wing[0]!=cut_wing[1]);
			modified|=fabs(pAngle->cut_wing_para[0][0]-cut_wing_para[1][0])>0.5;
			modified|=fabs(pAngle->cut_wing_para[0][1]-cut_wing_para[1][1])>0.5;
			modified|=fabs(pAngle->cut_wing_para[0][2]-cut_wing_para[1][2])>0.5;

			pAngle->m_bUserSpecStartCutAngle=m_bUserSpecEndCutAngle;
			memcpy(&pAngle->cut_angle[0][0],&cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],&cut_angle[3][0],sizeof(double)*2);
			pAngle->cut_wing[0]=cut_wing[1];
			memcpy(&pAngle->cut_wing_para[0][0],&cut_wing_para[1][0],sizeof(double)*3);
		}
		else
		{
			modified|=fabs(pAngle->cut_angle[2][0]-cut_angle[2][0])>0.5;
			modified|=fabs(pAngle->cut_angle[2][1]-cut_angle[2][1])>0.5;
			modified|=fabs(pAngle->cut_angle[3][0]-cut_angle[3][0])>0.5;
			modified|=fabs(pAngle->cut_angle[3][1]-cut_angle[3][1])>0.5;
			modified|=(pAngle->cut_wing[1]!=cut_wing[1]);
			modified|=fabs(pAngle->cut_wing_para[1][0]-cut_wing_para[1][0])>0.5;
			modified|=fabs(pAngle->cut_wing_para[1][1]-cut_wing_para[1][1])>0.5;
			modified|=fabs(pAngle->cut_wing_para[1][2]-cut_wing_para[1][2])>0.5;

			pAngle->m_bUserSpecEndCutAngle=m_bUserSpecEndCutAngle;
			memcpy(&pAngle->cut_angle[2][0],&cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],&cut_angle[3][0],sizeof(double)*2);
			pAngle->cut_wing[1]=cut_wing[1];
			memcpy(&pAngle->cut_wing_para[1][0],&cut_wing_para[1][0],sizeof(double)*3);
		}
		if(reverseWingXnY)
		{	//X��Y֫�ߵ�
			double tmparr[3];
			memcpy(tmparr,&pAngle->cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[2][0],&pAngle->cut_angle[3][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],tmparr,sizeof(double)*2);

			if(pAngle->cut_wing[1]==1)
				pAngle->cut_wing[1]=2;
			else if(pAngle->cut_wing[1]==2)
				pAngle->cut_wing[1]=1;
		}
		if(modified)
		{
			pAngle->SetModified(modified);
			AppendModifiedAngles(pAngle);
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cutAngleWing")==0)&&IsSyncMirPropWith(pDestObj,"cutAngleWing"))
	{	//ʼ���н���֫
		memcpy(pAngle->cut_angle,cut_angle,sizeof(double)*8);
		memcpy(pAngle->cut_wing,cut_wing,sizeof(long)*2);
		memcpy(pAngle->cut_wing_para,cut_wing_para,sizeof(double)*6);
		pAngle->m_bUserSpecStartCutAngle=m_bUserSpecStartCutAngle;
		pAngle->m_bUserSpecEndCutAngle=m_bUserSpecEndCutAngle;
		if(reverseStartEnd)
		{	//ʼĩ�˵ߵ�
			pAngle->m_bUserSpecStartCutAngle=m_bUserSpecEndCutAngle;
			pAngle->m_bUserSpecEndCutAngle=m_bUserSpecStartCutAngle;
			memcpy(&pAngle->cut_angle[0][0],&cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],&cut_angle[3][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[2][0],&cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],&cut_angle[1][0],sizeof(double)*2);
			pAngle->cut_wing[0]=cut_wing[1];
			pAngle->cut_wing[1]=cut_wing[0];
			memcpy(&pAngle->cut_wing_para[0][0],&cut_wing_para[1][0],sizeof(double)*3);
			memcpy(&pAngle->cut_wing_para[1][0],&cut_wing_para[0][0],sizeof(double)*3);
		}
		if(reverseWingXnY)
		{	//X��Y֫�ߵ�
			double tmparr[3];
			memcpy(tmparr,&pAngle->cut_angle[0][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[0][0],&pAngle->cut_angle[1][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[1][0],tmparr,sizeof(double)*2);
			memcpy(tmparr,&pAngle->cut_angle[2][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[2][0],&pAngle->cut_angle[3][0],sizeof(double)*2);
			memcpy(&pAngle->cut_angle[3][0],tmparr,sizeof(double)*2);

			if(pAngle->cut_wing[0]==1)
				pAngle->cut_wing[0]=2;
			else if(pAngle->cut_wing[0]==2)
				pAngle->cut_wing[0]=1;
			if(pAngle->cut_wing[1]==1)
				pAngle->cut_wing[1]=2;
			else if(pAngle->cut_wing[1]==2)
				pAngle->cut_wing[1]=1;
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"push_flat")==0)&&IsSyncMirPropWith(pDestObj,"push_flat"))
	{	//ѹ����Ϣ
		pAngle->start_push_flat_length=start_push_flat_length;
		pAngle->end_push_flat_length=end_push_flat_length;
		pAngle->start_push_flat_x1_y2=start_push_flat_x1_y2;
		pAngle->end_push_flat_x1_y2=end_push_flat_x1_y2;
		//�м�ѹ��
		pAngle->mid_push_flat_start_offset=mid_push_flat_start_offset;
		pAngle->mid_push_flat_end_offset=mid_push_flat_end_offset;
		pAngle->mid_push_flat_x1_y2=mid_push_flat_x1_y2;
		pAngle->mid_push_flat_node_handle=mid_push_flat_node_handle;
		if(mid_push_flat_node_handle>0X20)
			MirPartHandle(BelongModel(),&pAngle->mid_push_flat_node_handle,mid_push_flat_node_handle,mirmsg);
		if(reverseStartEnd)
		{	//ʼĩ�˵ߵ�
			pAngle->start_push_flat_length=end_push_flat_length;
			pAngle->end_push_flat_length=start_push_flat_length;
			pAngle->start_push_flat_x1_y2=end_push_flat_x1_y2;
			pAngle->end_push_flat_x1_y2=start_push_flat_x1_y2;
			pAngle->mid_push_flat_start_offset=mid_push_flat_end_offset;
			pAngle->mid_push_flat_end_offset=mid_push_flat_start_offset;
		}
		if(reverseWingXnY)
		{	//X��Y֫�ߵ�
			if(pAngle->start_push_flat_x1_y2==1)
				pAngle->start_push_flat_x1_y2=2;
			else if(pAngle->start_push_flat_x1_y2==2)
				pAngle->start_push_flat_x1_y2=1;
			if(pAngle->end_push_flat_x1_y2==1)
				pAngle->end_push_flat_x1_y2=2;
			else if(pAngle->end_push_flat_x1_y2==2)
				pAngle->end_push_flat_x1_y2=1;
			if(pAngle->mid_push_flat_x1_y2==1)
				pAngle->mid_push_flat_x1_y2=2;
			else if(pAngle->mid_push_flat_x1_y2==2)
				pAngle->mid_push_flat_x1_y2=1;
		}
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"manuProcess")==0)&&IsSyncMirPropWith(pDestObj,"manuProcess"))
	{	//���๤����Ϣ","��������������ӵȹ���
		pAngle->m_blScatterBoltLocaTolerance=m_blScatterBoltLocaTolerance;
		pAngle->m_bCutRoot=m_bCutRoot;
		pAngle->m_bCutBer=m_bCutBer;
		pAngle->m_bWeldPart=m_bWeldPart;
	}
	pDestObj->SetModified();
	return true;
}
bool CLDSLineAngle::SyncMirProp(const char* propKeyStr/*=NULL*/,BYTE ciMirAll0X1Y2Z3/*=0*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr��ʾͬ���Գ�ȫ������
{
	MIRMSG xSpecMirInfo(ciMirAll0X1Y2Z3);
	for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		if (pRelaObj->hObj==handle)
			continue;	//�Լ����Լ����д���ͬ���������׵��½Ǹ�֫������ wjh-2019.12.2
		if(ciMirAll0X1Y2Z3!=0&&!pRelaObj->mirInfo.IsSameMirMsg(xSpecMirInfo))
			continue;
		BOOL pushed=relativeObjs.push_stack();
		CLDSPart* pAngle=BelongModel()->FromPartHandle(pRelaObj->hObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		SyncMirPropTo(pAngle,pRelaObj->mirInfo,propKeyStr,dwPropFilterFlag);
		relativeObjs.pop_stack(pushed);
	}
	return true;
}
void CLDSLineAngle::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	BelongModel()->ValidateObjRefH(&des_norm_x.hViceJg);
	BelongModel()->ValidateObjRefH(&des_norm_y.hViceJg);
	BelongModel()->ValidateObjRefH(&desStartPos.datum_jg_h);
	BelongModel()->ValidateObjRefH(&desEndPos.datum_jg_h);
	BelongModel()->ValidateObjRefH(&desStartOdd.m_hRefPart1);
	BelongModel()->ValidateObjRefH(&desStartOdd.m_hRefPart2);
	BelongModel()->ValidateObjRefH(&desEndOdd.m_hRefPart1);
	BelongModel()->ValidateObjRefH(&desEndOdd.m_hRefPart2);
	BelongModel()->ValidateObjRefH(&group_father_jg_h);
	desStartPos.CleanInvalidObjRef(BelongModel());
	desEndPos.CleanInvalidObjRef(BelongModel());
	if(m_nClassTypeId==CLS_GROUPLINEANGLE)
	{
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)this;
		for(int i=0;i<4;i++)
			BelongModel()->ValidateObjRefH(&pGroupAngle->son_jg_h[i]);
	}
}
void CLDSLineAngle::ResetRelativeObjs()
{
	for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		CLDSLineAngle *pMirAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pRelaObj->hObj,CLS_LINEANGLE);
		if(pMirAngle==NULL)
		{	//ɾ�������õĹ�������
			relativeObjs.DeleteCursor();
			continue;
		}
		if(pMirAngle==this)
			continue;
	}
}
void CLDSLineAngle::UpdateRelateRayJgPos()
{
	BelongModel()->PushPartStack();
	for(CLDSLinePart *pLinePart=BelongModel()->EnumRodFirst();pLinePart;pLinePart=BelongModel()->EnumRodNext())
	{
		if(pLinePart->IsAngle())
		{
			CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pLinePart;
			if( pLineAngle->desStartPos.datum_jg_h==handle ||
				(pLineAngle->desStartPos.datum_jg_h==0&&pLineAngle->pStart&&pLineAngle->pStart->hFatherPart==handle))
			{
				if(pLineAngle->desStartPos.spatialOperationStyle==2)
					pLineAngle->desStartPos.spatialOperationStyle=3;
				else if(pLineAngle->desStartPos.spatialOperationStyle==3)
					pLineAngle->desStartPos.spatialOperationStyle=2;
				else if(pLineAngle->desStartPos.spatialOperationStyle==5)
					pLineAngle->desStartPos.spatialOperationStyle=6;
				else if(pLineAngle->desStartPos.spatialOperationStyle==6)
					pLineAngle->desStartPos.spatialOperationStyle=5;
				else if(pLineAngle->desStartPos.spatialOperationStyle==8)
					pLineAngle->desStartPos.spatialOperationStyle=9;
				else if(pLineAngle->desStartPos.spatialOperationStyle==9)
					pLineAngle->desStartPos.spatialOperationStyle=8;
				else if(pLineAngle->desStartPos.spatialOperationStyle==11)
					pLineAngle->desStartPos.spatialOperationStyle=12;
				else if(pLineAngle->desStartPos.spatialOperationStyle==12)
					pLineAngle->desStartPos.spatialOperationStyle=11;
				else if(pLineAngle->desStartPos.spatialOperationStyle==13)
					pLineAngle->desStartPos.spatialOperationStyle=14;
				else if(pLineAngle->desStartPos.spatialOperationStyle==14)
					pLineAngle->desStartPos.spatialOperationStyle=13;
				pLineAngle->UnlockStart();
				pLineAngle->UnlockEnd();
				pLineAngle->CalPosition();
			}
			else if( pLineAngle->desEndPos.datum_jg_h==handle ||
				(pLineAngle->desEndPos.datum_jg_h==0&&pLineAngle->pEnd&&pLineAngle->pEnd->hFatherPart==handle))
			{
				if(pLineAngle->desEndPos.spatialOperationStyle==2)
					pLineAngle->desEndPos.spatialOperationStyle=3;
				else if(pLineAngle->desEndPos.spatialOperationStyle==3)
					pLineAngle->desEndPos.spatialOperationStyle=2;
				else if(pLineAngle->desEndPos.spatialOperationStyle==5)
					pLineAngle->desEndPos.spatialOperationStyle=6;
				else if(pLineAngle->desEndPos.spatialOperationStyle==6)
					pLineAngle->desEndPos.spatialOperationStyle=5;
				else if(pLineAngle->desEndPos.spatialOperationStyle==8)
					pLineAngle->desEndPos.spatialOperationStyle=9;
				else if(pLineAngle->desEndPos.spatialOperationStyle==9)
					pLineAngle->desEndPos.spatialOperationStyle=8;
				else if(pLineAngle->desEndPos.spatialOperationStyle==11)
					pLineAngle->desEndPos.spatialOperationStyle=12;
				else if(pLineAngle->desEndPos.spatialOperationStyle==12)
					pLineAngle->desEndPos.spatialOperationStyle=11;
				else if(pLineAngle->desEndPos.spatialOperationStyle==13)
					pLineAngle->desEndPos.spatialOperationStyle=14;
				else if(pLineAngle->desEndPos.spatialOperationStyle==14)
					pLineAngle->desEndPos.spatialOperationStyle=13;
				pLineAngle->UnlockStart();
				pLineAngle->UnlockEnd();
				pLineAngle->CalPosition();
			}
		}
		else if(pLinePart->IsTube()||pLinePart->GetClassTypeId()==CLS_LINESLOT||pLinePart->GetClassTypeId()==CLS_LINEFLAT)
		{
			CTubeEndPosPara *pDesStartPos=NULL,*pDesEndPos=NULL; 
			if(pLinePart->IsTube())
			{
				pDesStartPos = &((CLDSLineTube*)pLinePart)->desStartPos;
				pDesEndPos = &((CLDSLineTube*)pLinePart)->desEndPos;
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
			{
				pDesStartPos = &((CLDSLineSlot*)pLinePart)->desStartPos;
				pDesEndPos = &((CLDSLineSlot*)pLinePart)->desEndPos;
			}
			else //if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
			{
				pDesStartPos = &((CLDSLineFlat*)pLinePart)->desStartPos;
				pDesEndPos = &((CLDSLineFlat*)pLinePart)->desEndPos;
			}
			if( pDesStartPos->hDatumPart==handle ||
				(pDesStartPos->hDatumPart==0&&pLinePart->pStart&&pLinePart->pStart->hFatherPart==handle))
			{
				if(pDesStartPos->datum_pos_style==1)
					pDesStartPos->datum_pos_style=2;
				else if(pDesStartPos->datum_pos_style==2)
					pDesStartPos->datum_pos_style=1;
				else if(pDesStartPos->datum_pos_style==4)
					pDesStartPos->datum_pos_style=5;
				else if(pDesStartPos->datum_pos_style==5)
					pDesStartPos->datum_pos_style=4;
				pLinePart->UnlockStart();
				pLinePart->UnlockEnd();
			}
			else if( pDesEndPos->hDatumPart==handle ||
				(pDesEndPos->hDatumPart==0&&pLinePart->pStart&&pLinePart->pEnd->hFatherPart==handle))
			{
				if(pDesEndPos->datum_pos_style==1)
					pDesEndPos->datum_pos_style=2;
				else if(pDesEndPos->datum_pos_style==2)
					pDesEndPos->datum_pos_style=1;
				else if(pDesEndPos->datum_pos_style==4)
					pDesEndPos->datum_pos_style=5;
				else if(pDesEndPos->datum_pos_style==5)
					pDesEndPos->datum_pos_style=4;
				pLinePart->UnlockStart();
				pLinePart->UnlockEnd();
			}
		}
	}
	BelongModel()->PopPartStack();
}

//���¸˼�������Ϣ wht 09-12-24
void CLDSLineAngle::UpdateConnectInfo(CLDSNode *pNode)
{
	if(pNode==NULL||pStart==NULL||pEnd==NULL)
		return;
	long hFatherPart;
	CConnectInfo *pConnectInfo;
	CMultiOffsetPos *pDesPosPara;
	if(pNode->handle==pStart->handle)
	{	//ʼ������
		hFatherPart=pStart->hFatherPart;
		pConnectInfo=&connectStart;
		pDesPosPara=&desStartPos;
	}
	else if(pNode->handle==pEnd->handle)
	{	//�ն�����
		hFatherPart=pEnd->hFatherPart;
		pConnectInfo=&connectEnd;
		pDesPosPara=&desEndPos;
	}
	else 
		return;
	if(pDesPosPara->datum_jg_h>0)	//ָ����ӻ�׼�Ǹ�
		hFatherPart=pDesPosPara->datum_jg_h;
	
	if(hFatherPart==handle)	//�˽ڵ㸸�˼����ǵ�ǰ�˼���������Ϊ��Ҫ��˫֫����
		pConnectInfo->m_iConnectWing=0;
	else if(pDesPosPara->jgber_cal_style!=1 || pDesPosPara->spatialOperationStyle==0||
		pDesPosPara->spatialOperationStyle>=15||//�Ǵ�Ӷ�λ
		pDesPosPara->IsInDatumJgBer()&&toupper(layer(1))=='Z')	//���Ĵ�����һ������
		pConnectInfo->m_iConnectWing=0;
	else
	{
		f3dPoint datum_norm;
		CSuperSmartPtr<CLDSLinePart> pDatumLinePart;
		pDatumLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(hFatherPart);
		if(pDatumLinePart.IsNULL()||!pDatumLinePart->IsLinePart())
			return;
		if(pDatumLinePart->GetClassTypeId()==CLS_LINEANGLE||pDatumLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if(pDesPosPara->IsInDatumJgWingX())
				datum_norm=pDatumLinePart.LineAnglePointer()->get_norm_x_wing();
			else if(pDesPosPara->IsInDatumJgWingY())
				datum_norm=pDatumLinePart.LineAnglePointer()->get_norm_y_wing();
		}
		else
		{
			f3dPoint datum_vec,line_vec;
			datum_vec=pDatumLinePart->End()-pDatumLinePart->Start();
			line_vec=End()-Start();
			datum_norm=datum_vec^line_vec;
			normalize(datum_norm);
		}
		if( fabs(datum_norm*get_norm_x_wing())>fabs(datum_norm*get_norm_y_wing()))
			pConnectInfo->m_iConnectWing=1;	//X֫����
		else
			pConnectInfo->m_iConnectWing=2;	//Y֫����
	}
}
BOOL CLDSLineAngle::IsUnequalAngle()
{
	return IsUnequalAngle(size_wide,size_height,size_thick);
}
BOOL CLDSLineAngle::IsUnequalAngle(double x_wing_wide,double y_wing_wide,double thick)
{	//Y֫��ȴ���֫���Ҳ�����X֫���
	if(y_wing_wide>=thick&&fabs(x_wing_wide-y_wing_wide)>0)
		return TRUE;
	return FALSE;
}
BYTE CLDSLineAngle::FuncType()
{
	//��Ϊ�Զ�����
	if(m_cFuncType>0)
		return m_cFuncType;
	//�����ж�
	if(pStart==NULL||pEnd==NULL)
		m_cFuncType=CLDSPart::FUNC_REINFORCE;
	else if(toupper(Layer(1))=='Z')
	{
		char cFirst=toupper(Layer(0));
		if(cFirst=='L')	//�Ȳ����ĽǸ�
			m_cFuncType=CLDSPart::FUNC_LEGANGLE;
		else if(cFirst=='S')
			m_cFuncType=CLDSPart::FUNC_BODYANGLE;
		else if(cFirst=='T')
			m_cFuncType=CLDSPart::FUNC_HEADANGLE;
	}
	return m_cFuncType;
}
//��Դ�Ǹ���Ϣ���Ƶ�Ŀ��Ǹ���
void CopyLineAngle(CLDSLineAngle *pDestLineAngle,CLDSLineAngle *pSrcLineAngle,BOOL bReverseStartEnd)
{
	if(pSrcLineAngle==NULL||pDestLineAngle==NULL)
		return;
	int i=0,j=0;
	pDestLineAngle->SetBelongModel(pSrcLineAngle->BelongModel());
	pDestLineAngle->EmptyLsRef();
	if(bReverseStartEnd)
	{
		if(pSrcLineAngle->huoqu_base_wing_x0_y1_both2==0)
			pDestLineAngle->huoqu_base_wing_x0_y1_both2 = 1;
		else if(pSrcLineAngle->huoqu_base_wing_x0_y1_both2==1)
			pDestLineAngle->huoqu_base_wing_x0_y1_both2 = 0;
		else
			pDestLineAngle->huoqu_base_wing_x0_y1_both2 = 2;
		if(pSrcLineAngle->kaihe_base_wing_x0_y1==0)
			pDestLineAngle->kaihe_base_wing_x0_y1 = 1;
		else if(pSrcLineAngle->kaihe_base_wing_x0_y1==1)
			pDestLineAngle->kaihe_base_wing_x0_y1 = 0;
		else
			pDestLineAngle->kaihe_base_wing_x0_y1 = 2;
		pDestLineAngle->SetStart(pSrcLineAngle->End());
		pDestLineAngle->SetEnd(pSrcLineAngle->Start());
		pDestLineAngle->SetStartOdd(pSrcLineAngle->endOdd());
		pDestLineAngle->SetEndOdd(pSrcLineAngle->startOdd());
		pDestLineAngle->set_norm_x_wing(pSrcLineAngle->get_norm_y_wing());
		pDestLineAngle->set_norm_y_wing(pSrcLineAngle->get_norm_x_wing());
		for(CLsRef *pLsRef=pSrcLineAngle->GetFirstLsRef();pLsRef;pLsRef=pSrcLineAngle->GetNextLsRef())
			pDestLineAngle->AppendMidLsRef(*pLsRef);
		pDestLineAngle->set_wing_wide(pSrcLineAngle->GetWidth());
		pDestLineAngle->set_wing_thick(pSrcLineAngle->GetThick());
		pDestLineAngle->pStart = pSrcLineAngle->pEnd;
		pDestLineAngle->pEnd = pSrcLineAngle->pStart;
		pDestLineAngle->cMaterial = pSrcLineAngle->cMaterial;
		pDestLineAngle->iSeg = pSrcLineAngle->iSeg;
		pDestLineAngle->SetPartNo(pSrcLineAngle->GetPartNo());
		pDestLineAngle->handle = pSrcLineAngle->handle;
		pDestLineAngle->huoqu_handle = pSrcLineAngle->huoqu_handle;
		pDestLineAngle->huoqu_r = pSrcLineAngle->huoqu_r;
		pDestLineAngle->m_bCutRoot = pSrcLineAngle->m_bCutRoot;
		pDestLineAngle->m_bCutBer = pSrcLineAngle->m_bCutBer;
		pDestLineAngle->m_bWeldPart = pSrcLineAngle->m_bWeldPart;
		pDestLineAngle->cfgword = pSrcLineAngle->cfgword;
		pDestLineAngle->SetLayer(pSrcLineAngle->layer());
		//��֫�н�
		pDestLineAngle->cut_wing[0] = (3-pSrcLineAngle->cut_wing[1])%3;
		pDestLineAngle->cut_wing[1] = (3-pSrcLineAngle->cut_wing[0])%3;
		/*cut_wing[0]��ʾʼ���н�cut_wing[1]��ʾ�ն��н�
		...[][0]��ʾ�Ǹ�������ȡ����
		...[][1]��ʾ��һ֫��ȡ���
		...[][2]��ʾ֫������ȡ����
		*/
		pDestLineAngle->cut_wing_para[0][0] = pSrcLineAngle->cut_wing_para[1][0];
		pDestLineAngle->cut_wing_para[0][1] = pSrcLineAngle->cut_wing_para[1][1];
		pDestLineAngle->cut_wing_para[0][2] = pSrcLineAngle->cut_wing_para[1][2];
		pDestLineAngle->cut_wing_para[1][0] = pSrcLineAngle->cut_wing_para[0][0];
		pDestLineAngle->cut_wing_para[1][1] = pSrcLineAngle->cut_wing_para[0][1];
		pDestLineAngle->cut_wing_para[1][2] = pSrcLineAngle->cut_wing_para[0][2];
		
		for(j=0;j<4;j++)
		{
			pDestLineAngle->cut_angle[j][0] = pSrcLineAngle->cut_angle[3-j][0];
			pDestLineAngle->cut_angle[j][1] = pSrcLineAngle->cut_angle[3-j][1];
		}
	}
	else
	{
		pDestLineAngle->huoqu_base_wing_x0_y1_both2=pSrcLineAngle->huoqu_base_wing_x0_y1_both2;
		pDestLineAngle->SetStart(pSrcLineAngle->Start());
		pDestLineAngle->SetEnd(pSrcLineAngle->End());
		pDestLineAngle->SetStartOdd(pSrcLineAngle->startOdd());
		pDestLineAngle->SetEndOdd(pSrcLineAngle->endOdd());
		pDestLineAngle->set_norm_x_wing(pSrcLineAngle->get_norm_x_wing());
		pDestLineAngle->set_norm_y_wing(pSrcLineAngle->get_norm_y_wing());
		for(CLsRef *pLsRef=pSrcLineAngle->GetFirstLsRef();pLsRef;pLsRef=pSrcLineAngle->GetNextLsRef())
			pDestLineAngle->AppendMidLsRef(*pLsRef);
		pDestLineAngle->set_wing_wide(pSrcLineAngle->GetWidth());
		pDestLineAngle->set_wing_thick(pSrcLineAngle->GetThick());
		pDestLineAngle->pStart = pSrcLineAngle->pStart;
		pDestLineAngle->pEnd = pSrcLineAngle->pEnd;
		pDestLineAngle->cMaterial = pSrcLineAngle->cMaterial;
		pDestLineAngle->iSeg = pSrcLineAngle->iSeg;
		pDestLineAngle->SetPartNo(pSrcLineAngle->GetPartNo());
		pDestLineAngle->handle = pSrcLineAngle->handle;
		pDestLineAngle->huoqu_handle = pSrcLineAngle->huoqu_handle;
		pDestLineAngle->huoqu_r = pSrcLineAngle->huoqu_r;
		pDestLineAngle->m_bCutRoot = pSrcLineAngle->m_bCutRoot;
		pDestLineAngle->m_bCutBer = pSrcLineAngle->m_bCutBer;
		pDestLineAngle->m_bWeldPart = pSrcLineAngle->m_bWeldPart;
		pDestLineAngle->cfgword = pSrcLineAngle->cfgword;
		pDestLineAngle->SetLayer(pSrcLineAngle->layer());
		//��֫�н�
		pDestLineAngle->cut_wing[0] = pSrcLineAngle->cut_wing[0];
		pDestLineAngle->cut_wing[1] = pSrcLineAngle->cut_wing[1];
		/*cut_wing[0]��ʾʼ���н�cut_wing[1]��ʾ�ն��н�
		...[][0]��ʾ�Ǹ�������ȡ����
		...[][1]��ʾ��һ֫��ȡ���
		...[][2]��ʾ֫������ȡ����
		*/
		for(j=0;j<3;j++)
		{
			pDestLineAngle->cut_wing_para[0][j] = pSrcLineAngle->cut_wing_para[0][j];
			pDestLineAngle->cut_wing_para[1][j] = pSrcLineAngle->cut_wing_para[1][j];
		}
		//��ʶ���нǸ�֫,0��ʾ����֫,1��ʾX֫,2��ʾY֫
		// --------- 1.3. �н� -----------
		/*
		cut_angle[0]��ʾ���X֫
		cut_angle[1]��ʾ���Y֫
		cut_angle[2]��ʾ�յ�X֫
		cut_angle[3]��ʾ�յ�Y֫
		...[][0]��ʾƽ�����߷�����ȡ��  ...[][1]��ʾ��ֱ���߷�����ȡ��
		*/
		for(j=0;j<4;j++)
		{
			pDestLineAngle->cut_angle[j][0] = pSrcLineAngle->cut_angle[j][0];
			pDestLineAngle->cut_angle[j][1] = pSrcLineAngle->cut_angle[j][1];
		}
	}
}
#ifndef __NO_SUPPORT_PROCESS_INFO_
BOOL CalKaiHeStartEndPos(CLDSLineAngle *pJg,CLDSNode *pNode,f3dPoint &startPos
						,f3dPoint &endPos,UCS_STRU &angleUcs,BOOL bIntelligent/*=FALSE*/
						,BOOL bUcsIncStartOdd/*=FALSE*/);
int CompareKaiHeJiao(const KAI_HE_JIAO_TEMP& item1,const KAI_HE_JIAO_TEMP& item2)
{
	if(item1.StartPosLength()>item2.StartPosLength())
		return 1;
	else if(item1.StartPosLength()<item2.StartPosLength())
		return -1;
	else
		return 0;
}
struct RIB_PLATE_INFO_TEMP
{
	float fThick;            //�߰���
	float fPosZ;			 //�߰�λ��
	CXhChar16 sPlateNo;		 //�߰���
	char m_cMaterial;		 //����
};
//����Ǹֿ��Ͻ�
bool ProcessKaiHeAngle(CLDSLineAngle *pJg,UCS_STRU &angleUcs,ATOM_LIST<KAI_HE_JIAO_TEMP> &KaiHeJiaoArr)
{
	if(pJg->pStart==NULL||pJg->pEnd==NULL)
		return FALSE;
	if((pJg->layer(0)=='S'||pJg->layer(0)=='L')&&pJg->layer(1)=='Z'||pJg->GetKaiHeWingX0Y1()<0)
		return FALSE;	//�Ȳ���������һ�㲻���Ͻ�,��Ϊָ�������Ͻ�ʱҲ����Ҫ����
	if (CLDSPart::CustomerSerial == 6 || CLDSPart::CustomerSerial == 1)
	{	//�������������ڲ�������ʱ�ã�
		double MAX_KAIHE_CLEARANCE= CLDSPart::library!=NULL?CLDSPart::library->GetMaxKaiHeJiaoClearance():1;
		if(pJg->ConnectClearance()<MAX_KAIHE_CLEARANCE)
			return FALSE;	//���Ͻ�һ�㲻ͬʱ������֫������Ӧ����ȫ���ǶȲwjh-2017.12.14
	}
	//
	KAI_HE_JIAO_TEMP kai_he_jiao;
	kai_he_jiao.decWingAngle = (float)pJg->GetDecWingAngle();
	//����97��ͼ�涨���е��޸� WJH-2004/09/28 
	//ʹ�á����Ͻ���ֵ�������Ƿ���Ҫ��ע���Ͻ���ϢĬ��Ϊ2��wht 11-05-06
	double dfKaiHeThreshold = pJg->library != NULL ? pJg->library->GetJgKaiHeJiaoThreshold() : 2;
	if(fabs(kai_he_jiao.decWingAngle-90)< dfKaiHeThreshold)	//�н�ƫ�����2��Ҫ���Ͻ�
		return false;
	//1.ͳ�ƿ��Ͻ�����
	NODESET nodeset;
	CLDSNode *pNode=NULL;
	f3dLine line;
	line.startPt = pJg->pStart->Position(false);
	line.endPt = pJg->pEnd->Position(false);
	for(pNode=pJg->BelongModel()->EnumNodeFirst();pNode;pNode=pJg->BelongModel()->EnumNodeNext())
	{
		int ret=line.PtInLine(pNode->Position(false));
		if(ret==0)
			continue;
		else if((ret<0&&pNode->hFatherPart==pJg->handle)||ret>0)
			nodeset.append(pNode);	//λ�ڽǸ��ϵĽڵ�
	}
	double length=pJg->GetLength();
	f3dPoint dimEndPos,dimStartPos;
	ARRAY_LIST<KAI_HE_JIAO_TEMP> kaiHeJiaoScopeList;
	for(pNode=nodeset.GetFirst();pNode!=NULL;pNode=nodeset.GetNext())
	{
		if(pNode==pJg->pStart)
			dimEndPos = pJg->Start();
		else if(pNode==pJg->pEnd)
			dimEndPos = pJg->End();
		else
			dimEndPos = pJg->GetDatumPosBer(pNode);//->pos;
		coord_trans(dimEndPos,angleUcs,FALSE);
		dimStartPos = dimEndPos;
		//���ϽǱ�ע�м�ڵ�λ��(�������);
		//���������ϵangleUcs�����Ǹ�ʼ����ͷ��Ϣ(����ϵԭ��Ϊ�Ǹ�ʼ��ʵ�����)��bUcsIncStartOdd����Ӧ����ΪTRUE wht 15-04-23
		if(!CalKaiHeStartEndPos(pJg,pNode,dimStartPos,dimEndPos,angleUcs,FALSE,TRUE))
			continue;
		else if(fabs(dimStartPos.z-dimEndPos.z)<EPS2)
			continue;
		double dfKaiHeMinDeita = CLDSPart::library != NULL ? CLDSPart::library->GetJgKaiHeJiaoMinDeita() : 5;
		dimStartPos.z -= dfKaiHeMinDeita;
		dimEndPos.z += dfKaiHeMinDeita;
		if(dimStartPos.z<0)
			dimStartPos.z=0;
		if(dimEndPos.z>length)
			dimEndPos.z=length;
		dimStartPos.Set(dimStartPos.z);
		dimEndPos.Set(dimEndPos.z);
		if(pNode==pJg->pStart)
		{
			dimStartPos.Set(0,0);
			dimEndPos.Set(dimEndPos.x,0);
		}
		else if(pNode==pJg->pEnd)
		{
			dimStartPos.Set(dimStartPos.x,0);
			dimEndPos.Set(length,0);
		}
		else
		{
			dimStartPos.Set(dimStartPos.x,0);
			dimEndPos.Set(dimEndPos.x,0);
		}
		kai_he_jiao.SetStartPosLength((short)dimStartPos.x);
		kai_he_jiao.SetEndPosLength((short)dimEndPos.x);
		kaiHeJiaoScopeList.append(kai_he_jiao);
	}
	//2.������Ͻ��Ŀ��ϽǺϲ�Ϊһ�����Ͻ�
	int i,n=kaiHeJiaoScopeList.GetSize();
	//�����������򿪺ϽǱ�ע
	CHeapSort<KAI_HE_JIAO_TEMP>::HeapSort(kaiHeJiaoScopeList.m_pData,n,CompareKaiHeJiao);
	double MERGE_KAIHE_SPACE =pJg->GetWidth()*2;
	for(i=0;i<n-1;i++)
	{
		KAI_HE_JIAO_TEMP *pScope1=kaiHeJiaoScopeList.GetByIndex(i);
		KAI_HE_JIAO_TEMP *pScope2=kaiHeJiaoScopeList.GetByIndex(i+1);
		if(i==0&&pScope1->StartPosLength()<MERGE_KAIHE_SPACE)
			pScope1->SetStartPosLength(0);
		if(i==n-2&&length-pScope2->EndPosLength()<MERGE_KAIHE_SPACE)
			pScope2->SetEndPosLength((short)length);
		if(pScope1->EndPosLength()>pScope2->StartPosLength()-MERGE_KAIHE_SPACE)	//����Ͻ��ĺϲ�Ϊһ�ο��Ͻ�
		{
			pScope1->SetEndPosLength(pScope2->EndPosLength());
			kaiHeJiaoScopeList.RemoveAt(i+1);
			i--;
			n--;
		}
	}
	//3.���ؿ��Ͻ�����
	KaiHeJiaoArr.Empty();
	for(KAI_HE_JIAO_TEMP *pKaiHeJiao=kaiHeJiaoScopeList.GetFirst();pKaiHeJiao;pKaiHeJiao=kaiHeJiaoScopeList.GetNext())
	{
		pKaiHeJiao->SetStartPosLength(pKaiHeJiao->StartPosLength());
		pKaiHeJiao->SetEndPosLength(pKaiHeJiao->EndPosLength());
		KaiHeJiaoArr.append(*pKaiHeJiao);
	}
	return true;;
}
bool ProcessPolyPartKaiHeJiao(CLDSPolyPart *pPolyPart,ATOM_LIST<KAI_HE_JIAO_TEMP> &kaiHeJiaoList,double* fWeldLineLen=NULL)
{
	CArray<UCS_STRU,UCS_STRU&>angleUcsArr;
	CArray<CLDSLineAngle,CLDSLineAngle&>segArr;
	segArr.SetSize(pPolyPart->segset.GetNodeNum());
	angleUcsArr.SetSize(segArr.GetSize());
	CLDSLineAngle *pFirstSeg = (CLDSLineAngle*)pPolyPart->segset.GetFirst();
	CLDSLineAngle *pSecondSeg = (CLDSLineAngle*)pPolyPart->segset.GetNext();
	CLDSLineAngle *pTailSeg = (CLDSLineAngle*)pPolyPart->segset.GetTail();
	CLDSLineAngle* pSeg=NULL,*pPrevSeg =NULL;
	double realStartPosX=0;
	int i=0;
	//������β�νӵ���ʱ�Ǹֶ�����
	for(pSeg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pSeg;pSeg=(CLDSLineAngle*)pPolyPart->segset.GetNext())
	{
		BOOL bPrevSegNeedReverseStartEnd=FALSE,bCurSegNeedReverseStartEnd=FALSE;
		if(pPrevSeg==NULL)	//�׶λ����ӽǸ�
		{
			if( (pSeg->pStart==pSecondSeg->pStart&&DISTANCE(pSeg->Start(),pSecondSeg->Start())<2.0)||
				(pSeg->pStart==pSecondSeg->pEnd&&DISTANCE(pSeg->Start(),pSecondSeg->End())<2.0))
				bCurSegNeedReverseStartEnd=TRUE;
		}
		else
		{
			if(pPrevSeg->pStart==pSeg->pStart&&DISTANCE(pPrevSeg->Start(),pSeg->Start())<2.0)
			{
				bPrevSegNeedReverseStartEnd=TRUE;
				bCurSegNeedReverseStartEnd=FALSE;
			}
			else if( (pPrevSeg->pStart==pSeg->pEnd&&DISTANCE(pPrevSeg->Start(),pSeg->End())<2.0))
			{
				bPrevSegNeedReverseStartEnd=TRUE;
				bCurSegNeedReverseStartEnd=TRUE;
			}
			else if(pPrevSeg->pEnd==pSeg->pStart&&DISTANCE(pPrevSeg->End(),pSeg->Start())<2.0)
			{
				bPrevSegNeedReverseStartEnd=FALSE;
				bCurSegNeedReverseStartEnd=FALSE;
			}
			else if(pPrevSeg->pEnd==pSeg->pEnd&&DISTANCE(pPrevSeg->End(),pSeg->End())<2.0)
			{
				bPrevSegNeedReverseStartEnd=FALSE;
				bCurSegNeedReverseStartEnd=TRUE;
			}
		}
		//���������ǰһ�Ǹֶ�����ͷ ���ڼ���������δ���������ֵ
		if(pPrevSeg)
		{
			if(bPrevSegNeedReverseStartEnd)	//ǰһ�Ǹֶ�ʼ�����ӵ�ǰ�����
				pPrevSeg->SetStartOdd(0);
			else	//ǰһ�Ǹֶ��ն����ӵ�ǰ�����
				pPrevSeg->SetEndOdd(0);
			//����������һ�Ǹ�(��ǰ�Ǹֶ�)����ͷ ���ڼ���������δ���������ֵ
			if(bCurSegNeedReverseStartEnd)	//��һ�Ǹֶ��ն����ӵ�ǰ�����
				pSeg->SetEndOdd(0);
			else	//��һ�Ǹֶ�ʼ�����ӵ�ǰ�����
				pSeg->SetStartOdd(0);
		}
		CopyLineAngle(&segArr[i],pSeg,bCurSegNeedReverseStartEnd);
		//�Ե�ǰ�νǸ�Ϊ��׼������Ǹ�������ֱ��,����Ǹֵĵ��������ϵ
		segArr[i].getUCS(angleUcsArr[i],segArr[i].huoqu_base_wing_x0_y1_both2);
		if(pPrevSeg==NULL)
			angleUcsArr[i].origin-=angleUcsArr[i].axis_z*segArr[i].startOdd();	//��������ͷӰ��
		else
			angleUcsArr[i].origin-=angleUcsArr[i].axis_z*realStartPosX;
		pPrevSeg=pSeg;
		i++;
	}
	for(i=0;i<segArr.GetSize();i++)
		ProcessKaiHeAngle(&segArr[i],angleUcsArr[i],kaiHeJiaoList);			//�����Ͻ�
	//����Ǹֽ��и��ں�ʱ�������ൺ���ͨ������㷽ʽ���м���
	double ff=0;
	for(i=1;i<segArr.GetSize();i++)
	{
		double fAngleX=0,fAngleY=0,fHuoquAngle=0;
		fAngleX=ACOS(segArr[i-1].get_norm_x_wing()*segArr[i].get_norm_x_wing());
		fAngleY=ACOS(segArr[i-1].get_norm_y_wing()*segArr[i].get_norm_y_wing());
		if(segArr[i-1].huoqu_base_wing_x0_y1_both2==0)		//X֫����
			fHuoquAngle=fAngleX; 
		else if(segArr[i-1].huoqu_base_wing_x0_y1_both2==1)	//Y֫����
			fHuoquAngle=fAngleY; 
		else if(segArr[i-1].huoqu_base_wing_x0_y1_both2==2)	//˫֫����
		{	
			if(fabs(fAngleX-fAngleY)<0.01)
				continue;	//X֫��������Y֫������һ��ʱ�������и�ں�
			fHuoquAngle=fAngleX>fAngleY?fAngleX:fAngleY;
		}
		fHuoquAngle*=DEGTORAD_COEF;
		double fPlaneAngle=180-fHuoquAngle,fLen=0,fWingW=segArr[i].GetWidth();
		if((fWingW<=63 && fPlaneAngle<165)||
			(fWingW>=70&&fWingW<=80 && fPlaneAngle<172))
			fLen=5000;
		else if(fWingW>80&&fWingW<=125 && fPlaneAngle<175)
		{
			if(fWingW<100)
				fLen=5000;
			else
				fLen=7000;
		}
		else if(fWingW>125 && fPlaneAngle<177)
		{
			if(fWingW<180)
				fLen=7000;
			else
				fLen=10000;
		}
		if(fLen>0 && fWeldLineLen)
		{
			*fWeldLineLen=fLen;
			break;
		}
	}
	return true;
}
//����Ǹ��߰�
void UpdateRibPlateList(CLDSLineAngle *pLineAngle,UCS_STRU angleDrawUcs,ATOM_LIST<RIB_PLATE_INFO_TEMP> &rib_plate_list)
{
	//���Ǹ��߰嵱����˨��������
	f3dPoint angle_vec=pLineAngle->End()-pLineAngle->Start();
	normalize(angle_vec);
	for(CLDSPart* pPart=pLineAngle->BelongModel()->EnumPartFirst();pPart;
		pPart=pLineAngle->BelongModel()->EnumPartNext())
	{	
		if(!pPart->IsPlate())
			continue;
		CLDSGeneralPlate *pRibPlate=(CLDSGeneralPlate*)pPart;
		if(pRibPlate->m_hPartWeldParent!=pLineAngle->handle)
			continue;
		BOOL bIsRibPlate=(pRibPlate->GetClassTypeId()==CLS_PARAMPLATE&&((CLDSParamPlate*)pRibPlate)->m_iParamType==TYPE_LINEANGLERIBPLATE);
		if(!bIsRibPlate)
			bIsRibPlate=fabs(angle_vec*pRibPlate->ucs.axis_z)>EPS_COS;
		if(!bIsRibPlate)
			continue;
		f3dPoint realPos=pRibPlate->ucs.origin+(pRibPlate->GetNormOffset()+pRibPlate->GetThick()*0.5)*pRibPlate->ucs.axis_z;
		coord_trans(realPos,angleDrawUcs,FALSE);
		RIB_PLATE_INFO_TEMP rib_plate_info;
		rib_plate_info.fPosZ=(float)realPos.z;
		rib_plate_info.sPlateNo.Copy(pRibPlate->GetPartNo());
		rib_plate_info.m_cMaterial=pRibPlate->cMaterial;
		rib_plate_info.fThick=(float)pRibPlate->GetThick();
		rib_plate_list.append(rib_plate_info);
	}
}

int GetProcessAngleExchangeBuffer(CLDSPart *part,CBuffer &buffer,bool bSaveSubSerial,long version/*=0*/)
{
	float bolt_d_increment=0;	//��˨ֱ������
	int process_info_first[8]={0};//��4����ʾ�Ƿ������5���Ƿ������6���Ƿ񺸽ӡ�7���Ƿ��¿�  ��1��0��
	int poly_part_num=0;//����ֶ���
	int push_flate =0;		//0:��ѹ��    1:X֫ѹ��    2:Y֫ѹ��
	int kaihe_base_wing=-1; //-1:�޿��Ͻ� 0:X֫���Ͻ�  1:Y֫���Ͻ�
	WORD start_flate_pos=0,end_flate_pos=0;//ʼ��ѹ��λ�ã��ն�ѹ��λ��
	ATOM_LIST<KAI_HE_JIAO_TEMP> kaiHeJiaoList;

	CLDSLineAngle *pJg=NULL,*pTailJg=NULL;
	CLDSPolyPart *pPolyPart=NULL;
	if(part->GetClassTypeId()==CLS_LINEANGLE)
	{
		pJg=(CLDSLineAngle*)part;
		pTailJg=pJg;
	}
	else if(part->GetClassTypeId()==CLS_POLYPART)
		pPolyPart=(CLDSPolyPart*)part;
	else 
		return 0;
	if(pPolyPart)
	{	
		pJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
		pTailJg =(CLDSLineAngle *)pPolyPart->segset.GetTail();
	}
	UCS_STRU ucs;
	pJg->getUCS(ucs);
	//1.��������˨�׾�����
	//bolt_count=(short)pJg->GetLsCount();
	CLsRef *pLsRef=pJg->GetFirstLsRef();
	if(pLsRef)
		bolt_d_increment=(float)(*pLsRef)->hole_d_increment	;//��˨ֱ������
	else 
		bolt_d_increment=0;

	if(pJg->m_bCutRoot)
		process_info_first[4]=1;		//���
	if(pJg->m_bCutBer)
		process_info_first[5]=1;		//����
	if(pJg->m_bWeldPart)
		process_info_first[6]=1;		//����

	if(pPolyPart)
	{
		poly_part_num=pPolyPart->segset.GetNodeNum();//����ֶ���
		pJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
	}
	CLDSLineAngle *pTempJg=pJg;
	do 
	{
		if(pTempJg->start_push_flat_x1_y2==1||pTempJg->end_push_flat_x1_y2==1||pTempJg->mid_push_flat_x1_y2==1)
			push_flate = 1;					//x֫ѹ��
		if(pTempJg->start_push_flat_x1_y2==2||pTempJg->end_push_flat_x1_y2==2||pTempJg->mid_push_flat_x1_y2==2)
			push_flate = 2;					//y֫ѹ��
		if(pPolyPart)
			pTempJg=(CLDSLineAngle*)pPolyPart->segset.GetNext();
		else
			break;
	} while (pTempJg);
	//-1��ʾ�����Ͻǣ�0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻ�
	kaihe_base_wing=pJg->GetKaiHeWingX0Y1();
	ATOM_LIST<BOLTHOLE_INFO>boltList;
	if(pPolyPart==NULL)
	{
		for(pLsRef=pJg->GetFirstLsRef();pLsRef;pLsRef=pJg->GetNextLsRef())
		{
			f3dCircle cir;
			cir.centre = (*pLsRef)->ucs.origin;
			coord_trans(cir.centre,ucs,FALSE);
			double g =pJg->GetLsG(pLsRef);
			cir.centre.Set(cir.centre.z+pJg->startOdd(),g,0);
			if(fabs(pJg->get_norm_x_wing()*(*pLsRef)->get_norm())>
				fabs(pJg->get_norm_y_wing()*(*pLsRef)->get_norm()))		//��˨�ڽǸ�X֫��
				cir.centre.y=-fabs(cir.centre.y);						//��ȷ���λ�ã�float������ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫��
			else														//��˨�ڽǸ�Y֫��
				cir.centre.y=fabs(cir.centre.y);						//��ȷ���λ�ã�float������ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫��
			//�Բ���Ҫ���п��ϽǵĶ̽Ǹֽ�����˨λ������   wxc-2016.10.28
			if((pJg->pStart==NULL || pJg->pEnd==NULL)&&pJg->GetKaiHeWingX0Y1()<0)
				pJg->EmendShortJgLsPosY(pLsRef->GetLsPtr(),cir.centre.y);
			cir.radius=(*pLsRef)->get_d()*0.5;
			cir.feature=(int)pLsRef;
			boltList.append(BOLTHOLE_INFO(cir.centre,pLsRef));
		}
	}
	else
	{
		CLDSPolyPart::AnaylyzeAngleBolts(pPolyPart);	//ͳ����˨ʱ�ѿ���ʼ������ͷ wht 15-09-24
		BOLTHOLE_INFO* pBoltHole,boltHole;
		for(pBoltHole=pPolyPart->boltListX.GetFirst();pBoltHole;pBoltHole=pPolyPart->boltListX.GetNext())
		{
			boltHole=*pBoltHole;
			boltHole.centre.y=-fabs(boltHole.centre.y);	//��ȷ���λ�ã�float������ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫
			boltList.append(boltHole);
		}
		for(pBoltHole=pPolyPart->boltListY.GetFirst();pBoltHole;pBoltHole=pPolyPart->boltListY.GetNext())
		{
			boltHole=*pBoltHole;
			boltHole.centre.y=fabs(boltHole.centre.y);	//��ȷ���λ�ã�float������ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫
			boltList.append(boltHole);
		}
	}

	double startLength=0.0; //��¼ѹ��ķֶνǸ�ǰ���νǸֳ��� 
	int nBiaoShi=0;	
	pTempJg=pJg;
	if(pPolyPart)
		pTempJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
	do 
	{
		double length=0.0;
		if(nBiaoShi>0)
		{
			length+=startLength;
			startLength+=pTempJg->GetLength();
		}
		nBiaoShi++;
		if(pTempJg->start_push_flat_x1_y2>0)
		{
			start_flate_pos=(WORD)length;
			end_flate_pos = (WORD)(pTempJg->start_push_flat_length+length);
			break;
		}
		else if(pTempJg->end_push_flat_x1_y2>0)
		{
			double lengthTemp=pTempJg->GetLength();
			start_flate_pos=(WORD)(lengthTemp-pTempJg->end_push_flat_length+length);
			end_flate_pos = 0xFFFF;	//�ն�ѹ��ʱ�ն�λ��Ӧ���ڽǸֳ���ֱ��ʹ�ó�����������������ܵ���С�ڳ�������Ϊ�м�ѹ�� wht 17-01-10
			break;
		}
		else if(pTempJg->mid_push_flat_x1_y2>0)
		{
			f3dPoint mid_push_flat_pos;
			CLDSNode *pMidPushFlatNode=pTempJg->BelongModel()->FromNodeHandle(pTempJg->mid_push_flat_node_handle);
			if(pMidPushFlatNode)
				mid_push_flat_pos=pTempJg->GetDatumPos(pMidPushFlatNode);
				//SnapPerp(&mid_push_flat_pos,pTempJg->Start(),pTempJg->End(),pMidPushFlatNode->Position(false));
			double len_starttomid = pTempJg->startOdd()+DISTANCE(pTempJg->Start(),mid_push_flat_pos);
			double start_pos = len_starttomid-pTempJg->mid_push_flat_start_offset+length;
			double end_pos = len_starttomid+pTempJg->mid_push_flat_end_offset+length;
			start_flate_pos = (WORD)start_pos;
			end_flate_pos=(WORD)end_pos;
			break;
		}
		if(pPolyPart)
			pTempJg=(CLDSLineAngle*)pPolyPart->segset.GetNext();
		else
			break;
	} while(pTempJg);
	//���Ͻ�
	if(pPolyPart!=NULL)
		ProcessPolyPartKaiHeJiao(pPolyPart,kaiHeJiaoList);
	else 
	{
		pJg=(CLDSLineAngle*)part;
		ucs.origin-=ucs.axis_z*pJg->startOdd();
		ProcessKaiHeAngle(pJg,ucs,kaiHeJiaoList);
	}
	//��֫�нǲ�����90�ȣ��Ҳ���Ҫ���п��ϽǵĽǸ�ʱ�����һ�����Ͻ���������¼��֫�н�Ĭ�Ͽ���λ��Ϊ0 wht 15-05-29
	float fWingAngle=(float)pJg->GetDecWingAngle();
	if(kaiHeJiaoList.GetNodeNum()==0&&fabs(fWingAngle-90)>0)
	{
		KAI_HE_JIAO_TEMP *pKaiHeJiao=kaiHeJiaoList.append();
		pKaiHeJiao->decWingAngle=fWingAngle;
		pKaiHeJiao->SetStartPosLength(0);
		pKaiHeJiao->SetEndPosLength(0);
	}
	//����Ǹ��߰�
	pTempJg=pJg;
	if(pPolyPart)
	{
		pTempJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();
		ucs=pPolyPart->GetUCS(true);
	}
	else 
	{
		pTempJg->getUCS(ucs);	
		ucs.origin-=ucs.axis_z*pTempJg->startOdd();
	}
	ATOM_LIST<RIB_PLATE_INFO_TEMP> rib_plate_list;
	do 
	{
		UpdateRibPlateList(pTempJg,ucs,rib_plate_list);
		if(pPolyPart)
			pTempJg=(CLDSLineAngle*)pPolyPart->segset.GetNext();
		else
			break;
	} while(pTempJg);

	int nInitLen = buffer.GetLength();
	//1.��������˨�׾�����
	long rib_plate_count=rib_plate_list.GetNodeNum();
	if(rib_plate_list.GetNodeNum()>127)
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The number of 0X%X{%s} angle's rib plate is %d and it supports a maximum of 127��",pJg->handle,pJg->GetPartNo(),rib_plate_list.GetNodeNum());
#else
		logerr.Log("0X%X{%s}�Ǹ��߰����Ϊ%d,�Ǹ��߰�������֧��127����",pJg->handle,pJg->GetPartNo(),rib_plate_list.GetNodeNum());
#endif
	buffer.WriteByte((BYTE)rib_plate_count);//�Ǹ��߰����
	buffer.WriteWord((WORD)boltList.GetNodeNum());//��˨����
	//buffer.WriteWord(bolt_count);			//��˨����
	buffer.WriteFloat(bolt_d_increment);	//��˨ֱ������
	
	//2.1B��������Ϣ�ֽ�1(1.ʼ�ˣ�֫�н�(֫),2.ʼ�ˣ�֫�н�(֫),3.�նˣ�֫�н�(֫),4.�նˣ�֫�н�(֫),5.���,6.����,7.����,8.�¿�)
	BYTE process_info_first_byte=0;
	const BYTE byteConstArr[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	BOOL processStateArr[8]={((pJg->cut_wing_para[0][0]&&pJg->cut_wing[0]==1)||pJg->cut_angle[0][0]>0),	//1.ʼ��X֫��֫(��)
							 ((pJg->cut_wing_para[0][0]&&pJg->cut_wing[0]==2)||pJg->cut_angle[1][0]>0),	//2.ʼ��Y֫��֫(��)
							 ((pJg->cut_wing_para[1][0]&&pJg->cut_wing[1]==1)||pJg->cut_angle[2][0]>0),	//3.�ն�X֫��֫(��)
							 ((pJg->cut_wing_para[1][0]&&pJg->cut_wing[1]==2)||pJg->cut_angle[3][0]>0),	//4.�ն�Y֫��֫(��)
							 process_info_first[4]==1,process_info_first[5]==1,							//5.���,6.����
							 process_info_first[6]==1,process_info_first[7]==1};						//7.����,8.�¿�
	for(int i=0;i<8;i++)
	{
		if(processStateArr[i])
			process_info_first_byte |= byteConstArr[i];
	}
	buffer.WriteByte(process_info_first_byte);
	//3. 1B��������Ϣ�ֽ�2(1~3λ��ʾ����ֶ�������4λ0��ʾX֫ѹ�⣬1��ʾY֫ѹ�⣬5~8λ����)
	BYTE process_info_second_byte=0;
	//3.1 ����ֶ���
	if(poly_part_num>7)
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The number of 0X%X{%s} bending segment is %d and it supports a maximum of 7��",pJg->handle,pJg->GetPartNo(),poly_part_num);
#else
		logerr.Log("0X%X{%s}����ֶ���Ϊ%d,��������֧��7���ֶΣ�",pJg->handle,pJg->GetPartNo(),poly_part_num);
#endif
	process_info_second_byte =(BYTE)poly_part_num; //����ֶ���
	//3.2 ѹ��֫
	if(push_flate==1)
		process_info_second_byte &=0xF7;				//��4λ0��ʾx֫ѹ��
	else if(push_flate==2)
		process_info_second_byte |=0x08;				//��4λ1��ʾy֫ѹ��
	buffer.WriteByte(process_info_second_byte);
	//4. 1B��������Ϣ�ֽ�3����1~7λ����������������8λ��0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻǣ�
	BYTE process_info_third_byte =0;
	//4.1 ����������(��1~7λ)
	if(kaiHeJiaoList.GetNodeNum()>127)
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The number of 0X%X{%s} leg opening and closing region is %d and it supports a maximum of 127��",pJg->handle,pJg->GetPartNo(),kaiHeJiaoList.GetNodeNum());
#else
		logerr.Log("0X%X{%s}���Ͻ�������Ϊ%d,���Ͻ����������֧��127����",pJg->handle,pJg->GetPartNo(),kaiHeJiaoList.GetNodeNum());
#endif
	process_info_third_byte=(BYTE)kaiHeJiaoList.GetNodeNum();
	//4.2 ��8λ��0��ʾX֫���Ͻǣ�1��ʾY֫���Ͻ�
	if(kaihe_base_wing==0)
		process_info_third_byte &=0x7F;		//x֫���Ͻ�
	else if(kaihe_base_wing==1)
		process_info_third_byte |=0x80;		//y֫���Ͻ�
	buffer.WriteByte(process_info_third_byte);
	//5.48B(4Bx4x3)�н�(֫)����
	for (int i=0;i<4;i++)
	{
		if(pJg->cut_angle[i][0]>0)
		{	//�н�
			buffer.WriteWord((WORD)pJg->cut_angle[i][0]);
			buffer.WriteWord((WORD)pJg->cut_angle[i][1]);
			buffer.WriteWord((WORD)0xFFFF);
		}
		else 
		{	//��֫
			int index=i<2?0:1;
			buffer.WriteWord((WORD)pJg->cut_wing_para[index][0]);
			buffer.WriteWord((WORD)pJg->cut_wing_para[index][1]);
			buffer.WriteWord((WORD)pJg->cut_wing_para[index][2]);
		}
	}
	//6.��˨��Ϣ
	//	4B�����ȷ���λ�ã�float��
	//	4B����ȷ���λ�ã�float������ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫��
	//	2B����˨�׾�
	//	8B����˨���ߺ�
	//	2B����Բ������
	//	24B����Բ�׷���
	int serial=1;
	BOLTHOLE_INFO *pBoltHole;
	for(pBoltHole=boltList.GetFirst();pBoltHole;pBoltHole=boltList.GetNext(),serial++)
	{
		if(bSaveSubSerial)
			buffer.WriteDword(serial);
		buffer.WriteFloat((float)pBoltHole->centre.x);		  //���ȷ���λ�ã�float��
		buffer.WriteFloat((float)pBoltHole->centre.y);		  //��ȷ���λ�ã�float������ֵ��ʾλ�ڣ�֫����ֵ��ʾλ�ڣ�֫
		buffer.WriteWord((short)(pBoltHole->bolt_d));		  //��˨ֱ��
		buffer.WriteWord(pBoltHole->waistLen);				  //��Բ��
		buffer.WriteDword(pBoltHole->dwRayNo);                //���ڵ����������ţ���ͼʱ��
		buffer.WritePoint(pBoltHole->axis_x);				  //��Բ�׷���
		buffer.WriteByte(pBoltHole->cFuncType);				  //��˨�׹�������
		if(version>=1000001)
			buffer.WriteFloat(pBoltHole->hole_d_increment);		//�׾�����
	}
	//7.�ֶνǸ�������ʶ����֫չ�����򡢣�֫չ���������췽��
	//�����ڵ�һ��ֱ�߽Ǹ֣�֫չ������Ϊ���ᣬ�˼����췽��Ϊ���������Ǹֶ�λ����ϵ�¶����
	if(pPolyPart)
	{
		int sign=0;
		f3dPoint datum_vec;
		for(CLDSLineAngle *pJg=(CLDSLineAngle*)pPolyPart->segset.GetFirst();pJg;pJg=(CLDSLineAngle*)pPolyPart->segset.GetNext())
		{
			f3dPoint norm_x=pJg->get_norm_x_wing();
			f3dPoint norm_y=pJg->get_norm_y_wing();
			f3dPoint len_vec ;
			Sub_Pnt(len_vec,pJg->End(),pJg->Start());
			if(datum_vec.IsZero())
			{
				if(pJg->bHuoQuStart)
					len_vec*=-1;
				datum_vec=len_vec;
			}
			else if(len_vec*datum_vec<0)
				len_vec*=-1;
			normalize(len_vec);
			len_vec*=pJg->GetLength();			//��λ�����ȷ������Ը˼�����(��������ͷ) wht 15-09-24
			buffer.WriteInteger(sign++);		//������ʶ
			//if (sign>1)
			{
				vector_trans(norm_x,ucs,FALSE);
				vector_trans(norm_y,ucs,FALSE);
				vector_trans(len_vec,ucs,FALSE);
			}
			buffer.WritePoint(norm_x);		 //X֫չ������
			buffer.WritePoint(norm_y);		 //Y֫չ������
			buffer.WritePoint(len_vec);		 //���췽��,����������Ϣ wht 15-05-05
		}
	}
	//8.ѹ����ʼλ�� ����ֹλ��
	//	2B��ѹ����ʼλ�ã����ȷ���0��ʾʼ��ѹ�⣩
	//	2B��ѹ����ֹλ�ã�>�������Ȼ�0xffff��ʾ�ն�ѹ�飩����ʼλ��>=��ֹλ�ñ�ʾ��ѹ�⡣
	buffer.WriteWord(start_flate_pos);
	buffer.WriteWord(end_flate_pos);
	//9.���Ͻ�LOOP
	for (KAI_HE_JIAO_TEMP *pKaiHeJiao=kaiHeJiaoList.GetFirst();pKaiHeJiao;pKaiHeJiao=kaiHeJiaoList.GetNext())
	{
		buffer.WriteFloat(pKaiHeJiao->decWingAngle);	//��֫�н�
		buffer.WriteWord(pKaiHeJiao->Position());		//�궨λ��
		buffer.WriteWord(pKaiHeJiao->StartLength());	//ʼ�˿��ϳ���
		buffer.WriteWord(pKaiHeJiao->EndLength());		//�ն˿��ϳ���
	}
	//10.�Ǹ��߰�LOOP
	for (RIB_PLATE_INFO_TEMP *pRibPlateInfoTemp=rib_plate_list.GetFirst();pRibPlateInfoTemp;pRibPlateInfoTemp=rib_plate_list.GetNext())
	{
		buffer.WriteFloat(pRibPlateInfoTemp->fPosZ);				//�߰�λ��
		buffer.WriteFloat(pRibPlateInfoTemp->fThick);				//�߰���
		buffer.Write(&pRibPlateInfoTemp->m_cMaterial,sizeof(char));	//����
		buffer.Write(pRibPlateInfoTemp->sPlateNo,16*sizeof(char));	//�߰���
	}
	return buffer.GetLength()-nInitLen;
}
#else
int GetProcessAngleExchangeBuffer(CLDSPart *part,CBuffer &buffer,bool bSaveSubSerial,long version/*=0*/)
{
	return 0;
}
#endif
int CLDSLineAngle::WriteProcessBuffer(CBuffer &buffer,bool bSaveSubSerial/*=false*/,long version/*=0*/)
{	
	return GetProcessAngleExchangeBuffer(this,buffer,bSaveSubSerial,version);
}
void CLDSLineAngle::EmendShortJgLsPosY(CLDSBolt* pBolt,double &fLsPosY)
{
	//1.��ȡ�ö̽ǸֵĿ��Ͻ�
	double fAngle=this->GetRadWingAngle();
	if(fabs(fAngle-Pi*0.5)<EPS)
		return;
	double fTanValue=tan(fAngle*0.5);
	//2.������˨λ��
	//2.1 ������˨��ȡ��˨�������ĽǸֵĺ��
	BOOL pushed=BelongModel()->PushPartStack();
	CLDSLineAngle* pMainAngle=NULL;
	for(THICK_ITEM item=pBolt->GetFirstL0ThickItem();!item.IsFinalKey();item=pBolt->GetNextL0ThickItem())
	{
		if(!item.IsHandle() || (item.IsHandle() && item.val==this->handle))
			continue;
		pMainAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(item.val,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pMainAngle!=NULL&&pMainAngle->pStart&&pMainAngle->pEnd&&pBolt->des_base_pos.hPart==pMainAngle->handle)
		{
			//����ͳһ��Ϊ�̽Ǹ��Ƿ���Ҫ���Ͻǣ��̽Ǹֱ������Ͻ�ʱ�������������������ж��û����ÿ��ơ�wjh-2016.10.31
			break;
			//if(pMainAngle->GetKaiHeWingX0Y1()==-1)
			//	break;
			//TODO:�������ϽǽǸ��ϴ���İ�����ͷ�������ͷ��Ҳ���迪�Ͻǣ�
			//�������ǣ�������˨�����ӹ���ֻ����һ����׼�Ǹ֣�����Ǹ�Ϊ�̽Ǹֻ����׼�Ǹ�ƽ�У�
			//�Ҳ����������Ӱ����CLDSPlate::desingInfo.partList.GetNodeNum()==0��ֻ��pMainAngle
			//Ӧ�����ظ����ô˴���
			//else if(�ֲ����迪�Ͻ������
		}
		pMainAngle=NULL;
	}
	BelongModel()->PopPartStack(pushed);
	//2.2 ������˨λ����Ϣ
	if(pMainAngle)
	{
		GEPOINT offset=pMainAngle->Start()-this->Start();
		double offsetThickX=offset*this->vxWingNorm;
		double offsetThickY=offset*this->vyWingNorm;
		if(fLsPosY>eps)	//Y֫
			fLsPosY+=ftoi(offsetThickY/fTanValue-offsetThickY);
		else	//X֫
			fLsPosY-=ftoi(offsetThickX/fTanValue-offsetThickX);
	}
}
#ifdef __LDS_CONTEXT_
bool CLDSLineAngle::CloneToBomPart(BOMPART* pBomPart)
{
	if (pBomPart->cPartType != BOMPART::ANGLE)
		return false;
	CLDSPart::CloneToBomPart(pBomPart);	//��¡��������
	//��¡�Ǹ���������
	PART_ANGLE* pBomAngle = (PART_ANGLE*)pBomPart;
	if (pStart == NULL || pEnd == NULL)
		pBomAngle->siSubType = BOMPART::SUB_TYPE_ANGLE_SHORT;
	pBomAngle->wingWideY = GetWidthWingY();
	pBomAngle->wing_angle = (float)GetDecWingAngle();
	pBomAngle->bCutRoot = m_bCutRoot;
	pBomAngle->bCutBer = m_bCutBer;
	pBomAngle->bHasFootNail = IsHasFootNail();
	double dfKaihejiaoThreshold = library != NULL ? library->GetJgKaiHeJiaoThreshold() : 2;
	if (GetDecWingAngle() - 90 > dfKaihejiaoThreshold &&GetKaiHeWingX0Y1() != -1)
		pBomAngle->bKaiJiao = TRUE;
	else
		pBomAngle->bKaiJiao = FALSE;
	if (90 - GetDecWingAngle() > dfKaihejiaoThreshold &&GetKaiHeWingX0Y1() != -1)
		pBomAngle->bHeJiao = TRUE;
	else
		pBomAngle->bHeJiao = FALSE;
	// ����
	//���Ӹ�����
	BOOL bTempWeldPart = FALSE;
	CLDSPart *pFatherPart = BelongModel()->FromPartHandle(m_hPartWeldParent);
	if (pFatherPart == NULL)
	{	//�жϸù����Ƿ���ں����ӹ�������������Ϊ�ù�����Ϊ���Ӽ�
		BOOL bPush = m_pModel->PushPartStack();
		for (CLDSPart* pSonPart = m_pModel->EnumPartFirst(); pSonPart; pSonPart = m_pModel->EnumPartNext())
		{
			if (pSonPart->m_hPartWeldParent == handle)
			{
				bTempWeldPart = TRUE;
				break;
			}
		}
		if (bPush)
			m_pModel->PopPartStack();
	}
	else
		bTempWeldPart = TRUE;
	if (IsCutAngleOrWing())
	{
		pBomAngle->bCutAngle = 1;//TRUE;
		if ((cut_wing[0] > 0 && cut_wing_para[0][1] > 0) ||
			(cut_wing[1] > 0 && cut_wing_para[1][1] > 0))	//����֫
			pBomAngle->bCutAngle = 2;	//BCģʽ�������������ṩ��ʽҪ��
	}
	else
		pBomAngle->bCutAngle = 0;//FALSE;
	pBomAngle->bWeldPart = (bTempWeldPart || m_bWeldPart);
	pBomAngle->nPushFlat = 0;
	//ͳ�ƽǸ�ѹ�� wht 19-07-19
	pBomAngle->listPushFlat.DeleteList();
	if (start_push_flat_x1_y2 > 0)
	{
		pBomAngle->nPushFlat |= 0x01;	//ʼ��ѹ��
		ANGLE_PUSH_FLAT *pStartPushFlat = pBomAngle->listPushFlat.Append();
		pStartPushFlat->push_flat_x1_y2 = start_push_flat_x1_y2;
		pStartPushFlat->flat_start_pos = 0;
		pStartPushFlat->flat_end_pos = (short)start_push_flat_length;
	}
	if (mid_push_flat_x1_y2 > 0)
	{
		pBomAngle->nPushFlat |= 0x02;	//�м�ѹ��
		CLDSNode *pMidPushFlatNode = BelongModel()->FromNodeHandle(mid_push_flat_node_handle);
		if (pMidPushFlatNode)
		{
			ANGLE_PUSH_FLAT *pStartPushFlat = pBomAngle->listPushFlat.Append();
			pStartPushFlat->push_flat_x1_y2 = mid_push_flat_x1_y2;
			//�����м�ѹ�⿪ʼλ�� wht 19-06-27
			f3dPoint mid_push_flat_pos = GetDatumPos(pMidPushFlatNode);
			double len_starttomid = startOdd() + DISTANCE(Start(), mid_push_flat_pos);
			double start_pos = len_starttomid - mid_push_flat_start_offset;
			double end_pos = len_starttomid + mid_push_flat_end_offset;
			pStartPushFlat->flat_start_pos = (WORD)start_pos;
			pStartPushFlat->flat_end_pos = (WORD)end_pos;
		}
	}
	if (end_push_flat_x1_y2 > 0)
	{
		pBomAngle->nPushFlat |= 0x04;	//�ն�ѹ��
		ANGLE_PUSH_FLAT *pEndPushFlat = pBomAngle->listPushFlat.Append();
		pEndPushFlat->push_flat_x1_y2 = end_push_flat_x1_y2;
		pEndPushFlat->flat_start_pos = (short)(GetLength() - end_push_flat_length);
		pEndPushFlat->flat_end_pos = (short)GetLength();
	}
	if (strlen(sNotes) > 0)
		strcpy(pBomAngle->sNotes, sNotes);
	pBomAngle->cut_wing[0] = cut_wing[0];
	pBomAngle->cut_wing[1] = cut_wing[1];
	pBomAngle->cut_wing_para[0][0] = ftoi(cut_wing_para[0][0]);
	pBomAngle->cut_wing_para[0][1] = ftoi(cut_wing_para[0][1]);
	pBomAngle->cut_wing_para[0][2] = ftoi(cut_wing_para[0][2]);
	pBomAngle->cut_wing_para[1][0] = ftoi(cut_wing_para[1][0]);
	pBomAngle->cut_wing_para[1][1] = ftoi(cut_wing_para[1][1]);
	pBomAngle->cut_wing_para[1][2] = ftoi(cut_wing_para[1][2]);
	pBomAngle->cut_angle[0][0] = ftoi(cut_angle[0][0]);
	pBomAngle->cut_angle[1][0] = ftoi(cut_angle[1][0]);
	pBomAngle->cut_angle[2][0] = ftoi(cut_angle[2][0]);
	pBomAngle->cut_angle[3][0] = ftoi(cut_angle[3][0]);
	pBomAngle->cut_angle[0][1] = ftoi(cut_angle[0][1]);
	pBomAngle->cut_angle[1][1] = ftoi(cut_angle[1][1]);
	pBomAngle->cut_angle[2][1] = ftoi(cut_angle[2][1]);
	pBomAngle->cut_angle[3][1] = ftoi(cut_angle[3][1]);
	//��˨
	pBomAngle->nMSumLs = GetLsCount();
	pBomAngle->m_arrBoltRecs.SetSize(pBomAngle->nMSumLs);
	GEPOINT lenStdVec = End() - Start();
	normalize(lenStdVec);
	int i = 0;
	for (CLsRef *pLsRef = GetFirstLsRef(); pLsRef; pLsRef = GetNextLsRef())
	{
		int X0_Y1;
		IsLsInWing(pLsRef->GetLsPtr(), &X0_Y1);
		GEPOINT bolt_vec = (*pLsRef)->ucs.origin - Start();
		float lenpos = (float)(bolt_vec*lenStdVec + startOdd());
		float g = (float)GetLsG(pLsRef);
		pBomAngle->m_arrBoltRecs[i].d = ftoi((*pLsRef)->get_d());
		pBomAngle->m_arrBoltRecs[i].hole_d_increment = (float)(*pLsRef)->hole_d_increment;
		pBomAngle->m_arrBoltRecs[i].x = lenpos;
		if (X0_Y1 == 0)		//X֫
			pBomAngle->m_arrBoltRecs[i].y = -g;
		else if (X0_Y1 == 1)	//Y֫
			pBomAngle->m_arrBoltRecs[i].y = g;
		else	//��������
			pBomAngle->m_arrBoltRecs[i].y = 0;
		pBomAngle->m_arrBoltRecs[i].waistLen = pLsRef->waistLen;
		CLDSBolt* pBolt = pLsRef->GetLsPtr();
		pBomAngle->m_arrBoltRecs[i].cFuncType = pBolt->FuncType();
		pBomAngle->m_arrBoltRecs[i].cFlag = pLsRef->cFlag;
		i++;
	}
	//ʹ���û����õĿ��Ͻ���ֵ�жϽǸ��Ƿ���Ҫ���п��Ͻ� wht 11-05-06
	ATOM_LIST<KAI_HE_JIAO_TEMP> listKaiHeJiao;
	getUCS(ucs);
	ucs.origin -= ucs.axis_z*startOdd();
	ProcessKaiHeAngle(this, ucs, listKaiHeJiao);
	pBomAngle->listKaihe.DeleteList();
	for (KAI_HE_JIAO_TEMP* pKaiHe = listKaiHeJiao.GetFirst(); pKaiHe; pKaiHe = listKaiHeJiao.GetNext())
	{
		KAI_HE_JIAO kaihe;
		kaihe.decWingAngle = (float)GetDecWingAngle();
		kaihe.position = pKaiHe->Position();
		kaihe.startLength = pKaiHe->StartLength();
		kaihe.endLength = pKaiHe->EndLength();
		pBomAngle->listKaihe.AttachNode(kaihe);
	}
	return true;
}
#endif
#endif
