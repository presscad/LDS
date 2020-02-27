#ifndef __ATOM_PTR_LIST
#define __ATOM_PTR_LIST
#include "f_ent_list.h"
//链表模板类
template <class TYPE> class ATOMPTR_LIST
{
	ATOM_LIST<TYPE> *type_list;
public:
    ATOMPTR_LIST();
    ~ATOMPTR_LIST();

//4.公有成员函数定义
public:

	void PushAtomPos(){if(type_list) type_list->push_stack();}
	void PopAtomPos(){if(type_list) type_list->pop_stack();}
	//BOOL DeleteAtom(THANDLE atom_handle){return type_list->DeleteNode(atom_handle);}
	int GetAtomCount(){return type_list->GetNodeNum();}
	void EmptyAtom()
	{
		type_list->Empty();
	}
	TYPE* GetFirstAtom(){return type_list->GetFirst();}
	TYPE* GetNextAtom(){return type_list->GetNext();}
    TYPE* AppendAtom(){return type_list->append();}	//在节点链表的末尾添加节点
};
template <class TYPE>
inline ATOMPTR_LIST<TYPE>::ATOMPTR_LIST(){	// 构造函数
    type_list=new ATOM_LIST<TYPE>;
}
template <class TYPE>
inline ATOMPTR_LIST<TYPE>::~ATOMPTR_LIST(){	// 构造函数
    delete type_list;
}

#endif