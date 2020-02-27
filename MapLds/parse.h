#ifndef PARSE_H
#define PARSE_H
/// 将一个代表编号序列的字符串解析为逐个编号
/// </summary>
/// <param name="limit_str">编号序列字符串 </param>
/// <param name="delimiter1">编号间分隔符，一般为'+'</param>
/// <param name="delimiter2">编号范围延续符，一般为'-'</param>
/// <param name="delimiter3">编号范围延续符，一般为'*'</param>
//CHashTable<int> table;
	long FindAdaptNo(char *limit_str,char *delimiter1/*=NULL*/,char *delimiter2/*=NULL*/)
	{
		char *limstr=NULL;
		if(limit_str)
		{
			limstr=new char[strlen(limit_str)+1];
			strcpy(limstr,limit_str);
		}
		long iNo;
		if(limit_str)
		{
			//NoArr.Empty();
			//table.Empty();
			char *sKey;
			if(delimiter1)
				sKey=strtok(limstr,delimiter1);
			else
				sKey=strtok(limstr,",\n");
			while(sKey)
			{
				char *delimiter;
				if(delimiter2)
					delimiter=strchr(sKey,*delimiter2);
				else
					delimiter=strchr(sKey,'-');
				if(delimiter)
				{		
					*delimiter=' ';
					int start_i,end_i;
					sscanf(sKey,"%d%d",&start_i,&end_i);
					for(iNo=start_i;iNo<=end_i;iNo++)
						NoArr.append(iNo);
				}
				else
				{
					iNo=atoi(sKey);
					NoArr.append(iNo);
				}
				sKey=strtok(NULL,",\n");
			}
			long *pNo=NoArr.GetFirst();
			if(pNo)
				iNo=*pNo;
			else
				iNo=0;
		}
		else
		{
			long *pNo=NoArr.GetNext();
			if(pNo)
				iNo=*pNo;
			else
				iNo=0;
		}
		if(limstr)
			delete []limstr;
		return iNo;
}
#endif