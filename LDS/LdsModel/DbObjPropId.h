#include "stdafx.h"

#pragma once

struct DBOBJ_PROPID{
	static const WORD PROP_LOCATION		=1;	//װ��λ��
	static const WORD PROP_SIZESPEC		=2;	//�������
	static const WORD PROP_WIDTH		=3;	//�������
	static const WORD PROP_THICK		=4;	//�������
	static const WORD PROP_LENGTH		=5;	//��������
	static const WORD PROP_MATERIAL		=6;	//��������
	static const WORD PROP_NORMAL		=7;	//��������
	static const WORD PROP_WINGX_NORMAL=8;	//�Ǹ�X֫����
	static const WORD PROP_WINGY_NORMAL=9;	//�Ǹ�Y֫����
	static const WORD PROP_POS_START	=10;//�˼�ʼ��λ��
	static const WORD PROP_POS_END		=11;//�˼��ն�λ��
	static const WORD PROP_PROFILE		=12;//�ְ�����
};
struct PLATE_PROPID : public DBOBJ_PROPID
{
	struct BASE{	//������
		//Բ�λ�����
		static const WORD R = 13; 
		//���λ�����
		static const WORD XA = 14;
		static const WORD XB = 15;
		static const WORD YA = 16;
		static const WORD YB = 17;
	};
	struct STATIONCONNPLATE{
		static const WORD PROP_HOLE_R=13;	//�Ҷ���ֱ��
		static const WORD PROP_HOLE_H=14;	//�Ҷ��׸߶�
	};
	struct STATIONSHOEPLATE{
		static const WORD PROP_HOLE_R=13;	//�Ҷ���ֱ��
		static const WORD PROP_HOLE_H=14;	//�Ҷ��׸߶�
	};
	struct STATIONCROSSPLATE{
		static const WORD PROP_PARA_H=13;	//���߶�
	};
};
