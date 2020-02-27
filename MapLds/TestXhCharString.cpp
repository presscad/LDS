#include "stdafx.h"
#include "Tower.h"
#include "XhCharString.h"

void TestXhString()
{}
	/*CXhString sDest;
	//1.Copy
	sDest.Copy("Test");
	if(strcmp(sDest,"Test")!=0)
		logto.Log("XhString Copy Fuction ERROR");
	//2.Printf
	
	//3.Append

	//4.Append

	//5.At

	//6.Equal

	//7.EqualNoCase

	//8.operator

	//9.Length

	//10.LengthMax

	//11.ResizeLength

	//12.StartWith

	//13.EndWith
}
void TestXhChar16()
{
	CXhChar16 sDest("hello word");
	//LengthMax
	if(sDest.LengthMax()!=16)
		logto.Log("CXhChar16 LengthMax() is ERROR");
	
	//Length
	if(sDest.Length()!=strlen("hello word"))
		logto.Log("CXhChar16 Length() is ERROR");
	
	//Copy
	sDest.Copy("Test");
	if(strcmp(sDest,"Test")!=0)
		logto.Log("CXhChar16 Copy1() ERROR");
	sDest.Copy("AaBbCcDdEeFfGgHh******");
	if(strcmp(sDest,"AaBbCcDdEeFfGgHh")!=0)
		logto.Log("CXhChar16 Copy2() ERROR");
	
	//Empty
	sDest.Empty();
	if(strlen(sDest)!=0)
		logto.Log("CXhChar16 Empty() ERROR");
	
	//Append(char*)
	sDest.Append("hello");
	sDest.Append("word");
	if(strcmp(sDest,"helloword")!=0)
		logto.Log("CXhChar16 Append1() ERROR");

	sDest.Append("123456789");
	if(strcmp(sDest,"helloword1234567")!=0)
		logto.Log("CXhChar16 Append2() ERROR");
	
	//Append(char)
	CXhChar16 sDest2("hello word");
	sDest2.Append('!');
	if(strcmp(sDest2,"hello word!")!=0)
		logto.Log("CXhChar16 Append2() ERROR");
	
	//Equal
	if(!sDest2.Equal("hello word!"))
		logto.Log("CXhChar16 Equal() ERROR");
	
	//EqualNoCase
	if(!sDest2.EqualNoCase("hello word!"))
		logto.Log("CXhChar16 EqualNoCase() ERROR");
	
	//operator
	CXhChar16 str("how are you");
	sDest2=str;
	if(strcmp(sDest2,"how are you")!=0)
		logto.Log("CXhChar16 operator= ERROR");
	sDest2+="!!!!";
	if(strcmp(sDest2,"how are you!!!!")!=0)
		logto.Log("CXhChar16 operator+= ERROR");
	
	//At
	if(sDest2.At(5)!='a')
		logto.Log("CXhChar16 At() ERROR");
	
	//ResizeLength
	CXhChar16 sDest3("abcdef");
	sDest3.ResizeLength(10,'*');
	if(strcmp(sDest3,"abcdef****")!=0)
		logto.Log("CXhChar16 ResizeLength1() ERROR");
	sDest3.ResizeLength(18,'*',true);
	if(strcmp(sDest3,"**********abcdef")!=0)
		logto.Log("CXhChar16 ResizeLength2() ERROR");

	//StartWith

	//EndWith

	//Printf
}*/