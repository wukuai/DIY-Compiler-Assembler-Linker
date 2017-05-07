#include <bits/stdc++.h>

#define MYCOMPILER
#define MAXWORD		10				//token和keyword最大长度
#define MAXLENGTH	30
#define MAXC		50				//符号表的最大容量
#define MAXOP		500				//最大的操作指令条数
#define MAXLEV		10				//最大的层数
#define MAXF		10				//最多的实常量数
#define MAXCASE		50
#define MAXGO		100				//最大运行栈
#define MAXGOLEV	10				//运行栈最大的调用层
#define	MAXS		5				//printf语句中最多的转换字符数
#define MAXERROR	50
#define kk			12				//关键词个数

char *Keyword[]={					//关键字
	"const","int","char","void",
	"main","if","else", "do","while",
	"scanf","printf", "return"
};

char *oprate[]={					//操作码
	"","","JME","JMC","JMP","CAS",
	"RED","EXF","WRI","",":+",":-","",
	":*",":/","MUS", "j>", "j<",  "j==",
	"j!=",  "j>=", "j<=","","CAL","BEGIN",
	"END",":="
};

union Cha{
	int num;
	char cischar;
};

struct Character{					//符号表
	char *name ;
	int type,lev,adr,kind;
	Cha uchar;
};

struct opr{							//中间代码
	int op,op1,op2,result,cas;
};

struct aopr{
	int anum,ad1;

};

char* err_msg[] =					//定义出错提示信息
{
/*0*/ "",
/*1*/ "标识符或数字过长",
/*2*/ "非法单词",
/*3*/ "缺少符号",
/*4*/ "缺少类型标识符",
/*5*/ "字符串过长或缺少符号",
/*6*/ "此处缺少运算的因子",
/*7*/ "符号表出界",
/*8*/ "标识符重复定义",
/*9*/ "此处应有'='",
/*10*/ "变量使用出错",
/*11*/ "无返回值函数不能用在此处",
/*12*/ "缺少“}”",
/*13*/ "缺少','",
/*14*/ "无返回值函数不能有return语句",
/*15*/ "无主函数",
/*16*/ "函数调用时参数个数不对",
/*17*/ "缺少'('",
/*18*/ "缺少')'",
/*19*/ "编译生成指令数过多",
/*20*/ "标识符未定义",
/*21*/ "转换符用法错误,为非变量型数据赋值",
/*22*/ "缺少“;”",
/*23*/ "缺少“{”",
/*24*/ "运行栈泄漏"
/*25*/ "函数调用超过了最大的层次数",
/*26*/ "函数返回值与函数类型不匹配",
/*27*/ "此处应为标识符",
/*28*/ "变量的类型和值的类型不匹配",

};

FILE *FIN, *FOUT;                  //输入输出文件指针
FILE *EquFout;                     //四元式文件
FILE *KeyWord;                     //关键字文件
FILE *Oprate;                      //操作符文件
FILE *Token;
FILE *Eorro;
FILE *Cifa;
FILE *huibian;

char Word[MAXWORD];					//store token
int num;							//store 整数
char sisstring[MAXLENGTH];			//store 字符串
int sym;							//store 标号；
char cischar;						//store 字符
char fp[20]={"./work/input.txt"};						//输入文件名
int linenum=1;					//源程序行号
char name[MAXC][MAXWORD];		//存标识符
int NumOfC=1;					//符号表内元素的个数
Character Char[MAXC];			//符号表
int numoflev[MAXLEV];			//各层的表中变量数
opr  opra[MAXOP];				//存指令
aopr aop[MAXOP];
int oprnum;						//指令数
int go[MAXGO];					//运行栈
int dl[MAXGOLEV],bl[MAXGOLEV];	//运行栈辅助指针
int errornum=0,error[MAXERROR],errorline[MAXERROR];
int gonum=0,dlnum=0,blnum=0;
int type=0;
int snum=0;
int start;
int lev=1;
int breakopr,Isbreak=0;
int typeofexp1=11, typeofexp2=12;
int oprnumcase=0;
int Isret=1;
int adr[MAXC];
int ad=0;
int ab=0;
int gsym[MAXC];
int sy=0;
int A=0;
int adr1[MAXC];

//子函数说明
int getsym();									//词法分析程序
int enter(int lev,int type,int kind);			//把变量填入符号表
//int constdeclaration();							//分析常量说明部分
int program();									//分析程序
int charcheck();								//检查标识符是否重复定义
//int constdefine( int type );					//分析常量定义
int vardefine( int type);						//分析变量说明部分
int returnfunction( int type)	;				//有返回值的函数
int voidfunction( );							//无返回值的函数定义语句
int statementArray( );							//语句列
int statement( );								//语句
int ifstatement( );								//if条件语句
int whilestatement( );							//while循环语句
int scanfstatement( );							//scanf读语句
int printfstatement( );							//printf写语句
int returnstatement( );							//return语句
int assignstatement(int a);						//赋值语句
int mainfunction( );							//主函数
int call(int a);								//函数调用语句
int expression();								//表达式
int gen(int op,int op1,int op2,int result,int mode);					//产生一条表达式
int term();										//项
int factor();									//因子
int find();										//在符号表中查找标识
int backout();									//不用的变量要退出符号表
void error_msg(int i);
int newTemp();

//初始化文件
int init()
{
	int i=0;
	KeyWord=fopen("./init/keyword.txt","w");
	for(i=0;i<12;i++)
		fprintf(KeyWord,"%d\t\t%s\n",i,Keyword[i]);
	fclose(KeyWord);

	Oprate=fopen("./init/oprate.txt","w");
	for(i=0;i<26;i++)
		fprintf(Oprate,"%d\t\t%s\n",i,oprate[i]);
	fclose(Oprate);

	EquFout=fopen("./work/four.txt","w");
	fclose(EquFout);


	return 0;
}


//错误处理
void error_msg(int i)
{
	fprintf(Eorro,"line(%d) error%d: %s\n",errorline[errornum],i,err_msg[i]);
}


//字符获取和处理
int getsym()
{
	char ch;
	int i,j,m;
	ch=fgetc(FIN);             //从读入文件读入字符
	while(isspace(ch))         //处理空格
	{
		//putc(ch,FOUT);
		if(ch=='\n')             //是换行符号
		{
			linenum++;           //行数加1
			//printf("%d:\t",linenum);
		}
		ch=fgetc(FIN);
	}

	i=0;
	Word[i]=ch;
	i++;

    //sym=tokenAnalyze();   //token 标识符处理
	//token处理
	if(isalpha(ch))   //如果是字符
	{
		//putc(ch,FOUT);   //输出
		ch=fgetc(FIN);
		while(isalpha(ch)||isdigit(ch))
		{
			Word[i]=ch;
			i++;

			if(i==MAXWORD)   //字符串长度超过最大长度MAXWORD=10
			{
				errorline[errornum]=linenum;
				error[errornum++]=1;       //错误提示为字符串过长
				i--;
				Word[i]='\0';
				for (int i=0;Word[i]!='\0';i++)
				putc(Word[i],Cifa);
				char tmp='\n';
				putc(tmp,Cifa);
				//putc(ch,FOUT);
				ch=fgetc(FIN);

				while(isalpha(ch)||isdigit(ch))
				{
					//putc(ch,FOUT);
				}
				ungetc(ch,FIN);
				return 1;     //是标识符
			}
			//putc(ch,FOUT);
			ch=fgetc(FIN);
		}
		ungetc(ch,FIN);  //
		Word[i]='\0';   //加0
		for (int i=0;Word[i]!='\0';i++)
        putc(Word[i],Cifa);
        char tmp='\n';
        putc(tmp,Cifa);
		for(m=0;m<kk;m++)
		{
  			if(!(strcmp(Keyword[m],Word)))	  //读取的字符串符合对key word的判断
			{
				return m+10;      //返回相应的keyword的对应数字
			}
		}
		if(m==kk){
			return 1;        //不是key word,是标识符
		}
	}

	//sym=numberAnalyze();   //数字处理
	//数值处理
	if(isdigit(ch))  //如果是数字
	{
		//putc(ch,FOUT);

		while(isdigit((ch=fgetc(FIN))))
		{
			//putc(ch,FOUT);
			Word[i++]=ch;
			if(i==MAXWORD)
			{
				errorline[errornum]=linenum;
				error[errornum++]=1;
				Word[--i]='\0';
				for (int i=0;Word[i]!='\0';i++)
				putc(Word[i],Cifa);
				char tmp='\n';
				putc(tmp,Cifa);
				ch=fgetc(FIN);
				while(isdigit(ch))
				{
					//putc(ch,FOUT);
				}
				num=atoi(Word);   //将字符串转化为对应的数字
				ungetc(ch,FIN);
				return 2;      //是数字
			}

		}
		ungetc(ch,FIN);
		Word[i]='\0';
		for (int i=0;Word[i]!='\0';i++)
		putc(Word[i],Cifa);
		char tmp='\n';
				putc(tmp,Cifa);
		num=atoi(Word);   //将字符串转化为对应的数字
		return 2;      //是数字
	}

	//分界符
	i=34;     // 34为 /
	//putc(ch,FOUT);
	switch(ch){
	case'\'':
			ch=fgetc(FIN);
			if(ch=='+'||ch=='-'||ch=='*'||ch=='\''||isalpha(ch)) //对应于printf语句中的各种参数  可能不需要.
			{
				cischar=ch;
				//putc(ch,FOUT);
				putc(ch,Cifa);
				if ((ch=fgetc(FIN))=='\'')
				{
					//putc(ch,FOUT);
					putc(ch,Cifa);
					putc('\n',Cifa);
					return 4;//字符
				}
				else
				{
					//putc(ch,FOUT);
					putc(ch,Cifa);
					putc('\n',Cifa);
					errorline[errornum]=linenum;
					error[errornum++]=3;
					return -1;
				}
			}
	case'"':   //处理“”
			j=0;
			while((ch=fgetc(FIN))!='"')  //若不是非空串""
			{
				//putc(ch,FOUT);
				putc(ch,Cifa);
				sisstring[j++]=ch;

				if(j==MAXLENGTH)
				{
					printf("warning!字符串过长\n");
					sisstring[--j]='\0';
					//putc(ch,FOUT);
					putc(ch,Cifa);
					ch=fgetc(FIN);
					while(ch!='"')
					{
						//putc(ch,FOUT);
						putc(ch,Cifa);
						j++;
						if(ch==EOF||j>100)	//字符串过长或缺少反引号
						{
							errorline[errornum]=linenum;
							error[errornum++]=5;
							return -1;
						}
					}
					putc('\n',Cifa);
					return 5;//返回字符串
				}
			}
			//putc(ch,FOUT);
			putc(ch,Cifa);
            putc('\n',Cifa);
			sisstring[j]='\0';//字符串结尾
			return 5;//返回空字符串

    case'}':
        putc('}',Cifa);
					putc('\n',Cifa);
		return 35;
	case'(':
		i++;
		putc('(',Cifa);
					putc('\n',Cifa);
		return 37;   //是( 返回37, 数字37表示(
	case')':
                     putc(')',Cifa);
					putc('\n',Cifa);
		i++;
		return 36;   // 是)返回36, 数字36表示)
	case';':
	    putc(';',Cifa);
					putc('\n',Cifa);
		i++;
		return 38;   //38为;  数字38表示;
	case',':
	    putc(',',Cifa);
					putc('\n',Cifa);
		i++;
		return 39;
	case '{':
	    putc('{',Cifa);
					putc('\n',Cifa);
		i++;
		return 40;    // 是{返回 40, 数字40表示{

	case'/':
		if((ch=getc(FIN))=='/')              //若为单行注释标志
		{
			//putc(ch,FOUT);                   //输出 /
			putc(ch,Cifa);
			while((ch=fgetc(FIN))!='\n')     // 当不是转行符
            {//putc(ch,FOUT);
            putc(ch,Cifa);}               // 注释内容不处理，直接输出
			//putc(ch,FOUT);
			putc(ch,Cifa);
			linenum++;                      //行数加1
			printf("%d:\t",linenum);        //输出行数
					putc('\n',Cifa);
			return (getsym());             //递归调用，处理下一行代码
		}
		ungetc(ch,FIN);
		return 34; //仅为/
	case'*':
	    putc('*',Cifa);
					putc('\n',Cifa);
		return 33;
	case '-':
	    putc('-',Cifa);
					putc('\n',Cifa);
	case'+':

		if(sym!=0&&sym!=1&&sym!=2)//不在等式中
		{
			j=0;
			Word[j++]=ch;
			ch=fgetc(FIN);
			if(isdigit(ch))
			{
				Word[j++]=ch;
				//putc(ch,FOUT);
				putc(ch,Cifa);
				while(isdigit(ch=fgetc(FIN)))
				{
					Word[j++]=ch;
					//putc(ch,FOUT);
					putc(ch,Cifa);
					if(i==MAXWORD)
					{
						errorline[errornum]=linenum;
						error[errornum++]=1;
						Word[--j]='\0';

						ch=fgetc(FIN);
						while(isdigit(ch))
						{
							//putc(ch,FOUT);
						}
						for (int i=0;Word[i]!='\0';i++)
						putc(Word[i],Cifa);
						char tmp='\n';
                        putc(tmp,Cifa);
						num=atoi(Word);
						ungetc(ch,FIN);
						return 2;//带符号的数字
					}
				}
				ungetc(ch,FIN);
				Word[j]='\0';
				for (int i=0;Word[i]!='\0';i++)
				putc(Word[i],Cifa);
				char tmp='\n';
				putc(tmp,Cifa);
				num=atoi(Word);
				return 2;//带符号的数字
			}
		}
		else			//是运算符
		{
			if(ch=='-')
                {putc('-',Cifa);
                putc('\n',Cifa);
				return 32;}   //32 -
			if(ch=='+')
				{putc('+',Cifa);
                putc('\n',Cifa);
				    return 31;}   // +
		}

	case'<':
	    putc('<',Cifa);
		if((ch=fgetc(FIN))=='=')
		{
			//putc(ch,FOUT);
			putc(ch,Cifa);
                putc('\n',Cifa);
			return 50;//	<=
		}
		else{

	    putc('\n',Cifa);
			ungetc(ch,FIN);
			return 46;//	<
		}
	case'>':
	    putc('>',Cifa);
		if((ch=fgetc(FIN))=='=')
		{
			//putc(ch,FOUT);
			putc(ch,Cifa);
                putc('\n',Cifa);
			return 49; //	>=
		}
		else
		{
		    putc('\n',Cifa);
			ungetc(ch,FIN);
			return 45; //	>
		}
	case'=':
	    putc('=',Cifa);
		if((ch=fgetc(FIN))=='=')
		{
			//putc(ch,FOUT);
			putc(ch,Cifa);
                putc('\n',Cifa);
			return 47;  //	==
		}
		else
		{
		    putc('\n',Cifa);
			ungetc(ch,FIN);
			return 51;       //	51为=
		}
	case'!':
	    putc('!',Cifa);

		if((ch=fgetc(FIN))=='=')
		{
			//putc(ch,FOUT);
			putc(ch,Cifa);
                putc('\n',Cifa);
			return 48; //	!=
		}
		else
		{							//不存在单独的!作为操作符
		    putc('\n',Cifa);
			ungetc(ch,FIN);
			errorline[errornum]=linenum;
			error[errornum++]=2;
		}
	}
	return 0;//出错
}

//分析程序
int program()
{
	int   i;
	char c;
	//printf("%d:\t",linenum);   //输出行数
	sym=getsym();

	if(sym==11)  //数字11表示 int,
	{
		i=sym;
		sym=getsym();
		if(sym==1)  // 标识符 1	内部字符串
		{
			strcpy(name[NumOfC],Word);    //NumOfC符号表内元素的个数  //name存标识符
			sym=getsym();
			if(sym!=39&&sym!=38&&sym!=37)  //（: 37, ；38,  ，:	39
			{
				errorline[errornum]=linenum;
				error[errornum++]=22;
				while((c=fgetc(FIN))!=EOF)
					{
						//putc(c,FOUT);
						if(c=='\n')
						{
							linenum++;
							//printf("%d:\t",linenum);
						}
					}
				//printf("\n");
				return 1;
			}

			if(sym==38||sym==39)  // ；38,  ，:	39
			{
				if( vardefine(i))
				{
					while((c=fgetc(FIN))!=EOF)
					{
						putc(c,FOUT);
						if(c=='\n')
						{
							linenum++;
							//printf("%d:\t",linenum);
						}
					}
					//printf("\n");
					return 1;
				}
			}

			while(sym==37)    //37表示(
			{
				if(returnfunction(i))  //有返回值的函数
				{
					while((c=fgetc(FIN))!=EOF)
					{
						putc(c,FOUT);
						if(c=='\n')
						{
							linenum++;
							//printf("%d:\t",linenum);
						}
					}
					//printf("\n");
					return 1;
				}

				if(sym==11||sym==12)    //11 int 12 char
				{
					sym=getsym();
					if(sym==1)    // 1 标识符
					{
						strcpy(name[NumOfC],Word);
						if((sym=getsym())==37)    //37 (
							continue;
						else
						{
							errorline[errornum]=linenum;
							error[errornum++]=17;
							while((c=fgetc(FIN))!=EOF)
							{
								putc(c,FOUT);
								if(c=='\n')
								{
									linenum++;
									//printf("%d:\t",linenum);
								}
							}
							//printf("\n");
							return 1;
						}
					}
					else
					{
						errorline[errornum]=linenum;
						error[errornum++]=27;
						while((c=fgetc(FIN))!=EOF)
						{
							putc(c,FOUT);
							if(c=='\n')
							{
								linenum++;
								//printf("%d:\t",linenum);
							}
						}
						//printf("\n");
						return 1;
					}
				}
				else
					break;
			}

		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=27;
			while((c=fgetc(FIN))!=EOF)
			{
				putc(c,FOUT);
				if(c=='\n')
				{
					linenum++;
					//printf("%d:\t",linenum);
				}
			}
			//printf("\n");
			return 1;
		}
	}
	if(sym==13)    //13表示 void
	{
		sym=getsym();
		while(sym==1)    //1表示标识符
			if(voidfunction())   //无返回值函数
			{
				while((c=fgetc(FIN))!=EOF)
				{
					putc(c,FOUT);
					if(c=='\n')
					{
						linenum++;
						//printf("%d:\t",linenum);
					}
				}
				//printf("\n");
				return 1;
			}
		if(sym==14)  //14表示 main
		{
			if(mainfunction())  //主函数分析
			{
				while((c=fgetc(FIN))!=EOF)
				{
					putc(c,FOUT);
					if(c=='\n')
					{
						linenum++;
						//printf("%d:\t",linenum);
					}
				}
				//printf("\n");
				return 1;
			}
			else
				return 0;
		}
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=15;
		while((c=fgetc(FIN))!=EOF)
		{
			putc(c,FOUT);
			if(c=='\n')
			{
				linenum++;
				//printf("%d:\t",linenum);
			}
		}
		//printf("\n");
		return 1;
	}
	return 0;
}

//分析变量说明部分
int vardefine(int type)
{
	if(enter(lev,type,0))
		return 1;
	if(sym==39) //，:	39
	{
		if((sym=getsym())==1)   //1 标识符
		{
			strcpy(name[NumOfC],Word);
			sym=getsym();
			return(vardefine(type));  //调用处理int a,b,c....形式
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=27;
			return 1;
		}
	}
	if(sym==38) //；38
	{
		sym=getsym();
		if(sym==12||sym==11)  	// int	11,	char12  处理 int ....; int.....
		{
			int i=sym;
			if((sym=getsym())==1) // 标识符	1 内部字符串
			{
				strcpy(name[NumOfC],Word);
				sym=getsym();
				if(sym==39||sym==38) // , 39  //38 ;
				{
					if(vardefine(i))
						return 1;
				}
				else if(sym==37)   //37 (
					return 0;
				else
				{
					errorline[errornum]=linenum;
					error[errornum++]=22;
					return 1;
				}
			}
			else
			{
				errorline[errornum]=linenum;
				error[errornum++]=27;
				return 1;
			}
		}
		else
			return 0;
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=22;
		return 1;
	}
	return 0;
}

//把变量填入符号表
int enter(int lev,int type,int kind)
{
	if(NumOfC<MAXC-1)         //numofc符号表内的元素个数小于MAXC符号表的长度50
	{
		if(charcheck()&&kind!=3)   //检查参数且 参数类型不是变量
		{
			errorline[errornum]=linenum;
			error[errornum++]=8;
			return 1;
		}
		Char[NumOfC].lev=lev;           //
		Char[NumOfC].kind=kind;         // 0	para(参数)、1	pro(有返回值的函数)2	voidpro(无返回值的函数)
                                        //3	var（变量）4	const（常量）5	main（主函数）
		Char[NumOfC].type=type;         //参数类型 int/char
		Char[NumOfC].name=name[NumOfC];  //符号表里元素
		if(kind==0||kind==3)             // 0参数 3变量
			Char[NumOfC].adr=++numoflev[lev]; //取地址
		else
			Char[NumOfC].adr=oprnum; //指令数

			if(kind==4)  //4 const
			{
				if(type==11)  // 11 int
					Char[NumOfC].uchar.num=num;
				else
					Char[NumOfC].uchar.cischar =cischar;
			}

			NumOfC++;
		}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=7;
		return 1;  //
	}
	return 0;
}

//检查参数
int charcheck()
{
	int i;
	for(i=NumOfC-1;i>=0;i--)
	{
		if(Char[i].lev<lev)   //符号表里的元素
			return 0;
		else
		{
			if(!strcmp(Char[i].name,name[NumOfC]))//
				return 1;
		}
	}
	return 0;
}

//在符号表中查找标识符是否存在
int find()
{
	int i,j=1;
	for(i=NumOfC-1;i>=1;i--)
	{
		if(Char[i].lev<lev)   //
			j=0;
		if(Char[i].kind!=3||j)   //不是变量或是1
			if(!strcmp(Char[i].name,Word))  //在符号表里查找
			{
				return i;
			}//找到返回地址值 adr
	}
	return 0;
}

//语句列 函数里面的语句分析
int statementArray()
{
	int a=0;
	while(sym==1||sym==15||sym==18||
		sym==40||sym==19||sym==20||sym==21)   //1为标识符 15为if 18为while 40为{ 19scanf 20为printf 21为return
	{
		if((a=statement())==1)
			return 1;
	}
	if(sym==38)   //38 ;
		sym=getsym();
	return 0;
}

//语句
int statement()
{
	int a;
	switch(sym){

	case 15:    //15 if
		return(ifstatement());
	case 18:   //18 while
		return(whilestatement());
	case 19:   //19 scanf
		return(scanfstatement());
	case 20:   //20 printf
		return(printfstatement());

	case 1:                              //1 标识符
		if((a=find())==0)
			   return 1;
		sym=getsym();
		if(Char[a].kind==0||Char[a].kind==3)  //0为参数 3为变量
		{
			if(sym==51)         //51为 =
				return(assignstatement(a));  //赋值语句分析
			else
			{
				errorline[errornum]=linenum;
				error[errornum++]=9;
				return 1;
			}
		}
		else if(Char[a].kind==1||Char[a].kind==2)  //1 you fanhuizhi 2 wu fanhuizhi hanshu
		{
			if(sym==37)   //37 (
			{
				if(call(a))  // hanshu diao yong
					return 1;
				if(sym==38)    //38 ;
					sym=getsym();
				else
				{
					errorline[errornum]=linenum;
					error[errornum++]=22;
					return 1;
				}
				return 0;
			}
			else
			{
				errorline[errornum]=linenum;
				error[errornum++]=17;
				return 1;
			}
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=10;
			return 1;
		}
	case 40:    //40 {
		sym=getsym();
		if((a=statementArray())==1)
			return 1;
		if(sym!=35)  //35  }
		{
			errorline[errornum]=linenum;
			error[errornum++]=12;
			return 1;
		}
		sym=getsym();
		return a;
	case 38:   //38  ;
		sym=getsym();
		return 1;
	default:
		return 1;
	}
	return 1;
}

//表达式
int expression()
{
	bool anti=false;
	if(sym==31||sym==32)  //带有符号＋、－  31表示+ ,32表示-
	{
		if(sym==32)   //32 -
		{
			anti=true;//为负数
		}
		sym=getsym();
		gsym[sy]=sym;
		sy++;
	}

	if(term())  //项分析
	{
		return 1;
	}

	if(anti)   //false
	{
		if(gen(15,adr[ad],0,adr[ad],1))
		{
			return 1;
		}
	}

	while(sym==31||sym==32)  // 31 + 32 -
	{
		int b,c,d=oprnum;
		typeofexp2=typeofexp1;
		int a=sym;
		sym=getsym();
		b=sym;
		gsym[sy]=sym;
		c=sy;
		sy++;
		if(sym==37)
		{
			int k;
			k=adr[ad-1];
			adr1[A]=k;
			A++;
		}
		if(term())
			return 1;

		   if(b==1&&gsym[c-2]==1&&gsym[c-3]!=31&&gsym[c-3]!=32&&gsym[c-3]!=33&&gsym[c-3]!=34)
		   {
			   int temp1=newTemp();
			  if(gen(a-21,adr[ad-2],adr[ad-1],temp1,0))
		          return 1;
		   }
		   if(b==2&&gsym[c-2]==1&&gsym[c-3]!=31&&gsym[c-3]!=32&&gsym[c-3]!=33&&gsym[c-3]!=34)
		   {
			   int temp2=newTemp();
			  if(gen(a-21,adr[ad-1],num,temp2,1))
		          return 1;
		   }
		   if((b==37||(b==1&&(gsym[c+1]==33||gsym[c+1]==34)))&&gsym[c-2]==1&&gsym[c-3]!=31&&gsym[c-3]!=32&&gsym[c-3]!=33&&gsym[c-3]!=34)
		   {
			   int temp3=newTemp();
			   if(gen(a-21,adr1[A-1],opra[oprnum-1].result,temp3,0))
				   A--;
				   return 1;
		   }
		   if(b=1&&(gsym[c-2]==36||(gsym[c-2]==1&&(gsym[c-3]==33||gsym[c-3]==34||gsym[c-3]==31||gsym[c-3]==32))))
		   {
			   int temp4=newTemp();
			   if(gen(a-21,opra[oprnum-1].result,adr[ad-1],temp4,0))
				   return 1;
		   }

	   if((b==37&&gsym[c-2]==36)||(gsym[c-2]==1&&(gsym[c-3]==33||gsym[c-3]==34)&&b==1&&(gsym[c+1]==33||gsym[c+1]==34)))
	   {
		   int temp5=newTemp();
		   if(gen(a-21,opra[d-2].result,opra[oprnum-1].result,temp5,0))
			   return 1;
	   }

		if(typeofexp2>typeofexp1)
			typeofexp1=typeofexp2;
	}

	return 0;
}

//项
int term()
{
	int b,c;
	int d;
	if(factor())      //因子分析
		return 1;
	while(sym==33||sym==34)//×或÷
	{
		d=oprnum;
		d++;
		typeofexp2=typeofexp1;
		int a=sym;
		sym=getsym();
		b=sym;
		gsym[sy]=sym;
		c=sy;
		sy++;
		if(factor())
			return 1;

		  if(b==1&&gsym[c-2]==1&&gsym[c-3]!=33&&gsym[c-3]!=34)
		   {
			 int temp1=newTemp();
		     if(gen(a-20,adr[ad-2],adr[ad-1],temp1,0))  //
			 return 1;
		   }
		  if(gsym[c-2]==1&&(b==37||(b==1&&gsym[c+1]==37))&&(gsym[c-3]!=33||gsym[c-3]!=34))
		   {
			 int temp2=newTemp();
			 if(gen(a-20,A,opra[oprnum-1].result,temp2,0))
				return 1;
		   }

		  if((gsym[c-2]==36&&b==1)||(gsym[c-2]==1&&(gsym[c-3]==33||gsym[c-3]==34)&&b==1))
		  {
			  int temp3=newTemp();
			  if(gen(a-20,opra[d-2].result,adr[ad-1],temp3,0))
				return 1;
		   }

		if(b==37&&gsym[c-2]==36)
		{
		  int temp4=newTemp();
		  if(gen(a-20,opra[d-2].result,opra[oprnum-1].result,temp4,0))
			 return 1;
		}


		if(typeofexp2>typeofexp1,oprnum)
			typeofexp1=typeofexp2;
	}
	return 0;
}

//因子
int factor()
{
	//typeofexp1=11;  //11表示 int
	if(sym==1)     //是标识符
	{
		int a=find();    ///在符号表中查找标识符是否存在 存在返回地址值
		adr[ad]=a;
		ad++;
		if(a==0)
		{
		 	errorline[errornum]=linenum;
		 	error[errornum++]=20;      //标识符未定义
			return 1;
		}

		else if(Char[a].kind==0||Char[a].kind==3)  // 3 变量
		{
			sym=getsym();
			gsym[sy]=sym;
		    sy++;
		}
		else if(Char[a].kind==2)   //无返回值的函数
		{
			if((sym=getsym())==37)//(
			{
				if(call(a))
					return 1;
			}
			else
			{
				errorline[errornum]=linenum;
				error[errornum++]=17;
				return 1;
			}
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=11;
			return 1;
		}
		return 0;
	}
	else if(sym==37)//(
	{
		sym=getsym();
		gsym[sy]=sym;
		sy++;
		if(expression())
			return 1;
		if(sym!=36)//)
		{
			errorline[errornum]=linenum;
			error[errornum++]=18;
			return 1;
		}
		else
			sym=getsym();
		    gsym[sy]=sym;
		    sy++;
		return 0;
	}
	else if(sym==2)  // 2为整数
	{
		sym=getsym();
		gsym[sy]=sym;
		sy++;
		return 0;
	}

	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=6;
		return 1;
	}
	return 0;
}


//子函数调用
int call(int a)
{
	int i;
	if(gen(23,Char[a].type,0,oprnum,1)) //23 CAL hanshu diaoyong
		return 1;
	sym=getsym();

	for(i=a+1;i<MAXC;i++)
	{
		if(Char[i].kind!=3)
			break;
		if(expression())
			return 1;
		if(sym==39)  // 39 ,
			sym=getsym();
		else
			break;
	}
	if(Char[i].kind!=3&&sym!=36||Char[i+1].kind==3&&sym==36)//)
	{
		errorline[errornum]=linenum;
		error[errornum++]=16;
		return 1;
	}

	if(gen(24,0,0,Char[a].adr,3))//24 MKF kaishizhixing hanshu
		return 1;

	if(sym==36)  // 36 )
	{
		sym=getsym();
		return 0;
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=18;
		return 1;
	}
	return 0;
}

//有返回值的函数
int returnfunction( int type)
{
	Isret=0;
	if(enter(lev,type,2))   // 变量填入符号表 2表示是整数
		return 1;
	Char[NumOfC-1].adr=oprnum;
	sym=getsym();
	while(sym==11||sym==12)   //11表示是 int , 12表示是char
	{
		int i=sym;
		if((sym=getsym())==1)   //1表示内部标识符
		{
			strcpy(name[NumOfC],Word);    //此时Word表里的值和name相比较
			if(enter(lev+1,i,3))
				return 1;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=27;
			return 1;
		}
		if((sym=getsym())!=39)   //39表示是: ,
			break;
		sym=getsym();
		if(sym!=11)  // int
		{
			errorline[errornum]=linenum;
			error[errornum++]=4;
			return 1;
		}
	}
	if(sym==36)  //36 )
	{
		if((sym=getsym())==40)   //40表示是{
		{
			lev++;
			sym=getsym();

			if(sym==11)  // int char
			{
				int i=sym;
				if((sym=getsym())==1)  // 标识符 1
				{
					strcpy(name[NumOfC],Word);
					sym=getsym();
					if(sym!=38&&sym!=39)  // 38 ;  39 ,
					{
						errorline[errornum]=linenum;
						error[errornum++]=22;
						return 1;
					}
					if(sym==38||sym==39)  //38 ; 39 ,
					{
						if( vardefine(i))
							return 1;
					}
				}
				else
				{
					errorline[errornum]=linenum;
					error[errornum++]=27;
					return 1;
				}
			}
			//if(gen(9,0,0,numoflev[lev]))  //9表示分配数据区  numoflev表示分配大小
			//	return 1;
			if(statementArray())
				return 1;

		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=23;
			return 1;
		}
		if(sym==35)  //35 }
		{
			sym=getsym();
			backout();
			lev--;
			if(gen(7,0,0,0,3)) //7 EXF cun shu hanshu fanhuizhi jieshu hanshu diaoyong
				return 1;
			return 0;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=12;
			return 1;
		}
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=18;
		return 1;
	}
}

//无返回值的函数定义语句
int voidfunction()
{
	Isret=1;
	strcpy(name[NumOfC],Word);
	if(enter(lev,0,1))
		return 1;
	if((sym=getsym())!=37)  //37（
	{
		errorline[errornum]=linenum;
		error[errornum++]=17;
		return 1;
	}
	sym=getsym();
	while(sym==11)    // 11 int
	{
		int i=sym;
		if((sym=getsym())==1)     //1 标识符
		{
			strcpy(name[NumOfC],Word);
			if(enter(lev+1,i,3))
				return 1;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=27;
			return 1;
		}
		if((sym=getsym())!=39)     //39 ,
			break;
		sym=getsym();

		if(sym!=11&&sym!=12)      // 11 int 12char
		{
			errorline[errornum]=linenum;
			error[errornum++]=4;
			return 1;
		}
	}
	if(sym==36)  // 36 )
	{
		if((sym=getsym())==40)  //40  {
		{
			lev++;
			sym=getsym();

			if(sym==11||sym==12)  // int char
			{
				int i=sym;
				if((sym=getsym())==1)  //标示符
				{
					strcpy(name[NumOfC],Word);
					sym=getsym();
					if(sym!=38&&sym!=39)  // 39 ，
					{
						errorline[errornum]=linenum;
						error[errornum++]=22;
						return 1;
					}
					if(sym==38||sym==39)   // 38 ； 39 ，
						if( vardefine(i))
							return 1;
				}
				else{
					errorline[errornum]=linenum;
					error[errornum++]=27;
					return 1;
				}
			}
			//if(gen(9,0,numoflev[lev],oprnum))  //INF
			//	return 1;
			if(statementArray())
				return 1;

		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=23;
			return 1;
		}
		if(sym==35)   // 35  }
		{
			backout();
			lev--;
			if((sym=getsym())==13)  // 13  void
				if((sym=getsym())==1)
					return(voidfunction());

			if(gen(25,0,0,0,3)) //25 EKP wu fanhuizhi hanshu diaoyong jieshu
				return 1;

			return 0;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=12;
			return 1;
		}
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=18;
		return 1;
	}
	return 0;
}

//赋值语句
int assignstatement(int a)
{
	int adress=a;
	adr[ad]=a;
	ad++;
	sym=getsym();
	gsym[sy]=sym;
	sy++;
	if(expression()) //表达式分析
		return 1;
	if(typeofexp1!=Char[a].type)//类型不匹配
	{
		errorline[errornum]=linenum;
		error[errornum++]=28;
		return 1;
	}

    if(gsym[sy-2]!=2)
	 {
		 if(gen(26,0,opra[oprnum-1].result,adress,2))  //= fuzhi
		   return 1;
	 }

	else
	{
       if(gen(26,0,num,adr[ad-1],3))  //fuzhi a=1
			return 1;
	}

	if(sym!=38)//缺少;
	{
		errorline[errornum]=linenum;
		error[errornum++]=22;
		return 1;
	}
  	sym=getsym();
	return 0;
}

//if条件语句
int ifstatement()
{
	int oprnum1,oprnum2,a;
	if((sym=getsym())==37)     //37表示(
	{
		sym=getsym();
     		if(expression())      //表达式分析
			return 1;
		a=typeofexp1;
		if(sym==45||sym==46||sym==47||sym==48||sym==49||sym==50) //45为> 46为< 47为== 48为!= 49为>= 50为<=
		{
			int b=sym;
			sym=getsym();
			int d=sym;
			if(expression())
				return 1;
            if(d==2)
			{
				int temp1=newTemp();
				if(gen(b-29,adr[ad-1],num,temp1,1))
				   return 1;
			}
			else
			{
				int temp2=newTemp();
				if(gen(b-29,adr[ad-2],adr[ad-1],temp2,0))
			    	return 1;
			}
		}
		oprnum1=oprnum;
		if(gen(4,0,0,oprnum,3)) //JMP
			return 1;
		if(sym!=36)  // 36 )
		{
			errorline[errornum]=linenum;
			error[errornum++]=18;
			return 1;
		}
		sym=getsym();
		if(statement()==1)
			return 1;
		if(sym==16)   // 16 else
		{
			oprnum2=oprnum;
			if(gen(4,0,0,0,3)) //JMP
				return 1;
			opra[oprnum1].result=oprnum; //huitian if
			sym=getsym();
			if(statement()==1)
				return 1;
			opra[oprnum2].result=oprnum; //huitian else
		}
		else opra[oprnum1].result=oprnum; //huitian if

	}
	else{
		errorline[errornum]=linenum;
		error[errornum++]=17;
		return 1;
	}
	return 0;
}

//while循环语句
int whilestatement()
{
	int oprnum1=oprnum,oprnum2,oprnum4[MAXCASE],a,j=0;
	if((sym=getsym())==37)  //37 (
	{
		sym=getsym();
		if(expression())
			return 1;
		a=typeofexp1;

		if(sym==45||sym==46||sym==47||sym==48||sym==49||sym==50) //45 > 46 < 47 == 48 != 49 >= 50 <=
		{
			int b=sym;
			sym=getsym();
			int d=sym;
			if(expression())
				return 1;
			if(d==2)
			{
				int temp1=newTemp();
				if(gen(b-29,adr[ad-1],num,temp1,1))
				   return 1;
			}
			else
			{
				int temp2=newTemp();
				if(gen(b-29,adr[ad-2],adr[ad-1],temp2,0))
			    	return 1;
			}
		}
		oprnum2=oprnum;  //jixia  JMC zhilingshu
		if(gen(4,0,0,0,3)) // JMP
			return 1;
		if(sym!=36)
		{
			errorline[errornum]=linenum;
			error[errornum++]=18;
			return 1;
		}
		sym=getsym();
		if(statement())
			return 1;

		if(gen(4,0,0,oprnum1,3))  //tiaodao while panduan
			return 1;
		opra[oprnum2].result=oprnum; // huitian JMC

		for(a=0;a<j;a++){
			opra[oprnum4[a]].op2=oprnum;  //
		}
	}
	else {
		errorline[errornum]=linenum;
		error[errornum++]=17;
		return 1;
	}
	return 0;
}

//scanf语句
int scanfstatement()
{
	int a;
	sym=getsym();
	if(sym==37)   // (
	{
		sym=getsym();
		if(sym!=1)
		{
			errorline[errornum]=linenum;
			error[errornum++]=27;
			return 1;
		}
		a=find();
		if(a==0)
		{
			errorline[errornum]=linenum;
			error[errornum++]=20;
			return 1;
		}
		if(Char[a].kind==0||Char[a].kind==3)
		{
			if(gen(6,0,0,adr[ad-1],3))
				return 1;

		}
		if((sym=getsym())!=36)  // 36 )
		{
			errorline[errornum]=linenum;
			error[errornum++]=18;
			return 1;
		}
		if((sym=getsym())==38)  // 38 ;
		{
			sym=getsym();
			return 0;
		}
		else {
			errorline[errornum]=linenum;
			error[errornum++]=22;
			return 1;
		}

	}
	errorline[errornum]=linenum;
	error[errornum++]=17;
	return 0;
}

//printf语句
int printfstatement()
{
	int  i,oprnum1[MAXS],oprnum2,siss[MAXS];
	int	j=0;
	int	a=0;

	if((sym=getsym())==37)  //37 (
	{
		if((sym=getsym())==5)  //5 字符串
		{
			for(i=0;i<strlen(sisstring);i++)
			{
				if(sisstring[i]=='\\')
				{
					if(sisstring[++i]=='n')
					{
						if(gen(8,5,0,0,3))    //shuchu huiche
							return 1;
					}
					else if(sisstring[i]=='t')  //shuchu huiche
					{
						if(gen(8,6,0,0,3))
							return 1;
					}
					else
					{
						errorline[errornum]=linenum;
						error[errornum++]=21;
						return 1;
					}
				}
				else if(sisstring[i]!='%')
				{

					if(gen(8,4,0,0,3))
						return 1;
				}
				else
				{
					oprnum1[j]=oprnum; // jixia zhilingshu
					if(gen(4,0,0,0,3)) // wutiao tiao jiaozhan
						return 1;
					siss[j++]=sisstring[++i];
				}

			}
			oprnum2=oprnum; //jixia cishi zhilingshu
			if(j){
				if(gen(4,0,0,0,3))     //4 JMP wutiaojian
					return 1;
			}
		}
		sym=getsym();
		for(i=0;i<j;i++)
		{
			a=1;
			if(sym!=39)   //39 ,
			{
				errorline[errornum]=linenum;
				error[errornum++]=13;
				return 1;
			}
			sym=getsym();
			opra[oprnum1[i]].result=oprnum; // huitian result
			if(expression())  //biaoda shi fenxi
				return 1;
			if(siss[i]=='d') //shu chu zhengxing
			{
				if(gen(8,adr[ad-1],0,0,1))
					return 1;
			}
			else if(siss[i]=='f')
			{
				if(gen(8,3,0,0,3))
					return 1;
			}
			else if(siss[i]=='c') // shuchu zifu
			{
				if(gen(8,4,0,0,3))
					return 1;
			}
			else
			{
				errorline[errornum]=linenum;
				error[errornum++]=21;
				return 1;
			}
			if(gen(4,0,0,oprnum1[i]+1,3))//you tiaojian tiaozhan
				return 1;
		}
		if(j)
			opra[oprnum2].result=oprnum; //huitian result
		if(sym==36)
		{
			if((sym=getsym())==38)  // 38 ;
				sym=getsym();
			else{
				errorline[errornum]=linenum;
				error[errornum++]=22;
				return 1;
			}
		}
		else{
			errorline[errornum]=linenum;
			error[errornum++]=18;
			return 1;
		}
	}
	else{
		errorline[errornum]=linenum;
		error[errornum++]=17;
		return 1;
	}
	return 0;
}

//return语句
int returnstatement()
{
	if((sym=getsym())==37)  // 37 (
	{
		gsym[sy]=sym;
		sy++;
		sym=getsym();
		if(expression())
			return 1;
		if(gsym[sy-3]==37)
		{
			if(gsym[sy-2]==1)
			{
			   if(gen(7,adr[ad-1],0,oprnum,1))  //EXF  cunshu hanshu fanhui jieshu hanshu diaoyong
			     return 1;
			}
			else
			{
				if(gen(7,num,0,oprnum,3))
					return 1;
			}
		}
		else
		{
			if(gen(7,oprnum-1,0,oprnum,1))  //EXF  cunshu hanshu fanhui jieshu hanshu diaoyong
			  return 1;
		}

		if(sym==36)
		{
			if((sym=getsym())==38)   //38 ;
			{
				sym=getsym();
				return 0;
			}
			else{
				errorline[errornum]=linenum;
				error[errornum++]=22;
				return 1;
			}
		}
		else{
			errorline[errornum]=linenum;
			error[errornum++]=18;
			return 1;
		}
	}
	if(sym==38)   // 38 ;
	{
		if(gen(7,11,0,0,3)) //EXF
			return 1;

		sym=getsym();
		return 0;
	}
	errorline[errornum]=linenum;
	error[errornum++]=22;

	return 1;
}

//main函数处理
int mainfunction()
{
	Isret=1;
	start=oprnum;  //
	Isret=1;
	strcpy(name[NumOfC],Word);  //Word里面的标识符存到name[]
	if(enter(lev,0,5))    // 0 kind shi canshu, 5 is 'main'
		return 1;
	Char[NumOfC-1].adr=oprnum;  //adress is oprate number
	if((sym=getsym())!=37) //37为(
	{
		errorline[errornum]=linenum;
		error[errornum++]=17;
		return 1;
	}
	sym=getsym();
	while(sym==11)  //11 int
	{
		int i=sym;
		if((sym=getsym())==1)
		{
			strcpy(name[NumOfC],Word);
			if(enter(lev+1,i,3))
				return 1;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=27;
			return 1;
		}
		if((sym=getsym())!=39)  // 39 ,
			break;
		sym=getsym();

		if(sym!=11)  // 11 int
		{
			errorline[errornum]=linenum;
			error[errornum++]=4;
			return 1;
		}
	}
	if(sym==36)   //36 )
	{
		if((sym=getsym())==40)  //40 {
		{
			lev++;
			sym=getsym();

			if(sym==11)   //11 int
			{
				int i=sym;
				if((sym=getsym())==1)
				{
					strcpy(name[NumOfC],Word);
					sym=getsym();
					if(sym!=39&&sym!=38)  // 38 ;
					{
						errorline[errornum]=linenum;
						error[errornum++]=22;
						return 1;
					}
					if(sym==39||sym==38)  // 38 ; 39 ,
						if( vardefine(i))  //变量分析
							return 1;

				}
				else{
					errorline[errornum]=linenum;
					error[errornum++]=27;
					return 1;
				}
			}
			//if(gen(9,0,numoflev[lev],oprnum))  //9 分配numoflev[]个数据区
			//	return 1;
			if(statementArray())  //语句列分析
				return 1;

		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=23;
			return 1;		}
		if(sym==35)   // 35  }
		{
			backout();
			lev--;
			Isret=true;

			return 0;
		}
		else
		{
			errorline[errornum]=linenum;
			error[errornum++]=12;
			return 1;
		}
	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=18;
		return 1;
	}
	Isret=true;
	return 0;

}


//从符号表中退出不用的符号
int backout()
{
	int i;
	for(i=NumOfC-1;i>=1;i--)
	{
		if(Char[i].kind==3)
			break;
		if(Char[i].kind==0||Char[i].kind==4)
			NumOfC--;
		numoflev[lev]=1;
	}
	return 0;
}

int T=0;
int p;
int temp[50];
int newTemp()
{

	p=T+1;
	temp[T++]=p;
	return T;

}


//产生一条表达式
int gen(int op,int op1,int op2, int result, int mode)
{
	if(oprnum<MAXOP)     // MAXOP最大的操作指令条数
	{
		opra[oprnum].op=op;    //操作码
		opra[oprnum].op1=op1;  //变量类型
		opra[oprnum].op2=op2;     // 变量值
		opra[oprnum].result=result; //jie guo
		opra[oprnum].cas=mode;
		oprnum++;

	}
	else
	{
		errorline[errornum]=linenum;
		error[errornum++]=19;
		return 1;
	}
	return 0;
}
char a1[100];
char a2[100];
void fun1(int ty,int op)
{
    int tp=0;
    if (ty==0) a1[tp++]='t';
    char tmp[100];
    int tt=0;
    while (op)
    {
        tmp[tt++]='0'+op%10;
        op/=10;
    }
    for (int i=0;i<tt/2;i++)
    {
        char y=tmp[tt-1-i];
        tmp[tt-1-i]=tmp[i];
        tmp[i]=y;
    }
    for (int i=0;i<tt;i++)
    {
        a1[tp++]=tmp[i];
    }
    a1[tp++]='\0';
}
void fun2(int ty,int op)
{
    int tp=0;
    if (ty==0) a2[tp++]='t';
    char tmp[100];
    int tt=0;
    while (op)
    {
        tmp[tt++]='0'+op%10;
        op/=10;
    }
    for (int i=0;i<tt/2;i++)
    {
        char y=tmp[tt-1-i];
        tmp[tt-1-i]=tmp[i];
        tmp[i]=y;
    }
    for (int i=0;i<tt;i++)
    {
        a2[tp++]=tmp[i];
    }
    a2[tp++]='\0';
}
void compile()
{

    for(int i=0;i<=oprnum;i++)
    {
        fprintf (huibian,"f%d:  ",i);
        if (i==oprnum)
        {
            fprintf (huibian,"\n");
            break;
        }
        fun1(opra[i].cas/2,opra[i].op1);
        fun2(opra[i].cas%2,opra[i].op2);
        if (opra[i].op==10)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      ADD AX BX\n");
            fprintf (huibian,"      MOV t%d AX\n",opra[i].result);
        }
        else if (opra[i].op==11)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      SUB AX BX\n");
            fprintf (huibian,"      MOV t%d AX\n",opra[i].result);
        }
        else if (opra[i].op==13)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      MUL AX BX\n");
            fprintf (huibian,"      MOV t%d AX\n",opra[i].result);
        }
        else if (opra[i].op==14)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      DIV AX BX\n");
            fprintf (huibian,"      MOV t%d AX\n",opra[i].result);
        }
        else if (opra[i].op==26)
        {
            fprintf (huibian,"MOV AX %s\n",a2);
            fprintf (huibian,"      MOV t%d AX\n",opra[i].result);
        }
        else if (opra[i].op==17)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      JNL f%d\n",opra[i+1].result);
            i++;
        }
        else if (opra[i].op==16)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      JNG f%d\n",opra[i+1].result);
            i++;
        }
        else if (opra[i].op==19)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      JE f%d\n",opra[i+1].result);
            i++;
        }
        else if (opra[i].op==18)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      JNE f%d\n",opra[i+1].result);
            i++;
        }
        else if (opra[i].op==21)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      JG f%d\n",opra[i+1].result);
            i++;
        }
        else if (opra[i].op==20)
        {
            fprintf (huibian,"MOV AX %s\n",a1);
            fprintf (huibian,"      MOV BX %s\n",a2);
            fprintf (huibian,"      JL f%d\n",opra[i+1].result);
            i++;
        }
        else if (opra[i].op==4)
        {
            fprintf (huibian,"JMP   f%d\n",opra[i].result);
        }
        else if (opra[i].op==8)
        {
            if (opra[i].op1==5&&opra[i].op/2==1)
            {
                fprintf (huibian,"MOV ah 02h\n");
                fprintf (huibian,"      MOV dl 0dh\n");
                fprintf (huibian,"      int 21h\n");
                fprintf (huibian,"      MOV ah 02h\n");
                fprintf (huibian,"      MOV dl 0ah\n");
                fprintf (huibian,"      int 21h\n");
            }
            else if (opra[i].result/2==0)
            {
                fprintf (huibian,"MOV AX %s\n",a1);
                fprintf (huibian,"      WRI AX\n");
            }
        }
    }
}

//主函数
int main()
{
	int c=0;
	init();

	FIN=fopen(fp,"r");
    Cifa=fopen("./work/word.txt","wt+");
	//printf("\n源程序:\n");
    FOUT=stdout;

	for(int j=0;j<MAXLEV;j++)   //DEFINE:MAXLEV符号表的最大容量50
		numoflev[j]=1;          //各层的表中变量数
    Eorro=fopen("./work/error.txt","wt+");
	if(program()){
		int i=errornum;
		for(errornum=0;errornum<i;errornum++)
		{
			error_msg(error[errornum]);
		}
		return 0;
	}
	else
    {
        fprintf(Eorro,"success\n");
    }
	fclose(Eorro);
	fclose(Cifa);
	int i;
	EquFout=fopen("./work/four.txt","w");
    for(i=0;i<oprnum;i++)
	 {

		 fprintf(EquFout,"%d\t%s\t%d\t%d\t%d\t%d\n",i,oprate[opra[i].op],
			 opra[i].op, opra[i].op1,opra[i].op2,opra[i].result);
	 }
	fclose(EquFout);


    huibian=fopen("./work/huibian.txt","w");

    compile();
    fclose(huibian);
	return 0;

}
