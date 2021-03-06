#include "userdef.h"


/* ********************************************************************* */
/* additions to userdef.h						*/

#define MAXSTR 80
#define SOURCE 0
#define DEST 1
/* ********************************************************************* */



/* ******************************************************************* */
/* ROUTINE:instrmatch{}						       */
/* ******************************************************************* */
instrmatch(str)
char *str;
{
extern OPSTRUCTURE syntax1[];
extern CONDITIONALS typecondition[];
extern CONTROLREG CRreg[];
extern SIZETYPES sizefield[];
extern TYPES typebitfield[];
extern EAALLOWED_TYPE EAtype[];
extern condition;
extern int instructnum;
extern char sizechar[];
extern error;
/* ******************************** */
/* VARIABLES FOR SOURCE/DESTINATION */
extern EASPEC EA[];
extern int MIPOST;
int SorD;
char allowableEA[13];
/* ******************************** */
int maxentry,i,j,k,l,cr1,cr2;
int int1,int2;
int a,Bmatch,instructmatch;
int bitfieldvar;
char table_syntax[MAXLINE];
char teststring[MAXLINE];
char teststringfc[MAXLINE];
/* ******************************** */

stripwhite(str);

instructmatch=0;
for (i=0;i<=MAXINST;i++)	/* this uses that value to tell how 
					   many entries are in that table */
{
	condition=-1;
	Bmatch=FALSE;
	for(j=0;(syntax1[i].name[j] == str[j]) && (syntax1[i].name[j] !=ENDSTR);j++)
		;
	if(syntax1[i].name[j]=='~') 
		{
		if(i==59 || i==60 || i==66 || i==150 || i==151)
			{
			if(i!=66)
				{
				for(k=0;k<=1;k++)
				{
				if((str[j] == typecondition[k].condition[0]) || (str[j] == (typecondition[k].condition[0] + 0x20)))
					{
					j++;
					condition=k;
					break;
					}
				}
				}
			if(condition!=k)
				{
				for(k=2;k<=15;k++)
				{
				if((str[j]==typecondition[k].condition[0] || str[j]==typecondition[k].condition[0]+0x20) && (str[j+1]==typecondition[k].condition[1] || str[j+1]==typecondition[k].condition[1]+0x20))
					{
					j++;
					j++;
					condition=k;
					break;
					}
				}
				}
			/*NOTE: j now points to the '.' position of the syntax*/
			if((str[j]==' ' || str[j]=='\t' || str[j]==ENDSTR) && condition !=-1)
			{
				Bmatch=TRUE;
				sizechar[0]='~';
				sizechar[1]=ENDSTR;
			}
			else if(str[j]=='.' && instructnum!=150 && str[j+1]!='~' &&			     	condition != -1 
 && 			       (str[j+1]== sizefield[syntax1[i].sizestr].size[0] ||
				str[j+1]== sizefield[syntax1[i].sizestr].size[1] ||
				str[j+1]== sizefield[syntax1[i].sizestr].size[2] ||
				str[j+1]== sizefield[syntax1[i].sizestr].size[3] ||
				str[j+1]== sizefield[syntax1[i].sizestr].size[4] ||
				str[j+1]== sizefield[syntax1[i].sizestr].size[5] ||
				str[j+1]== sizefield[syntax1[i].sizestr].size[6]))
			{
				Bmatch=TRUE;
				sizechar[0]=str[j+1];
				sizechar[1]=ENDSTR;
			}
			j=0;/* ie. a point where there will never be a '.' or
				ENDSTR so that it won't match the next 2 if 
				statements				*/
			}/* end if 59 etc... */
#if (DEVICE >= 68020)
		else /* 156,157,158,159 */
			{
			for(k=0;k<=7;k++)
			{
				if((str[j]==typebitfield[k].type[0] || str[j]==typebitfield[k].type[0]+0x20) &&	(str[j+1]==typebitfield[k].type[1] || str[j+1]==typebitfield[k].type[1]+0x20) && (str[j+2]==typebitfield[k].type[2] || str[j+2]==typebitfield[k].type[2]+0x20))
					{
					j++;
					j++;
					j++;
					condition=k;
				if(i==156)
					{
					switch(condition)
						{
						case 0:
							i=157;
							break;
						case 1:
						case 3:
						case 5:
							i=158;
							break;
						case 2:
						case 4:
						case 6:
							i=156;
							break;
						case 7:
							i=159;
							break;
						}
					}
					else
					{
						i=160;
						break;
					}
					if(k!=1 && k!=3)
						break;
					if(condition==k && (k==1 || k==3))
						{
						if((str[j]==typebitfield[k].type[3]) || (str[j]==typebitfield[k].type[3]+0x20))
							{
							shiftarg(str,1);/* account for char 4 */
							break;
							}
						else
							{
							i=156;
							j=j-3;
							}
						}
					}
			}/* end for  */
			}/* end else */
#endif
		}/* end conditional check */
	if((syntax1[i].name[j]=='.' || syntax1[i].name[j]==ENDSTR) && 
		       (str[j]==' ' || 
		        str[j]=='\t' || 
			str[j]==ENDSTR))
		{
		Bmatch=TRUE;
		sizechar[0]='~';
		sizechar[1]=ENDSTR;
		}
	else if(syntax1[i].name[j]==ENDSTR && syntax1[i].name[j-1]=='.' && 
		       (str[j]== sizefield[syntax1[i].sizestr].size[0] ||
			str[j]== sizefield[syntax1[i].sizestr].size[1] ||
			str[j]== sizefield[syntax1[i].sizestr].size[2] ||
			str[j]== sizefield[syntax1[i].sizestr].size[3] ||
			str[j]== sizefield[syntax1[i].sizestr].size[4] ||
			str[j]== sizefield[syntax1[i].sizestr].size[5] ||
			str[j]== sizefield[syntax1[i].sizestr].size[6]))
		{
		Bmatch=TRUE;
		sizechar[0]=str[j];
		sizechar[1]=ENDSTR;
		}
	if(Bmatch==1)
	{
		instructmatch=i;
		for(l=0;str[l]!=ENDSTR;l++)
			{
			teststring[l]=str[l];
			teststringfc[l]=str[l];
			}
		teststring[l]=ENDSTR;
		teststringfc[l]=ENDSTR;


/* ************************SETUP SOURCE/DEST**************************** */
		l=0;
	for(int1=0;int1<=1;int1++)
	{
		if(int1 == 0)
			int2=syntax1[instructmatch].source;
		else
			int2=syntax1[instructmatch].dest;

		if(int1 == 1)
			table_syntax[l++]=',';

		if(EAtype[int2].allowableEA > 0xfff)
		{
			switch(EAtype[int2].allowableEA >> 12)
			{
				case 1:/* 'C' for CCR */
					table_syntax[l++]='C';
					break;
				case 2:/* 'S' for SR */
					table_syntax[l++]='S';
					break;
				case 3:/* 'R' for REG LIST */
					table_syntax[l++]='R';
					break;
				case 4:/* 'N' for NONE */
					if(int1==0)
						table_syntax[l++]='N';
					break;
				case 5:/* 'U' for USP */
					table_syntax[l++]='U';
					break;
				case 6:/* 'Z' for CONTROL REG */
					table_syntax[l++]='Z';
					break;
				case 7:/* 'Y' for D_:D_ FORMAT */
					table_syntax[l++]='Y';
					break;
				case 8:/* 'Y,Y,W'where W is same as Y except A_ or D_ */
					table_syntax[l++]='Y';
					table_syntax[l++]=',';
					table_syntax[l++]='Y';
					table_syntax[l++]=',';
					table_syntax[l++]='W';
					break;
#if (DEVICE==68030)
				case 9:/* <fc>,#mask */
					table_syntax[l++]='F';
					table_syntax[l++]=',';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],8);
					l=l+12;
					break;
#endif
				case 10:/* '4,4,8' for CCR */
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],4);
					l=l+12;
					table_syntax[l++]=',';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],4);
					l=l+12;
					table_syntax[l++]=',';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],8);
					l=l+12;
					break;
				case 11:/* 'X_______1000' for BITFIELD */
					table_syntax[l++]='X';
					syntaxsetup(&table_syntax[l],15);
					l=l+12;
					break;
				case 12:/* 'X_______1110' for BITFIELD */
					table_syntax[l++]='X';
					syntaxsetup(&table_syntax[l],16);
					l=l+12;
					break;
				case 13:/* 'QE001111111000' for CAS */
					table_syntax[l++]='Q';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],12);
					l=l+12;
					break;
				case 14:/* 'QE000000000001' for UNPK/PACK */
					table_syntax[l++]='Q';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],8);
					l=l+12;
					break;
				case 15:/* '13,13'  */
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],13);
					l=l+12;
					table_syntax[l++]=',';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],13);
					l=l+12;
					break;
				case 16:/* '4,4'  */
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],4);
					l=l+12;
					table_syntax[l++]=',';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],4);
					l=l+12;
					break;
#if (DEVICE==68030)
				case 17:/* <fc>  */
					table_syntax[l++]='F';
					break;
				case 18:/*MRn Memory Management Unit Register */
					table_syntax[l++]='M';
					break;
				case 19:/*<fc>,<ea>,#level */
					table_syntax[l++]='F';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],14);
					l=l+12;
					table_syntax[l++]=',';
					table_syntax[l++]='E';
					syntaxsetup(&table_syntax[l],8);
					l=l+12;
					break;
#endif
			}
		}
		else
		{
			table_syntax[l++]='E';
			syntaxsetup(&table_syntax[l],int2);
			l=l+12;
		}
		}
		table_syntax[l]=ENDSTR;
/* *****************CONCLUDE SOURCE SETUP/DEST************************** */


/* sets up both strings so that these may be changed instead of the originals */

		while(teststring[0]!=' ' && teststring[0]!='\t' && teststring[0]!=ENDSTR)
		{
			shiftarg(&teststring[0],1);/* remove instr name from front */
			shiftarg(&teststringfc[0],1);/* remove instr name from front */
		}
		if(teststring[0]!=ENDSTR)
			{
			striparg(&teststring[0]);
			striparg(&teststringfc[0]);
			}
	

		SorD=FALSE;		/* set to source		*/
		error=FALSE;

	while(table_syntax[0]!=ENDSTR)
		{	
		if(error==TRUE)
			{
			instructnum=-1;
			break;
			}
		error=FALSE;
		if(table_syntax[0]==',')
			{
			shiftarg(table_syntax,1);
			SorD=TRUE;
			}
		else if(table_syntax[0]=='Q')
			{
			EA[SOURCE].Aregnum=EA[SOURCE].Aregnum | (EA[DEST].Aregnum<<9);
			shiftarg(table_syntax,1);
			}
		else if(table_syntax[0]=='U')
			{
			if((teststring[0]!='u' && teststring[0]!='U') ||
				(teststring[1]!='s' && teststring[1]!='S') ||
				(teststring[2]!='p' && teststring[2]!='P') ||
				(teststring[3]!=ENDSTR && teststring[3]!=','))
				{
					error=TRUE;
					continue;
				}
			shiftarg(&teststring[0],4);
			shiftarg(table_syntax,1);
			}
#if (DEVICE==68030)
		else if(table_syntax[0]=='F')
			{
			EA[SorD].immediatedata=getnum(&teststringfc[1],NOERR,DEFAULTSCALE); 
			if((teststring[0]=='d' || teststring[0]=='D') && 
				teststring[1]>='0' && teststring[1]<='7' &&
				(teststring[2]==ENDSTR || teststring[2]==','))
				{
				EA[SorD].immediatedata=(teststring[1]-'0' | 0x8);
				shiftarg(&teststring[0],3);
				}
			else if(teststring[0]=='#' && !error)
				{
				shiftarg(&teststring[0],1);
				EA[SorD].immediatedata=(getnum(&teststring[0],NOERR,DEFAULTSCALE) | 0x10) & 0x1f; 
				shiftarg(&teststring[0],1);
				}
			else if((teststring[0]=='s' || teststring[0]=='S' || 
				  teststring[0]=='d' || teststring[0]=='D') &&
				(teststring[1]=='f' || teststring[1]=='F') &&
				(teststring[2]=='c' || teststring[2]=='C') &&
				(teststring[3]==ENDSTR || teststring[3]==','))
				{
				if(teststring[0]=='s' || teststring[0]=='S')
					EA[SorD].immediatedata= 0x0;
				else
					EA[SorD].immediatedata= 0x1;
				shiftarg(&teststring[0],4);
				}
			else
				{
					EA[SorD].immediatedata= 0;
					error=TRUE;
					continue;
				}
			shiftarg(table_syntax,1);
			}
		else if(table_syntax[0]=='M')
			{
			if((teststring[0]=='s' || teststring[0]=='S') &&
				(teststring[1]=='r' || teststring[1]=='R') &&
				(teststring[2]=='p' || teststring[2]=='P') &&
				(teststring[3]==ENDSTR || teststring[3]==','))
				{
				EA[SOURCE].immediatedata= 0x0;
				shiftarg(&teststring[0],4);
				}
			else if((teststring[0]=='c' || teststring[0]=='C') &&
				(teststring[1]=='r' || teststring[1]=='R') &&
				(teststring[2]=='p' || teststring[2]=='P') &&
				(teststring[3]==ENDSTR || teststring[3]==','))
				{
				EA[SOURCE].immediatedata= 0x1;
				shiftarg(&teststring[0],4);
				}
			else if((teststring[0]=='t' || teststring[0]=='T') &&
				(teststring[1]=='c' || teststring[1]=='C') &&
				(teststring[2]==ENDSTR || teststring[2]==','))
				{
				EA[SOURCE].immediatedata= 0x2;
				shiftarg(&teststring[0],3);
				}
			else if((teststring[0]=='t' || teststring[0]=='T') &&
				(teststring[1]=='t' || teststring[1]=='T') &&
				(teststring[2]=='0' || teststring[2]=='0') &&
				(teststring[3]==ENDSTR || teststring[3]==','))
				{
				EA[SOURCE].immediatedata= 0x3;
				shiftarg(&teststring[0],4);
				}
			else if((teststring[0]=='t' || teststring[0]=='T') &&
				(teststring[1]=='t' || teststring[1]=='T') &&
				(teststring[2]=='1' || teststring[2]=='1') &&
				(teststring[3]==ENDSTR || teststring[3]==','))
				{
				EA[SOURCE].immediatedata= 0x4;
				shiftarg(&teststring[0],4);
				}
			else if((teststring[0]=='p' || teststring[0]=='P') &&
				(teststring[1]=='s' || teststring[1]=='S') &&
				(teststring[2]=='r' || teststring[2]=='R') &&
				(teststring[3]==ENDSTR || teststring[3]==','))
				{
				EA[SOURCE].immediatedata= 0x5;
				shiftarg(&teststring[0],4);
				}
			else
				{
					error=TRUE;
					continue;
				}
			shiftarg(table_syntax,1);
			}
#endif
		else if(table_syntax[0]=='S')
			{
			if((teststring[0]!='s' && teststring[0]!='S') ||
				(teststring[1]!='r' && teststring[1]!='R') ||
				(teststring[2]!=ENDSTR && teststring[2]!=','))
			{
				error=TRUE;
				continue;
			}
			shiftarg(&teststring[0],3);
			shiftarg(table_syntax,1);
			}
		else if(table_syntax[0]=='N')
			{
			if(teststring[0]==ENDSTR)
				shiftarg(table_syntax,1);
			else
				{
				error=TRUE;	
				continue;
				}
			}
		else if(table_syntax[0]=='Z')
			{
			for(l=0;(teststring[l]!=',' || l==MAXSTR);l++)
				;
			l++;
			for(j=0;j<=CRENTRIES;j++)
				{
				cr1=TRUE;
				cr2=TRUE;
				for(k=0;CRreg[j].reg[k]!=ENDSTR;k++)
					{
					if(CRreg[j].reg[k]!=teststring[k])
						cr1=FALSE;
					if(CRreg[j].reg[k]!=teststring[l+k])
						cr2=FALSE;
					}
				if(cr1==TRUE || cr2==TRUE)
					break;
				}
#if (DEVICE >= 68040)
/* ************************************************************** */
/* ***** POSIBLE PRINTING OF BANNERS FOR PRIVILEGE INSTR. ******* */
	if(j==5)
		print("         WARNING: 68030 ONLY INSTRUCTION\n");
/* ************************************************************** */
#endif
			if(((teststring[0]=='a'||teststring[0]=='A') ||
			    (teststring[0]=='d'||teststring[0]=='D'))
				&& teststring[1]>='0' 
				&& teststring[1]<='7' 
				&& teststring[2]==','
				&& cr2)
				{
				EA[SOURCE].immediatedata=((teststring[0] & 0x1)<<15) | (teststring[1]-'0'<<12) | (CRreg[j].field);
				EA[SOURCE].Aregnum=1;
				}
			else if(((teststring[l]=='a'||teststring[l]=='A') ||
			    (teststring[l]=='d'||teststring[l]=='D'))
				&& teststring[l+1]>='0' 
				&& teststring[l+1]<='7' 
				&& teststring[l+2]==ENDSTR
				&& cr1)
				{
				EA[SOURCE].immediatedata=((teststring[l] & 0x1)<<15) | (teststring[l+1]-'0'<<12) | (CRreg[j].field);
				EA[SOURCE].Aregnum=0;
				}
			else
				{
					error=TRUE;
					continue;
				}
			shiftarg(table_syntax,1);
			}
		else if(table_syntax[0]=='C')
			{
			if((teststring[0]!='c' && teststring[0]!='C') ||
				(teststring[1]!='c' && teststring[1]!='C') ||
				(teststring[2]!='r' && teststring[2]!='R') ||
				(teststring[3]!=ENDSTR && teststring[3]!=','))
				{
					error=TRUE;
					continue;
				}
			shiftarg(&teststring[0],4);
			shiftarg(table_syntax,1);
			}
		else if(table_syntax[0]=='X')
			{
			for(l=0;l<12;l++)
				allowableEA[l]=table_syntax[1+l];	
			allowableEA[l]=ENDSTR;	
			shiftarg(table_syntax,13);
			EAallowed(&teststring[0],SorD,allowableEA);		
			if(error)
				{
				instructnum=-1;
				continue;
				}
			for(l=0;l<=1;l++)
			{
			if(teststring[0]==':')
				shiftarg(&teststring[0],1);
			bitfieldvar=getnum(&teststring[0],NOERR,DEFAULTSCALE); 
			if(error)
				break;
			if(bitfieldvar>=208 && bitfieldvar<=215)
				EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<(11-(6*l))) | ((bitfieldvar & 0xf)<<(6-(6*l)));
			else if(bitfieldvar>=0x30 && bitfieldvar<=0x32)
				EA[SorD].immediatedata=EA[SorD].immediatedata | (((bitfieldvar-18) & 0x1f)<<(6-(6*l)));
			else if(bitfieldvar>=0x20 && bitfieldvar<=0x29)
				EA[SorD].immediatedata=EA[SorD].immediatedata | ((bitfieldvar-12)<<(6-(6*l)));
			else if(bitfieldvar>=0x10 && bitfieldvar<=0x19)
				EA[SorD].immediatedata=EA[SorD].immediatedata | ((bitfieldvar-6)<<(6-(6*l)));
			else if(bitfieldvar<=0x10)
				EA[SorD].immediatedata=EA[SorD].immediatedata | ((bitfieldvar)<<(6-(6*l)));
			else
				{
				error=TRUE;
				break;
				}
			}/* end for */
			if(error || teststring[0]!='}')
				{
				error=TRUE;
				continue;
				}
			shiftarg(&teststring[0],2);
			}
		else if(table_syntax[0]=='W')
			{
			if((teststring[0]=='(') &&
				((teststring[1]=='d' || teststring[1]=='D') ||
				(teststring[1]=='a' || teststring[1]=='A')) &&
				(teststring[2]>='0' && teststring[2]<='7') &&
				(teststring[3]==')') &&
				(teststring[4]==':') &&
				(teststring[5]=='(') &&
				((teststring[6]=='d' || teststring[6]=='D') ||
				(teststring[6]=='a' || teststring[6]=='A')) &&
				(teststring[7]>='0' && teststring[7]<='7') &&
				(teststring[8]==')'))
				{
				EA[SorD].Aregnum=(EA[SorD].Aregnum) | ((teststring[2])-'0'<<12) | ((teststring[7])-'0'<<8);	
				if(teststring[1]=='a' || teststring[1]=='A')
					EA[SorD].Aregnum=(EA[SorD].Aregnum) | (0x1<<15);	
				if(teststring[6]=='a' || teststring[6]=='A')
					EA[SorD].Aregnum=(EA[SorD].Aregnum) | (0x1<<11);	
				shiftarg(&teststring[0],6);
				shiftarg(table_syntax,1);
				}
			else
				{
				error=TRUE;
				continue;
				}
			}

		else if(table_syntax[0]=='Y')
			{
			if((teststring[0]=='d' || teststring[0]=='D') &&
				(teststring[1]>='0' && teststring[1]<='7') &&
				(teststring[2]==':') &&
				(teststring[3]=='d' || teststring[3]=='D') &&
				(teststring[4]>='0' && teststring[4]<='7'))
				{
				EA[SorD].Aregnum=(((teststring[1])-'0'<<4) | ((teststring[4])-'0'));	
				shiftarg(&teststring[0],6);
				shiftarg(table_syntax,1);
				}
			else
				{
				error=TRUE;
				continue;
				}
			}
		else if(table_syntax[0]=='R')
			{
			if(((teststring[0]=='a' || teststring[0]=='A') ||
			(teststring[0]=='d' || teststring[0]=='D')) &&
				(teststring[1]>='0' && teststring[1]<='7') &&
				(teststring[2]=='/' || teststring[2]==',' || teststring[2]==ENDSTR))
			{
				if(teststring[0]=='a' || teststring[0]=='A')
					EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<(8+(teststring[1])-'0'));	
				else
					EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<teststring[1]-'0');	
				shiftarg(&teststring[0],2);
			}
			else if(((teststring[0]=='a' || teststring[0]=='A') &&
				(teststring[3]=='a' || teststring[3]=='A')) ||
				((teststring[0]=='d' || teststring[0]=='D') &&
				(teststring[3]=='d' || teststring[3]=='D')) &&
				(teststring[1]>='0' && teststring[1]<='7') &&
				(teststring[2]>='-') &&
				(teststring[4]>='0' && teststring[4]<='7') &&
				(teststring[5]=='/' || teststring[5]==',' || teststring[5]==ENDSTR))
			{
				if(teststring[3]=='a' || teststring[3]=='A')
					for(l=(8+teststring[1]-'0');l<=(8+(teststring[4]-'0'));l++)
					{
					EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<l);	
					}
				else
					for(l=(teststring[1]-'0');l<=(teststring[4]-'0');l++)
					{
					EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<l);	
					}
				shiftarg(&teststring[0],5);
			}
			else
			{
				error=TRUE;
				continue;
			}
			if(teststring[0]==',' || teststring[0]==ENDSTR)
				{
				shiftarg(table_syntax,1);		
				shiftarg(&teststring[0],1);		
				}
			else
				{
				shiftarg(&teststring[0],1);		
				continue;
				}
			}
		else if(table_syntax[0]=='H')
			{
			if((teststring[0]=='f' || teststring[0]=='F') &&
			(teststring[1]=='p' || teststring[1]=='P') &&	
				(teststring[2]>='0' && teststring[2]<='7') &&
				(teststring[3]=='/' || teststring[3]==',' || teststring[2]==ENDSTR))
			{
				EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<teststring[1]-'0');	
				shiftarg(&teststring[0],3);
			}
			else if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[4]=='f' || teststring[4]=='F') &&
				(teststring[5]=='p' || teststring[5]=='P') &&
				(teststring[2]>='0' && teststring[2]<='7') &&
				(teststring[3]>='-') &&
				(teststring[6]>='0' && teststring[6]<='7') &&
				(teststring[7]=='/' || teststring[7]==',' || teststring[5]==ENDSTR))
			{
				for(l=(teststring[1]-'0');l<=(teststring[4]-'0');l++)
					EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<l);	
				shiftarg(&teststring[0],5);
			}
			else
			{
				error=TRUE;
				continue;
			}
			if(teststring[0]==',' || teststring[0]==ENDSTR)
				{
				shiftarg(table_syntax,1);		
				shiftarg(&teststring[0],1);		
				}
			else
				{
				shiftarg(&teststring[0],1);		
				continue;
				}
			}
		else if(table_syntax[0]=='I')
			{
			if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[3]>='0' && teststring[2]<='7') &&
				(teststring[4]==','|| teststring[4]==ENDSTR || teststring[4]=='{'))
				{
				EA[SorD].Aregnum= teststring[3]-'0';
				shiftarg(table_syntax,1);		
				shiftarg(&teststring[0],4);		
				return;
				}
			}
		else if(table_syntax[0]=='E') 
			{
			for(l=0;l<12;l++)
				allowableEA[l]=table_syntax[1+l];	
			allowableEA[l]=ENDSTR;	
			shiftarg(table_syntax,13);
			EAallowed(&teststring[0],SorD,allowableEA);		
			}
/* ************************************************ */
		else
{
error=TRUE;
print("\nWE HAVE A MIS-TAKE ON THE EAallowed TRANSFER");
continue;
}
/* ************************************************ */
		if(table_syntax[0]==ENDSTR && !error)
			{
			instructnum=instructmatch;
			return;
			}
		}/*end while*/
	}			/* end of if Bmatch=1    */
} 
}
/* instrmatch end{}							*/

/* ************************************************************************ */
/* ROUTINE:EAallowed							    */
/* ************************************************************************ */

int EAallowed(strsyntax,SorD,allowableEA)
char *strsyntax;
int SorD;
char allowableEA[];
{
extern char sizechar[];	/* actual letter typed in command ie. andi.b<--- */
extern EASPEC EA[];
extern MIPOST;
extern EAFIELD seperatorfield[];
extern EAFIELD actualfield[];
extern error;

char teststring[MAXSTR];	/* another teststring being initialized to
				   protect the original			*/
char teststring1[MAXSTR];	/* another teststring being initialized to
				   protect the original			*/
char argumentstring[MAXSTR];

int i,j;
int s;
int numchars;
int lastallowed,firstallowed;
int actualfields,actualseperators;
int error1;

#if(DEVICE==68040 || COPROCESSOR==TRUE)
int j_fp,e_fp;
double f_fp;
int fraction_bits;
double weight;
int precision;

j_fp = 0;
e_fp = 0;
f_fp = 0;
fraction_bits = 0;
weight = 0;
precision = 0;
#endif

/* **************************** */
/* INITIALIZE EAfields		*/
/* **************************** */


error = FALSE;
error1 = FALSE;
for(i=0;strsyntax[i]!=ENDSTR;i++)
	{
	teststring[i]=strsyntax[i];  /* used for mode 101 and 111.010 	   */
	teststring1[i]=strsyntax[i]; /* below to determine error & error1  */
	}
teststring[i]=ENDSTR;
teststring1[i]=ENDSTR;

/* Check to see if an error is found by getnum before shift ie. MODE 111.0/1  */
if(teststring1[0]==ENDSTR)
	error1=TRUE;
else
	{
	getnum(&teststring1[0],NOERR,DEFAULTSCALE); 
	if(error)
		error1=TRUE;
	else
		error1=FALSE;
	}

shiftarg(&teststring[0],1);

getnum(&teststring[0],NOERR,DEFAULTSCALE);  /* setup 'error' variable   */

/* NOTE: after setting up error any calls from this routine can not use
	 the global variable error unless this setting is no longer
	 required						*/
 
/* ******************************************** */
/* used in EA's that start with a displacement 	*/
/* ******************************************** */
for(i=0;((strsyntax[i]!=',') && (strsyntax[i]!=ENDSTR) && (strsyntax[i]!=')') && (i < MAXLINE));i++)
	;

/* ************************** */
/* MODE 000		      */
/* ************************** */
if(strsyntax[0]=='d' &&
	strsyntax[1]>='0' &&
	strsyntax[1]<='7' &&
	(strsyntax[2]==','|| strsyntax[2]==ENDSTR || strsyntax[2]=='{'))
	{
	shiftarg(strsyntax,1);		/* removes the 'D' from the string */
	EA[SorD].mode=0;
	EA[SorD].Aregnum= getnum(strsyntax,NOERR,DEFAULTSCALE);
	shiftarg(strsyntax,1);
	EAmatchOK(allowableEA,SorD);
	return;
	}
/* ************************** */
/* MODE 001 		      */
/* ************************** */
else if(strsyntax[0]=='a' &&
	strsyntax[1]>='0' &&
	strsyntax[1]<='7' &&
	(strsyntax[2]==','|| strsyntax[2]=='\0' || strsyntax[2]=='{'))
	{
	shiftarg(strsyntax,1);
	EA[SorD].mode=1;
	EA[SorD].Aregnum= getnum(strsyntax,NOERR,DEFAULTSCALE);
	shiftarg(strsyntax,1);
	EAmatchOK(allowableEA,SorD);
	return;
	}
/* ************************** */
/* MODE 010		      */
/* ************************** */
else if(strsyntax[0]=='(' &&
	strsyntax[1]=='a' &&
	(strsyntax[2]=='0'||strsyntax[2]=='1'||strsyntax[2]=='2'||strsyntax[2]=='3'||strsyntax[2]=='4'||strsyntax[2]=='5'||strsyntax[2]=='6'||strsyntax[2]=='7') &&
	strsyntax[3]==')' &&
	(strsyntax[4]==','|| strsyntax[4]=='\0' || strsyntax[4]=='{'))
	{
	shiftarg(strsyntax,2);
	EA[SorD].mode=2;
	EA[SorD].Aregnum= getnum(strsyntax,NOERR,DEFAULTSCALE);
	shiftarg(strsyntax,2);
	EAmatchOK(allowableEA,SorD);
	return;
	}
/* **************************** */
/* MODE 011			*/
/* **************************** */
else if(strsyntax[0]=='(' &&
	strsyntax[1]=='a' &&
	(strsyntax[2]=='0'||strsyntax[2]=='1'||strsyntax[2]=='2'||strsyntax[2]=='3'||strsyntax[2]=='4'||strsyntax[2]=='5'||strsyntax[2]=='6'||strsyntax[2]=='7') &&
	strsyntax[3]==')' &&
	strsyntax[4]=='+' &&
	(strsyntax[5]==','|| strsyntax[5]=='\0' || strsyntax[5]=='{'))
	{
	shiftarg(strsyntax,2);
	EA[SorD].mode=3;
	EA[SorD].Aregnum= getnum(strsyntax,NOERR,DEFAULTSCALE);
	shiftarg(strsyntax,3);
	EAmatchOK(allowableEA,SorD);
	return;
	}
/* **************************** */
/* MODE 100			*/
/* **************************** */	
else if(strsyntax[0]=='-' &&
	strsyntax[1]=='(' &&
	strsyntax[2]=='a' &&
	strsyntax[3]>='0' &&
	strsyntax[3]<='7' &&
	strsyntax[4]==')' &&
	(strsyntax[5]==','|| strsyntax[5]=='\0' || strsyntax[5]=='{'))
	{
	shiftarg(strsyntax,3);
	EA[SorD].mode=4;
	EA[SorD].Aregnum= getnum(strsyntax,NOERR,DEFAULTSCALE);
	shiftarg(strsyntax,2);
	EAmatchOK(allowableEA,SorD);
	return;
	}
/* **************************** */
/* MODE 101 and 111.010		*/
/* **************************** */	
else if(strsyntax[0]=='(' &&
	strsyntax[i]==',' && 
	((strsyntax[i+1]=='a' && strsyntax[i+2]>='0' && strsyntax[i+2]<='7')||
	(strsyntax[i+1]=='p' && strsyntax[i+2]=='c')) && 
	strsyntax[i+3]==')' &&
	(strsyntax[i+4]==','|| strsyntax[i+4]=='\0' || strsyntax[i+4]=='{') &&
	!error)
	{
	shiftarg(strsyntax,1);
	if(strsyntax[i]=='a') /*it has been shifted once so 'A' should be at i*/
	{
		EA[SorD].mode=5;
		EA[SorD].dis16=getnum(strsyntax,NOERR,DEFAULTSCALE);
		if(error==TRUE)
			return;	
		shiftarg(strsyntax,2);
		EA[SorD].Aregnum=getnum(strsyntax,NOERR,DEFAULTSCALE);
		if(error==TRUE)
			return;	
	}
	else
	{
		EA[SorD].mode=7;
		EA[SorD].dis16=getnum(strsyntax,NOERR,DEFAULTSCALE);
		if(error==TRUE)
			return;	
		shiftarg(strsyntax,3);
		EA[SorD].Aregnum=2;
	}
	shiftarg(strsyntax,2);
	EAmatchOK(allowableEA,SorD);
	return;
	}
/* **************************** */
/* MODE 111.000 and 111.001	*/
/* **************************** */
else if(
	(strsyntax[i]==')' &&
	!error &&
	((strsyntax[i+1]==',' || strsyntax[i+1]=='\0' || strsyntax[i+1]=='{') ||
	((strsyntax[i+1]=='.') &&
	(strsyntax[i+2]=='w' || strsyntax[i+2]=='l') &&
	(strsyntax[i+3]==',' || strsyntax[i+3]=='\0' || strsyntax[i+3]=='{'))))||
	(!error1 &&
	(strsyntax[i]==',' || strsyntax[i]=='\0' || strsyntax[i]=='{') &&
	(strsyntax[1]!='[')))
	{
	EA[SorD].mode=7;
	if(strsyntax[0]=='(')
		shiftarg(strsyntax,1);
	EA[SorD].dis16=getnum(strsyntax,NOERR,DEFAULTSCALE);
	switch(strsyntax[2])
	{
		case 'w':
		case 'W':
			EA[SorD].Aregnum=0;
			break;
		case 'L':
		case 'l':
			EA[SorD].Aregnum=1;
			break;
		default:	/* letter not entered */
			if(EA[SorD].dis16==(EA[SorD].dis16 & 0xffff))
				EA[SorD].Aregnum=0;
			else
				EA[SorD].Aregnum=1;
			break;
	}
	if(strsyntax[1]=='.')
		shiftarg(strsyntax,2);
	shiftarg(strsyntax,2);
	EAmatchOK(allowableEA,SorD);
	return;
	}
#if(DEVICE == 68040 || COPROCESSOR == TRUE)
/* **************************** */
/* MODE 111.100	#_._E_ FORMAT	*/
/* **************************** */
else if(strsyntax[0]=='#')
	{
	if(!error && teststring[0]!='.' && 
	     teststring[0]!='_' &&
	     (sizechar[0]=='b' || sizechar[0]=='w' || 
	      sizechar[0]=='l' || sizechar[0]=='~'))
			{
			if(sizechar[0]=='~')
				{
				sizechar[0]='l';
				sizechar[1]=ENDSTR;
				}
			shiftarg(strsyntax,1);
			EA[SorD].mode=7;
			EA[SorD].Aregnum=4;
			EA[SorD].immediatedata=getnum(strsyntax,NOERR,DEFAULTSCALE);
			shiftarg(strsyntax,1);
			EAmatchOK(allowableEA,SorD);
			return;
			}
	for(i=0;strsyntax[i]!=ENDSTR;i++)
		teststring[i]=strsyntax[i];  /* reset up this teststring */
	teststring[i]=ENDSTR;
	shiftarg(teststring,1);/* remove # from string */
	if((sizechar[0]=='p' || sizechar[0]=='P') 
		&& ((teststring[0]=='-' && teststring[2]!='.')
		|| (teststring[0]!='-' && teststring[1]!='.')))
		{
		error = TRUE;
		return;
		}
/* Scan for '.' and '_E or _e' */
	for(i=0,j=0;teststring[i]!=ENDSTR;i++)
		{
		if(teststring[i]=='.' || teststring[i]=='_')
			{
			teststring1[j++]=teststring[i];  /* use this teststring
							    to maintain which 
							    letters were found 
							    in number */

			teststring[i]=' ';/* ie. a valid stop symbol recognized
					      by getnum */
			}
		}
	teststring1[j]=ENDSTR;

	j_fp = 0;
	f_fp = 0.0;
	e_fp = 0;
	if((teststring[0] == ' ' && teststring1[0]!='.') || 
	    teststring1[0]==ENDSTR)
		{
		error = TRUE;
		return;
		}
	if(teststring[0] != ' ')
		{
		j_fp=getnum(teststring,NOERR,REGSCALE);
		if(error)
			return;
		}
	while(teststring1[0] != ENDSTR )
	{
		switch(teststring1[0])
			{
			case '.':
				precision = getlengthfraction(teststring);
				f_fp=getnum(teststring,NOERR,REGSCALE);
				for(i=precision;i>0;i--)
					f_fp = f_fp/(REGSCALE);
				break;
			case '_':
				if(teststring[0]!='E' && teststring[0]!='e')
					{
					error = TRUE;
					break;
					}
				shiftarg(teststring,1);
				e_fp=getnum(teststring,NOERR,REGSCALE);
				break;
			}
		shiftarg(teststring1,1);
		if(error)
			break;
	}/* end while */

	if(error)/* from getting f_fp or e_fp */
		return;

	/* j_fp,f_fp,e_fp are now setup the #data for the single,double,
	   extended and packed formats will be stored in FP_data[SorD]
	   based off whether we are storing SOURCE or DEST immediatedata
	   and the format will be determined by the sizechar[0] which for
 	   floating point instructions defaults to 'x' */


	if(sizechar[0]=='p' || sizechar[0]=='P')
		{
		for(i=0;strsyntax[i]!=ENDSTR;i++)
			teststring[i]=strsyntax[i];/*reset up this teststring*/
		teststring[i]=ENDSTR;
		}

	/* *********** */
	/* SET UP s_fp */
	if(j_fp < 0)
		{
		j_fp = -j_fp;
		EA[SorD].s_fp = 1;/* Note: it was initialized to 0 */
		shiftarg(teststring,1);/* even if not 'p' or 'P' */
		}
	/* *********** */

	if(sizechar[0]!='p' && sizechar[0]!='P')
		{
		/* *********** */
		/* SET UP f_fp */
		if(j_fp == 0 && f_fp!=0)
			for(;j_fp == 0; e_fp--)
				{
				f_fp = f_fp * 2;
				j_fp = j_fp + (int) (f_fp);
				f_fp = f_fp - (int) f_fp;
				}
		else if(j_fp > 1)
			for(;j_fp > 1; e_fp++)
				{
				f_fp = f_fp/2 + ((double) j_fp)/2.0 - (j_fp/2);
				j_fp = j_fp/2;
				}
		/* *********** */
		}/* end if */

	switch(sizechar[0])
		{
			/* SET UP e_fp			  */
			/* Start range checking for error */
		case 's':
		case 'S':
			fraction_bits = 23;
			e_fp = e_fp + 127;
			if(e_fp > 255 || e_fp < 0)
				error = TRUE;
			break;
		case 'd':
		case 'D':
			fraction_bits = 52;
			e_fp = e_fp + 1023;
			if(e_fp > 2047 || e_fp < 0)
				error = TRUE;
			break;
		case '~':
		case 'x':
		case 'X':
			fraction_bits = 63;
			e_fp = e_fp + 16383;
			if(e_fp > 32767 || e_fp < 0)
				error = TRUE;
			break;
		case 'p':
		case 'P':
			if(e_fp < 0)
				{
				EA[SorD].s1_fp = 1;
				e_fp = -e_fp;
				}
			if(e_fp > 4095)
				error = TRUE;
			break;
		}

	/* Start packing EA[SorD].__fp fields for immediatedata */
	if(error)
		return;

	EA[SorD].e_fp  = e_fp;
	if(sizechar[0]!='p' && sizechar[0]!='P')
		{
		for(i=1;i<=fraction_bits;i++)
			{
			for(weight=1,j=i;j>0;j--)
				weight =  weight * 0.5;

			if(f_fp >= weight)  
				{
				if(fraction_bits == 23)
					EA[SorD].f1_fp = EA[SorD].f1_fp | 0x1<<(fraction_bits-i);
				else if(fraction_bits == 52 && i<21)
					EA[SorD].f1_fp = EA[SorD].f1_fp | 0x1<<(fraction_bits-i-32);
				else if(fraction_bits == 52)
					EA[SorD].f2_fp = EA[SorD].f2_fp | 0x1<<(fraction_bits-i);
				else if(fraction_bits == 63 && i<32)
					EA[SorD].f1_fp = EA[SorD].f1_fp | 0x1<<(fraction_bits-i-32);
				else if(fraction_bits == 63)
					EA[SorD].f2_fp = EA[SorD].f2_fp | 0x1<<(fraction_bits-i);
				f_fp = f_fp - weight;
				}/* end if */
			}/* end for */
		}/* end if */
	else
		{
		shiftarg(teststring,1);/* remove the # sign again */
		EA[SorD].p_fp = teststring[0]-'0';
		shiftarg(teststring,2);/* remove th e'.' also */
		for(i=7;i>=0 && teststring[0]!='_' && teststring[0]!=',' && teststring[0]!=ENDSTR;i--)
			{
			EA[SorD].f1_fp = EA[SorD].f1_fp | (teststring[0]-'0'<<(i*4));
			shiftarg(teststring,1);
			}
		for(i=7;i>=0 && teststring[0]!='_' && teststring[0]!=',' && teststring[0]!=ENDSTR;i--)
			{
			EA[SorD].f2_fp = EA[SorD].f2_fp | (teststring[0]-'0'<<(i*4));
			shiftarg(teststring,1);
			}
		}/* end else */
	for(;strsyntax[0]!=',' && strsyntax[0]!=ENDSTR;shiftarg(strsyntax,1))
		;
	EA[SorD].mode=7;
	EA[SorD].Aregnum=4;
	shiftarg(strsyntax,1);/* remove the ',' */
	EAmatchOK(allowableEA,SorD);
	return;
	}/* end else if */
#else
/* **************************** */
/* MODE 111.100			*/
/* **************************** */
else if(strsyntax[0]=='#' && !error)
	{
	shiftarg(strsyntax,1);
	EA[SorD].mode=7;
	EA[SorD].Aregnum=4;
	EA[SorD].immediatedata=getnum(strsyntax,NOERR,DEFAULTSCALE);
	shiftarg(strsyntax,1);
	EAmatchOK(allowableEA,SorD);
	return;
	}
#endif
/* **************************** */
/* MODE 110 and MODE 111.011	*/
/* **************************** */	
else if(strsyntax[0]!='(')
	{
	error=TRUE;
	return;
	}
for(i=0;strsyntax[i]!=')' && i<=MAXSTR;i++)
	{
	if( i==MAXSTR )
		break;
	}

if(i==MAXSTR || ( strsyntax[i+1] != ',' && strsyntax[i+1] != '\0' && strsyntax[i+1]!='{'))
	{
	error=TRUE;
	return;
	}
/* ie. does the EA end in a ')'	*/


EA[SorD].mode=6;	/* NOTE: it may still be changed to 111 later */
for(j=0;strsyntax[j]!=ENDSTR;j++)
	argumentstring[j]=strsyntax[j];
argumentstring[j]=ENDSTR;

shiftarg(strsyntax,i+2);/* remove the source EA and posible ',' */

s=0;
actualfields=0;
actualseperators=0;

while(argumentstring[0]!=')' && actualfields<=5 )
{
	if((argumentstring[0]=='[' && argumentstring[1]==',') ||
		(argumentstring[0]== ','&& argumentstring[1]==']') ||
		(argumentstring[0]== ','&& argumentstring[1]==',') ||
		(argumentstring[0]== '('&& argumentstring[1]==',') ||
		(argumentstring[0]== ','&& argumentstring[1]==')'))
		{
		if(argumentstring[0]=='[')
			{
			seperatorfield[s].field[0]=argumentstring[0];
			actualseperators++;
			}
		actualfield[actualfields].field[0]=',';
		actualfield[actualfields].field[1]=ENDSTR;
		actualfields++;
		s++;
		shiftarg(argumentstring,1);
		}
	else if(argumentstring[0]== ',')
		shiftarg(argumentstring,1);
	else if(argumentstring[0]== '(')
		shiftarg(argumentstring,1);
	else if(argumentstring[0]== '['|| argumentstring[0]==']')
		{
		seperatorfield[s].field[0]=argumentstring[0];
		actualseperators++;
		shiftarg(argumentstring,1);
		if(argumentstring[0]==',' &&
				 seperatorfield[s].field[0]==']' &&
				 argumentstring[1]!=',' &&
				 argumentstring[1]!=' ' &&
				 argumentstring[1]!=')')
			shiftarg(argumentstring,1);
		}
	else
		{
		error=FALSE;
		numchars=getarglength(argumentstring);
		if(error)
			return;
		for(i=0;i<=numchars;i++)
			actualfield[actualfields].field[i]=argumentstring[i];
		actualfield[actualfields].field[i]=ENDSTR;
		shiftarg(argumentstring,numchars);
		actualfields++;
		s++;
		}
}

/* actual format should be ,_,_,_,_, where the fields are a0,a1,a2,a3 and
   the ','s are actually s0,s1,s2,s3,s4 as represented by actualfields
   and actual seperators: Note actual seperators are only '[' or ']'.If
   the s0-4 field had either a '[' or ']' in it then this is the character
   that is copied into the seperatorfield[].field. If not then the characters
   that were there are put into the actualfiled[].field. Regardless everytime
   an actual field is entered the seperator field increments.		*/ 

#if (DEVICE>=68020)
if(actualseperators==2 && actualfields!=4)
	{
	error=TRUE;
	return;
	}

if(actualfields>4 || (actualseperators!=0 && actualseperators!=2))
	{
	error=TRUE;
	return;
	}

if(actualseperators==2 && 
	(seperatorfield[0].field[0]!='[' || 
	(seperatorfield[2].field[0]!=']' && seperatorfield[3].field[0] != ']')))
	{
	error=TRUE;
	return;
	}

if(actualseperators==2 && seperatorfield[2].field[0]==']')
	MIPOST=TRUE;
else
	MIPOST=FALSE;

#else
if(actualseperators!=0 || actualfields!=3)
	{
	print("\nillegal mode 68000 device");
	error=TRUE;
	return;
	}
#endif

/* actualfields end in either a ENDSTR or , character			*/

switch(actualseperators)
	{
	case 0:
		lastallowed=3;
		break;
	case 2:
		lastallowed=4;
		break;
	}

firstallowed=1;

for(i=0;i<=actualfields-1;i++)
{
	firstallowed=EAplacement(SorD,i,&firstallowed,&lastallowed);
	if(firstallowed==-1)
		{
		error=TRUE;
		return;
		}
}	
error=FALSE;
EAmatchOK(allowableEA,SorD);
return;
}



#if(DEVICE==68040 || COPROCESSOR==TRUE)

/* ******************************************************************* */
/* ROUTINE: getlengthfraction		 			       */
/* ******************************************************************* */

int getlengthfraction(strsyntax)
char *strsyntax;
{
extern int error;
int i;

for(i=0;(strsyntax[i]!=ENDSTR && 
	strsyntax[i]!=',' && 
	strsyntax[i]!='e' && 
	strsyntax[i]!='E') ;i++ )
	;
if( strsyntax[i-1]==' ')
	return(i-1);
else
	return(i);
}

#endif


/* ******************************************************************* */
/* ROUTINE: EAplacement			 			       */
/* ******************************************************************* */

int EAplacement(SorD,actualfieldnum,firstallowed,lastallowed)
int SorD;
int actualfieldnum;
int *firstallowed;
int *lastallowed;
{
extern int error;
extern EASPEC EA[];
extern EAFIELD actualfield[];
extern ACTUALFIELD_T_F Fieldoption[];

int i;
int Zshift;

Zshift=FALSE;
while(TRUE)
{
/* NOTE: if the field begins with the char ',' then*/
/* fieldshould be allowed but should be suppressed */
/* *firstallowed should remain the same and string */
/* changed so that upon continuing the correct     */
/* fields will be set				   */

if(actualfield[actualfieldnum].field[0]==',' && 								(*firstallowed==2 || *firstallowed==3))
	{
   	actualfield[actualfieldnum].field[0]='z';
	if(*firstallowed==2)
    		actualfield[actualfieldnum].field[1]='a';
	if(*firstallowed==3)
    		actualfield[actualfieldnum].field[1]='d';
    	actualfield[actualfieldnum].field[2]='0';
    	actualfield[actualfieldnum].field[3]=ENDSTR;
	}

/* NOTE: only the 2 and 3 field are bothered with because */
/*       for the displacementfields getnum when sent a ','*/
/*       will return a 0.				  */

switch(*firstallowed)
{
	case 1:
		*firstallowed=*firstallowed+1;
		EA[SorD].BDdisplacement=getnum(actualfield[actualfieldnum].field,NOERR,DEFAULTSCALE);
		if(error)
			{
			if(*firstallowed > *lastallowed)
				return(-1);
			else
				continue;
			}
		else
			Fieldoption[1].option=TRUE;
			if(EA[SorD].BDdisplacement==(EA[SorD].BDdisplacement & 0xff))/* 8bit displacement */
				EA[SorD].displacement8=EA[SorD].BDdisplacement;
			if(EA[SorD].BDdisplacement==0) /* NULL displacement */
				EA[SorD].BDsize=1;
			else if(EA[SorD].BDdisplacement==(EA[SorD].BDdisplacement & 0xffff))/* 16displacement */
				EA[SorD].BDsize=2;
			else /* 32displacement */
				EA[SorD].BDsize=3;
			
			return(*firstallowed);	/* returns the next available pos.*/
		break;
	case 2:
	case 3:
		switch(actualfield[actualfieldnum].field[0])
		{
			case 'z':
			case 'Z':
				switch(*firstallowed)
				{
					case 2:
						EA[SorD].BS=1;
						break;
					case 3:
						EA[SorD].IS=1;
						break;
				}
				Zshift=TRUE;
				actualfield[actualfieldnum].field[0]=actualfield[actualfieldnum].field[1];
				continue;/* should branch to while statement*/
			case 'd':
			case 'D':
				if(*firstallowed!=3)
					{
					*firstallowed=*firstallowed+1;
					if(*firstallowed > *lastallowed)
						return(-1);
					}
				EA[SorD].indexregtype=0;
			case 'a':
			case 'A':
				switch(actualfield[actualfieldnum].field[1+Zshift])
				{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						switch(*firstallowed)
						{
							case 2:
		EA[SorD].Aregnum=actualfield[actualfieldnum].field[1+Zshift]-'0';
		if(actualfield[actualfieldnum].field[2+Zshift] != ',' && actualfield[actualfieldnum].field[2+Zshift] != ENDSTR && actualfield[actualfieldnum].field[2+Zshift] != ')' && actualfield[actualfieldnum].field[2+Zshift] != ']')
			{
			*firstallowed=*firstallowed+1;
			if(*firstallowed > *lastallowed)
				return(-1);
			else
				continue;
			}
		else
			{
			if(Zshift==FALSE)
				Fieldoption[2].option=TRUE;
			*firstallowed=*firstallowed+1;
			return(*firstallowed);
			}
								break;
							case 3:
		EA[SorD].indexregnum=actualfield[actualfieldnum].field[1+Zshift]-'0';
								break;
						}
						break;
					default:
						*firstallowed=*firstallowed+1;
						if(*firstallowed > *lastallowed)
							return(-1);
						else
							continue;
						break;
				}
				switch(actualfield[actualfieldnum].field[2+Zshift])
				{
					case '.':
					case '*':
if(*firstallowed!=3)
	{
	*firstallowed=*firstallowed+1;
	if(*firstallowed > *lastallowed)
		return(-1);
	else
		continue;
	}
if(actualfield[actualfieldnum].field[2+Zshift]=='.')
	{
	if(actualfield[actualfieldnum].field[3+Zshift] != 'w' && actualfield[actualfieldnum].field[3+Zshift] != 'l')
		{
		*firstallowed=*firstallowed+1;
		if(*firstallowed > *lastallowed)
			return(-1);
		else
			continue;
		}
	else if(actualfield[actualfieldnum].field[3+Zshift] == 'l')
		EA[SorD].indexsize=1;/*note:should have been initialized to 0:*/

	shiftarg(actualfield[actualfieldnum].field,2);
	}
if(actualfield[actualfieldnum].field[2+Zshift]=='*')
	{
	shiftarg(actualfield[actualfieldnum].field,3+Zshift);
	error=FALSE;
	EA[SorD].scale=getnum(actualfield[actualfieldnum].field,NOERR,DEFAULTSCALE);

	if(EA[SorD].scale !=1 && EA[SorD].scale !=2 && EA[SorD].scale!=4 && EA[SorD].scale!=8)
		{
		*firstallowed=*firstallowed+1;
		if(*firstallowed > *lastallowed)
			return(-1);
		else
			continue;
		}
	}
					case ENDSTR:
					case ')':
					case ']':
					case ',':
				switch(EA[SorD].scale)
				{
					case 0:
					case 1:
						EA[SorD].scale=0;
						break;
					case 2:
						EA[SorD].scale=1;
						break;
					case 4:
						EA[SorD].scale=2;
						break;
					case 8:
						EA[SorD].scale=3;
						break;
				}
						if(Zshift==FALSE)
							Fieldoption[3].option=TRUE;
						*firstallowed=*firstallowed+1;
						return(*firstallowed);
						break;
					default:
						*firstallowed=*firstallowed+1;
						if(*lastallowed==2)
							return(-1);
						else
							continue;
						break;
				}
				break;
			case 'P':
			case 'p':
				if(*firstallowed!=2)
					{
					*firstallowed=*firstallowed+1;
					if(*firstallowed > *lastallowed)
						return(-1);
					else
						continue;
					}
				switch(actualfield[actualfieldnum].field[1+Zshift])
				{
					case 'C':
					case 'c':
						switch(actualfield[actualfieldnum].field[2+Zshift])
						{
							case ENDSTR:
							case ',':
							case ']':
				EA[SorD].mode=7;
				EA[SorD].Aregnum=3;
				if(Zshift==FALSE)
					Fieldoption[2].option=TRUE;
				*firstallowed=*firstallowed+1;
				return(*firstallowed);
								break;
							default:
				*firstallowed=*firstallowed+1;
				if(*firstallowed > *lastallowed)
					return(-1);
				else
								continue;
						}
					default:
						*firstallowed=*firstallowed+1;
						if(*firstallowed > *lastallowed)
							return(-1);
						else
							continue;
				}
				break;
			default:
				*firstallowed=*firstallowed+1;
				if(*firstallowed > *lastallowed)
					return(-1);
				else
					continue;
			}
			break;
	case 4:
		EA[SorD].ODdisplacement=getnum(actualfield[actualfieldnum].field,NOERR,DEFAULTSCALE);
		if(error)
			return(-1);/* this is the last allowed position 
				       so return an error		 */
		Fieldoption[4].option=TRUE;
		if(EA[SorD].ODdisplacement==0) 	   /* NULL displacement */
			EA[SorD].index_indirect_selection=1;
		else if(EA[SorD].ODdisplacement==(EA[SorD].ODdisplacement & 0xffff))		       	        /* Word displacement */
			EA[SorD].index_indirect_selection=2;
		else
			/* Long displacement */
			EA[SorD].index_indirect_selection=3;
		if(MIPOST && (EA[SorD].IS==FALSE))
			EA[SorD].index_indirect_selection=EA[SorD].index_indirect_selection+4;
		*firstallowed=*firstallowed+1;
		return(*firstallowed);	/* returns the next available pos.*/
	default:
		return(-1);
}	
}
}

/* ******************************************************************* */
/* ROUTINE: initEAstructures		 			       */
/* ******************************************************************* */

initEAstructures()
{
extern EASPEC EA[];
extern EAFIELD actualfield[];
extern EAFIELD seperatorfield[];
extern ACTUALFIELD_T_F Fieldoption[];
extern int assemblycode[];

#if(DEVICE==68040 || COPROCESSOR==TRUE)
extern optionSorD;
#endif

int i;

/* ****************************************** */
/* INITIALIZE FIELDS THAT WILL BE USED FOR    */
/* THE EAplacement ROUTINE		      */

for(i=0;i<=4;i++)
{
	Fieldoption[i].option=FALSE;
	actualfield[i].field[0]=ENDSTR;
	seperatorfield[i].field[0]=ENDSTR;
}

for(i=0;i<=10;i++)
{
	assemblycode[i]=0;
}

#if(DEVICE==68040 || COPROCESSOR==TRUE)
	optionSorD = ENDSTR;
#endif

for(i=0;i<=1;i++)
{
	EA[i].BDsize=1;
	EA[i].BS=0;
	EA[i].IS=0;
	EA[i].indexregtype=1;
	EA[i].indexsize=0;
	EA[i].index_indirect_selection=0;
	EA[i].scale=0;
	EA[i].mode=6;
	EA[i].BDdisplacement=0;
	EA[i].ODdisplacement=0;
	EA[i].immediatedata=0;
	EA[i].Aregnum=0;
#if(DEVICE==68040 || COPROCESSOR==TRUE)
	EA[i].s_fp=0;
	EA[i].s1_fp=0;
	EA[i].e_fp=0;
	EA[i].f1_fp=0;
	EA[i].f2_fp=0;
#endif

}
/* ****************************************** */
}

/* ******************************************************************* */
/* ROUTINE: getarglength		 			       */
/* ******************************************************************* */

int getarglength(strsyntax)
char *strsyntax;
{
extern int error;
int i;

for(i=0;(strsyntax[i]!=']' && strsyntax[i]!=',' && strsyntax[i]!=')');i++ )
	;
if(i>8)
	{
	error=TRUE;
	return;
	}
return(i);
}

/* ******************************************************************* */
	

/* ******************************************************************* */
/* ROUTINE: EAmatchOK			 			       */
/* ******************************************************************* */

EAmatchOK(allowableEA,SorD)
char allowableEA[13];
int SorD;
{
extern EASPEC EA[];
extern int error;

		error=FALSE;
		switch(EA[SorD].mode)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				if(allowableEA[EA[SorD].mode]!='1')
				{
					error=TRUE;
					return;
				}
				break;
			case 6:
#if (DEVICE>=68020)
				if(allowableEA[EA[SorD].mode]!='1')
				{
					error=TRUE;
					return;
				}
				break;
#else
				if((Fieldoption[1].option==TRUE &&
				   Fieldoption[2].option==TRUE &&
				   Fieldoption[3].option==TRUE &&
 				   Fieldoption[4].option==FALSE &&
				   EA[SorD].BDdisplacement!=(EA[SorD].BDdisplacement & 0xff))
				||(Fieldoption[4].option==TRUE)
			 	||(allowableEA[EA[SorD].mode]!='1'))
				{
					error=TRUE;
					return;
				}
				break;
#endif
			case 7:
#if (DEVICE>=68020)
				if(allowableEA[EA[SorD].mode+EA[SorD].Aregnum]!='1')
				{
					error=TRUE;
					return;
				}
				break;
#else
				if((Fieldoption[1].option==TRUE &&
				   Fieldoption[2].option==TRUE &&
				   Fieldoption[3].option==TRUE &&
 				   Fieldoption[4].option==FALSE &&
				   EA[SorD].BDdisplacement!=(EA[SorD].BDdisplacement & 0xff))
				||(Fieldoption[4].option==TRUE)
				||(allowableEA[EA[SorD].mode+EA[SorD].Aregnum]!='1'))
				{
					error=TRUE;
					return;
				}
				break;
#endif
		}
}


/* ******************************************************************* */
/* ROUTINE:packassemblycode1					       */
/* ******************************************************************* */
packassemblycode1(instructnum)
int instructnum;
{
extern int assemblycode[];
extern OPSTRUCTURE syntax1[];


int i;

assemblycode[1]=0;
for(i=0;i<=15;i++)
{
#if(DEVICE==68040 || COPROCESSOR==TRUE)
	if(instructnum < 200)
#endif
	switch(syntax1[instructnum].opwordstr[i])
	{
		case '0':
		case 'x':
			break;
		case '1':
			assemblycode[1]=assemblycode[1] | (0x1<<(15-i));
			break;
	}
#if(DEVICE==68040 || COPROCESSOR==TRUE)
	else if(instructnum >= 200)
		assemblycode[1]=0xf200;
#endif
}
}

/* ******************************************************************* */
/* ROUTINE:codeassembly						       */
/* ******************************************************************* */
int codeassembly(start,instructnum)
int start;
int instructnum;
{
/* ************************************ */
/* ASSEMBLY CODE ARRAY			*/
extern int assemblycode[];
/* ************************************ */
extern EASPEC EA[];
extern int error;
extern int packasm;
extern int condition;
extern char argv[];
extern char sizechar[];

#if(DEVICE==68040 || COPROCESSOR==TRUE)
extern COPROCSTRUCTURE syntax2[];
extern predicate;
extern optionSorD;
#endif


int size;
int returnsize;
int i;

packasm=0;


packassemblycode1(instructnum);
error = FALSE;

switch(instructnum)
{
	case 1:
	case 12:
	case 13:
	case 14:
	case 21:
	case 22:
		sizechar_num(sizechar[0],EA[SOURCE].immediatedata);
/* NOTE: if sizechar[] is not defined (ie. an '~') then this will NOT			 return an error. It will allow sizedef to determine the
	 sizechar[] and its validity.					*/
		if(error)
			{
			print("ERROR 11:immediate data/size option error ===> ");
			print(argv);
			print("\n");
			return(-1);
			}
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);

/* NOTE:generally a check for errors is made after a call to sizedef
	unless the instruction allows the .l-long addressing mode.	*/


		assemblycode[1]= (assemblycode[1] | (size<<6) | (EA[TRUE].mode<<3) | (EA[TRUE].Aregnum));
		switch(sizechar[0])
		{
			case 'b':
			case 'B':
				assemblycode[2]=EA[SOURCE].immediatedata & 0xff;
				packasm=3;
				break;
			case 'w':
			case 'W':
				assemblycode[2]=EA[SOURCE].immediatedata & 0xffff;
				packasm=3;
				break;
			case 'l':
			case 'L':
				assemblycode[2]=(EA[SOURCE].immediatedata>>16);
				assemblycode[3]=(EA[SOURCE].immediatedata & 0xffff);
				packasm=4;
				break;
		}
		EApackassembly(DEST);		
		break;
	case 2:
	case 10:
	case 19:
		assemblycode[2]=EA[SOURCE].immediatedata & 0xff;
		packasm=3;
		break;
	case 3:
	case 11:
	case 20:
		assemblycode[2]=EA[SOURCE].immediatedata;
		packasm=3;
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum<<9) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		packasm=2;
		if(bitnum(2,0,EA[DEST].mode)==0)
			sizechar[0]='l';
		else
			sizechar[0]='b';
		EApackassembly(DEST);		
		break;
	case 8:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (size<<6) | (EA[SOURCE].Aregnum));
		assemblycode[2]= EA[SOURCE].dis16;
		packasm=3;
		break;
	case 9:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum<<9) | (size<<6) | (EA[DEST].Aregnum));
		assemblycode[2]= EA[DEST].dis16;
		packasm=3;
		break;
	case 15:
	case 16:
	case 17:
	case 18:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]=(EA[SOURCE].immediatedata & 0xff);
		packasm=3;
		if(bitnum(2,0,EA[DEST].mode)==0)
			sizechar[0]='l';
		else
			sizechar[0]='b';
		EApackassembly(DEST);		
		break;
	case 23:
	case 25:
	case 27:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (EA[DEST].mode<<6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		packasm=2;
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;/* Note: since 23 has a .l option then
				 it is not neccessary to print an error */
		EApackassembly(SOURCE);		
		EApackassembly(DEST);		
		break;	
	case 24:
	case 26:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		packasm=2;
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		EApackassembly(SOURCE);		
		EApackassembly(DEST);		
		break;	
	case 40:
		size=sizedef(instructnum,&EA[DEST].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (size<<6) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		if(EA[DEST].mode==4)/* PERFORM THE REVERSE */
			{
			for(i=0;i<=15;i++)
				{
				if((EA[SOURCE].immediatedata>>15-i) & (0x1))
					assemblycode[2]= (assemblycode[2] |(0x1<<i));
				}
			}
		else
			{
			assemblycode[2]= EA[SOURCE].immediatedata;
			}
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 44:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (size<<6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= EA[DEST].immediatedata;
		packasm=3;
		EApackassembly(SOURCE);		
		break;
/*************************************************************************/
/* ASSEMBLYCODE DEFINED AND REQUIRES SPECIFIC PACKASM AND PACKING 	 */
/*************************************************************************/
	case 30:
	case 31:
#if (DEVICE >= 68020)
	case 137:
#endif
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		packasm=2;
		EApackassembly(SOURCE);		
		break;
#if (DEVICE >= 68010)
	case 125:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (size<<6) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]= ((EA[SOURCE].mode<<15) | (EA[SOURCE].Aregnum<<12) | (0x1<<11));
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 126:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (size<<6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= ((EA[DEST].mode<<15) | (EA[DEST].Aregnum<<12));
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 128:
		assemblycode[2]= (EA[SOURCE].immediatedata & 0xffff);
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 129:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum));
		assemblycode[2]= (EA[SOURCE].immediatedata);
		packasm=3;
		break;
#endif
#if (DEVICE >= 68020)
	case 131:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (size<<9) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= ((EA[DEST].mode<<15) | (EA[DEST].Aregnum<<12));
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 132:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (size<<9) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= ((EA[DEST].mode<<15) | (EA[DEST].Aregnum<<12) | (0x1<<11));
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 134:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]= (EA[SOURCE].immediatedata & 0xff);
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 135:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (size<<9) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]= (EA[SOURCE].Aregnum>>3) | (EA[SOURCE].Aregnum & 0x7);
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 136:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= assemblycode[1] | (size<<9);
		assemblycode[2]= (EA[DEST].Aregnum & 0xf000) | ((EA[DEST].Aregnum<<2) & 0x01c0) | ((EA[SOURCE].Aregnum>>4) & 0x7);
		assemblycode[3]= ((EA[DEST].Aregnum<<4) & 0xf000) | ((EA[DEST].Aregnum<<6) & 0x01c0) | (EA[SOURCE].Aregnum & 0x7);
		packasm=4;
		EApackassembly(DEST);		
		break;
	case 138:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum));
		assemblycode[2]= (EA[DEST].immediatedata>>16);
		assemblycode[3]= (EA[DEST].immediatedata & 0xffff);
		packasm=4;
		break;
	case 140:
	case 141:
	case 142:
	case 143:
	case 144:
	case 145:
	case 146:
	case 147:
	case 148:
	case 149:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		error=FALSE;
		switch(instructnum)
		{
		case 140:
		case 147:
			assemblycode[2]= (EA[DEST].Aregnum<<12) | (0x1<<11) | (EA[DEST].Aregnum);
			break;
		case 142:
		case 144:
			assemblycode[2]= (EA[DEST].Aregnum<<12) | (EA[DEST].Aregnum);
			break;
		case 141:
			assemblycode[2]= ((EA[DEST].Aregnum<<12) & 0x7000) | (0x3<<10) | ((EA[DEST].Aregnum>>4) & 0x7);
			if(bitnum(14,12,assemblycode[2])==bitnum(2,0,assemblycode[2]))
				error=TRUE;
			break;
		case 148:
			assemblycode[2]= ((EA[DEST].Aregnum<<12) & 0x7000) | (0x3<<10) | ((EA[DEST].Aregnum>>4) & 0x7);
			break;
		case 149:
			assemblycode[2]= ((EA[DEST].Aregnum<<12) & 0x7000) | (0x2<<10) | ((EA[DEST].Aregnum>>4) & 0x7);
			break;
		case 143:
			assemblycode[2]= ((EA[DEST].Aregnum<<12) & 0x7000) | (0x1<<10) | ((EA[DEST].Aregnum>>4) & 0x7);
			if(bitnum(14,12,assemblycode[2])==bitnum(2,0,assemblycode[2]))
				error=TRUE;
			break;
		case 145:
			assemblycode[2]= ((EA[DEST].Aregnum<<12) & 0x7000) | (0x1<<10) | ((EA[DEST].Aregnum>>4) & 0x7);
			break;
		case 146:
			assemblycode[2]= ((EA[DEST].Aregnum<<12) & 0x7000) | ((EA[DEST].Aregnum>>4) & 0x7);
			break;
		}
		if(error)
			{
			print("ERROR 13:dh=di is an undefined operation ===> ");
			print(argv);
			print("\n");
			return(-1);
			}
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 152:
	case 154:
		assemblycode[1]= (assemblycode[1] | (0x1<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (EA[DEST].immediatedata);
		packasm=3;
		break;
	case 153:
	case 155:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum));
		assemblycode[2]= (EA[DEST].immediatedata);
		packasm=3;
		break;
	case 156:
	case 157:
		assemblycode[1]= (assemblycode[1] | (condition<<8) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= EA[SOURCE].immediatedata;
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 158:
		assemblycode[1]= (assemblycode[1] | (condition<<8) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (EA[SOURCE].immediatedata) | (EA[DEST].Aregnum<<12);
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 159:
		assemblycode[1]= (assemblycode[1] | (condition<<8) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]= (EA[DEST].immediatedata) | (EA[SOURCE].Aregnum<<12);
		packasm=3;
		EApackassembly(DEST);		
		break;
#endif
/*************************************************************************/
/* ASSEMBLYCODE DEFINED AND REQUIRES THE DEST TO BE PACKED	         */
/*************************************************************************/
	case 28:
	case 61:
	case 62:
	case 63:
	case 77:
	case 87:
	case 93:
	case 107:
	case 120:
	case 124:
#if (DEVICE >= 68010)
	case 127:
#endif
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			{
			print("ERROR 11:immediate data/size option error ===> ");
			print(argv);
			print("\n");
			return(-1);
			}
		switch(instructnum)
		{
			case 28:
#if (DEVICE >= 68010)
			case 127:
#endif
				assemblycode[1]= (assemblycode[1] | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
				break;
			case 61:
			case 62:
			case 63:
			case 107:
				assemblycode[1]= (assemblycode[1] | ((EA[SOURCE].immediatedata & 0x7)<<9) | (size<<6) |	(EA[DEST].mode<<3) | (EA[DEST].Aregnum));
				break;
			case 77:
			case 87:
			case 93:
				assemblycode[1]= (assemblycode[1] |(EA[SOURCE].Aregnum<<9) | (size<<6) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
				break;
			case 120:
			case 124:
				assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum<<9) | (size<<6) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
				break;
		}
		packasm=2;
		EApackassembly(DEST);		
		break;	
/*************************************************************************/
/* ASSEMBLYCODE DEFINED AND REQUIRES THE SOURCE TO BE PACKED	         */
/*************************************************************************/
	case 29:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 39:
	case 42:
	case 43:
	case 70:
	case 71:
	case 72:
	case 75:
	case 76:
	case 79:
	case 80:
	case 86:
	case 88:
	case 91:
	case 92:
	case 94:
	case 95:
	case 96:
	case 97:
	case 108:
	case 111:
	case 114:
	case 117:
	case 121:
	case 122:
	case 123:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			{
			print("ERROR 11:immediate data/size option error ===> ");
			print(argv);
			print("\n");
			return(-1);
			}
	case 57:/* SIZE CHECK NOT REQUIRED */
	case 58:
	case 60:
		switch(instructnum)
		{
			case 29:
			case 32:
			case 34:
			case 36:
			case 43:
				assemblycode[1]= (assemblycode[1] | (size<<6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				break;
			case 60:
				assemblycode[1]= ((assemblycode[1]) | (condition <<8) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				break;
			case 33:
			case 35:
			case 37:
			case 39:
			case 42:
			case 57:
			case 58:
			case 94:
			case 95:
			case 96:
			case 97:
			case 108:
			case 111:
			case 114:
			case 117:
				assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				break;
			case 70:
			case 71:
				assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum <<9) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				break;
			case 72:
			case 75:
			case 88:
				assemblycode[1]= (assemblycode[1] |(EA[DEST].Aregnum<<9) | (size<<7) | (0x1<<6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				break;
			case 76:
			case 86:
			case 91:
			case 92:
				assemblycode[1]= (assemblycode[1] |(EA[DEST].Aregnum<<9) | (size<<6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				break;
			case 79:
			case 80:
				assemblycode[1]= (assemblycode[1] |(EA[DEST].Aregnum<<9) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				break;
			case 121:
			case 122:
			case 123:
				assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (size<<6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				break;
		}
		packasm=2;
		EApackassembly(SOURCE);		
		break;
/*************************************************************************/
/*************************************************************************/
/* ASSEMBLYCODE DEFINED SPECIFICALLY BELOW NO SOURCE OR DEST PACKING     */
/*************************************************************************/
	case 38:
	case 47:
	case 48:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum));
		packasm=2;
		break;
	case 49:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum));
		packasm=2;
		break;
	case 45:
		assemblycode[1]= (assemblycode[1] | ((EA[SOURCE].immediatedata) &0xf));
		packasm=2;
		break;
	case 46:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum));
		assemblycode[2]= EA[DEST].immediatedata;
		packasm=3;
		break;
	case 52:
		assemblycode[2]= (EA[SOURCE].immediatedata & 0xffff);
		packasm=3;
		break;
	case 59:
		assemblycode[1]= ((assemblycode[1]) | (condition <<8) | (EA[SOURCE].Aregnum));
		/* IMMEDIATE DATA */
		if(EA[DEST].mode==7 && EA[DEST].Aregnum!=0 && EA[DEST].Aregnum!=1)
			EA[DEST].immediatedata=EA[DEST].immediatedata -2; 
		/* ABSOLUTE DATA */
		else
			EA[DEST].immediatedata=EA[DEST].dis16-start-2; 
		if((EA[DEST].immediatedata % 2) != 0)
			{
			packasm=-1;
			print("ERROR 10:illegal change of flow ===> ");
			print(argv);
			print("\n");
			break;
			}
		assemblycode[2]= (EA[DEST].immediatedata & 0xffff);
		packasm=3;
		break;
#if (DEVICE >= 68020)
	case 150:
		assemblycode[1]= (assemblycode[1] | (condition <<8));
		packasm=2;
		break;
	case 151:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			{
			print("ERROR 11:immediate data/size option error ===> ");
			print(argv);
			print("\n");
			return(-1);
			}
		assemblycode[1]= (assemblycode[1] | (condition <<8) | (size));
		if(size==3)
			{
			assemblycode[2]= (EA[SOURCE].immediatedata>>16);
			assemblycode[3]= (EA[SOURCE].immediatedata & 0xffff);
			packasm=4;
			}
		else
			{
			assemblycode[2]= (EA[SOURCE].immediatedata & 0xffff);
			packasm=3;
			}
		break;
#endif
	case 0:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			{
			print("ERROR 11:immediate data/size option error ===> ");
			print(argv);
			print("\n");
			return(-1);
			}
		assemblycode[1]= EA[SOURCE].immediatedata;
		packasm=2;
		break;
	case 64:/*BRA*/
	case 65:/*BSR*/
	case 66:/*Bxx*/
		if(instructnum==66)
			assemblycode[1]= (assemblycode[1] | (condition <<8));

		/* IMMEDIATE DATA */
		if(EA[SOURCE].mode==7 && EA[SOURCE].Aregnum!=0 && EA[SOURCE].Aregnum!=1)
			EA[SOURCE].immediatedata=EA[SOURCE].immediatedata -2; 
		/* ABSOLUTE DATA */
		else
			EA[SOURCE].immediatedata=EA[SOURCE].dis16-start-2; 
		if((EA[SOURCE].immediatedata % 2) != 0)
			{
			packasm=-1;
			print("ERROR 10:illegal change of flow ===> ");
			print(argv);
			print("\n");
			break;
			}
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(
(size==0 && (EA[SOURCE].immediatedata != (EA[SOURCE].immediatedata & 0xff  ))) || 
(size==1 && (EA[SOURCE].immediatedata != (EA[SOURCE].immediatedata & 0xffff))))
			{
			packasm=-1;
			print("ERROR 16:Out of displacement range   ===> ");
			print(argv);
			print("\n");
			break;
			}
		if((EA[SOURCE].immediatedata == 0 && (sizechar[0] == 'b' || sizechar[0] == 'B' )))
			{
			packasm=-1;
			print("ERROR 15:illegal displacement  ===> ");
			print(argv);
			print("\n");
			break;
			}
		if(size==0)	
			{
			assemblycode[1]= (assemblycode[1] | (EA[SOURCE].immediatedata & 0xff));
			packasm=2;
			break;
			}
		else if(size==1)	
			{
			assemblycode[2]= (EA[SOURCE].immediatedata & 0xffff);
			packasm=3;
			break;
			}
		else if(size==2)	
			{
			assemblycode[1]=assemblycode[1] | (0xff);
			assemblycode[2]=(EA[SOURCE].immediatedata>>16);
			assemblycode[3]=(EA[SOURCE].immediatedata & 0xffff);
			packasm=4;
			break;
			}
		break;
	case 67:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (EA[SOURCE].immediatedata));
		packasm=2;
		break;
	case 73:
	case 74:
	case 78:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (0x1<<8) | (size<<6) | (EA[SOURCE].Aregnum));
		packasm=2;
		break;
	case 68:
	case 69:
	case 81:
	case 82:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (EA[SOURCE].Aregnum));
		packasm=2;
		break;
	case 89:
	case 90:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (EA[DEST].Aregnum<<9) | (size<<6) | (EA[SOURCE].Aregnum));
		packasm=2;
		break;
	case 83:
	case 84:
	case 85:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum<<9) | (EA[DEST].Aregnum));
		packasm=2;
		break;
	case 98:
	case 100:
	case 102:
	case 104:
	case 109:
	case 112:
	case 115:
	case 118:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | ((EA[SOURCE].immediatedata & 0x7)<<9) | (size<<6) | (EA[DEST].Aregnum));
		packasm=2;
		break;
	case 99:
	case 101:
	case 103:
	case 105:
	case 110:
	case 113:
	case 116:
	case 119:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum<<9) | (size<<6) | (EA[DEST].Aregnum));
		packasm=2;
		break;
	case 106:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (size<<6) | (EA[SOURCE].Aregnum));
		packasm=2;
		break;
#if (DEVICE >= 68010)
	case 130:
		assemblycode[1]= (assemblycode[1]) | (EA[SOURCE].immediatedata & 0x7);
		packasm=2;
		break;
#endif
#if (DEVICE >= 68020)
	case 139:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].Aregnum));
		packasm=2;
		break;
	case 133:
		assemblycode[1]= (assemblycode[1]) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum);
		packasm=2;
		break;
#endif

/* ******************************************************************** */
/* **************************68030 INSTRUCTIONS************************ */
/* ******************************************************************** */
/* ******************************************************************** */

#if (DEVICE==68030)
	case 160:
	case 161:
	case 162:
	case 163:
	case 164:
	case 165:
	case 166:
	case 167:
	case 168:
	case 169:
	case 170:
	case 171:
		switch(instructnum)
		{
			case 160:
				assemblycode[1]= (assemblycode[1] | (0x10));
				assemblycode[2]= (0x2400);
				packasm=3;
				break;
			case 161:
				assemblycode[1]= (assemblycode[1]) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum);
				assemblycode[2]= (0x3800 | ((EA[DEST].immediatedata & 0x7)<<5) | EA[SOURCE].immediatedata);
				packasm=3;
				EApackassembly(DEST);		
				break;
			case 162:
				assemblycode[1]= (assemblycode[1]) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum);
				assemblycode[2]= (0x3000 | ((EA[DEST].immediatedata & 0x7)<<5) | EA[SOURCE].immediatedata);
				packasm=3;
				break;
			case 163:
				assemblycode[1]= (assemblycode[1]) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum);
				assemblycode[2]= (0x2200 | EA[SOURCE].immediatedata);
				packasm=3;
				EApackassembly(DEST);		
				break;
			case 164:
				assemblycode[1]= (assemblycode[1]) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum);
				assemblycode[2]= (0x2000 | EA[SOURCE].immediatedata);
				packasm=3;
				EApackassembly(DEST);		
				break;
			case 165:
			case 166:
			case 167:
				if(instructnum==165)
					{
					assemblycode[1]= (assemblycode[1]) | (EA[DEST].mode<<3) | (EA[DEST].Aregnum);
					assemblycode[2]= 0x0200;
					}
				else if(instructnum==166)
					{
					assemblycode[1]= (assemblycode[1]) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum);
					}
				else
					{
					assemblycode[1]= (assemblycode[1]) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum);
					assemblycode[2]= 0x0100;
					}
				switch(EA[SOURCE].immediatedata)
				{
					case 0:
						assemblycode[2]=assemblycode[2] | 0x4800;
						break;
					case 1:
						assemblycode[2]=assemblycode[2] | 0x4c00;
						break;
					case 2:
						assemblycode[2]=assemblycode[2] | 0x4000;
						break;
					case 3:
						assemblycode[2]=assemblycode[2] | 0x0800;
						break;
					case 4:
						assemblycode[2]=assemblycode[2] | 0x0c00;
						break;
					case 5:
						assemblycode[2]=assemblycode[2] | 0x6000;
						break;
				}
				packasm=3;
				if(instructnum==165)
					{
					EApackassembly(DEST);		
					}
				else
					{
					EApackassembly(SOURCE);		
					}
				break;
			case 168:
			case 169:
			case 170:
			case 171:
				assemblycode[1]= (assemblycode[1]) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum);
				if(instructnum<=169)
					assemblycode[2]= 0x8200;
				else
					assemblycode[2]= 0x8000;
				if(instructnum==168 || instructnum==170)
					assemblycode[2]= assemblycode[2] | ((EA[DEST].immediatedata & 0x7) <<10) | 0x0100 | (EA[DEST].Aregnum<<5) | EA[SOURCE].immediatedata;
				else
					assemblycode[2]= assemblycode[2] | ((EA[DEST].immediatedata & 0x7) <<10) | EA[SOURCE].immediatedata;
				packasm=3;
				EApackassembly(SOURCE);		
				break;
		}
		break;
#endif

/* ******************************************************************** */
/* **************************68040 INSTRUCTIONS************************ */
/* ******************************************************************** */
/* ******************************************************************** */

#if ((DEVICE==68040) || (COPROCESSOR==TRUE))
	case 203:/*FBxx*/
		if(sizechar[0] == '~')
			{
			sizechar[0] = 'w';
			sizechar[1] = ENDSTR;
			}
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		assemblycode[1]= (assemblycode[1] | (1<<0x7) | (size<<6) | predicate);

		/* IMMEDIATE DATA */
		if(EA[SOURCE].mode==7 && EA[SOURCE].Aregnum!=0 && EA[SOURCE].Aregnum!=1)
			EA[SOURCE].immediatedata=EA[SOURCE].immediatedata -2; 
		/* ABSOLUTE DATA */
		else
			EA[SOURCE].immediatedata=EA[SOURCE].dis16-start-2; 

		if((EA[SOURCE].immediatedata % 2) != 0)
			{
			packasm=-1;
			print("ERROR 10:illegal change of flow ===> ");
			print(argv);
			print("\n");
			break;
			}

		if((size==0 && (EA[SOURCE].immediatedata != (EA[SOURCE].immediatedata & 0xffff))))
			{
			packasm=-1;
			print("ERROR 16:Out of displacement range   ===> ");
			print(argv);
			print("\n");
			break;
			}
		else if(size==0)	
			{
			assemblycode[2]= (EA[SOURCE].immediatedata & 0xffff);
			packasm=3;
			break;
			}
		else if(size==1)	
			{
			assemblycode[2]=(EA[SOURCE].immediatedata>>16);
			assemblycode[3]=(EA[SOURCE].immediatedata & 0xffff);
			packasm=4;
			break;
			}
		break;
	case 204:
		assemblycode[1]= ((assemblycode[1]) | (1<<0x6) | (1<<0x3) | (EA[SOURCE].Aregnum));

		/* IMMEDIATE DATA */
		if(EA[DEST].mode==7 && EA[DEST].Aregnum!=0 && EA[DEST].Aregnum!=1)
			EA[DEST].immediatedata=EA[DEST].immediatedata -2; 
		/* ABSOLUTE DATA */
		else
			EA[DEST].immediatedata=EA[DEST].dis16-start-2; 
		if((EA[DEST].immediatedata % 2) != 0)
			{
			packasm=-1;
			print("ERROR 10:illegal change of flow ===> ");
			print(argv);
			print("\n");
			break;
			}
		assemblycode[2]= predicate;
		assemblycode[3]= (EA[DEST].immediatedata & 0xffff);
		packasm=4;
		break;
	case 205:
	case 206:
		size=sizedef(instructnum,&EA[DEST].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		if(EA[DEST].mode==4 && instructnum==205)/* REVERSE */
			{
			for(i=0;i<=7;i++)
				{
				if((EA[SOURCE].immediatedata>>7-i) & (0x1))
					assemblycode[2]= (assemblycode[2] |(0x1<<i));
				}
			}
		else if(instructnum==205)/* NO REVERSE */
			assemblycode[2]= EA[SOURCE].immediatedata | (0x1<<12);
		else
			assemblycode[2]= EA[SOURCE].Aregnum<<4 | (0x1<<12);

		if(EA[DEST].mode==4 && instructnum==205)
			assemblycode[2]= assemblycode[2] | 0xe000;
		else if(instructnum==205)
			assemblycode[2]= assemblycode[2] | 0xf000;
		else if(EA[DEST].mode==4 && instructnum==206)
			assemblycode[2]= assemblycode[2] | 0xe800;
		else
			assemblycode[2]= assemblycode[2] | 0xf800;
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 207:
	case 208:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		if(instructnum==207)
			assemblycode[2]= EA[DEST].immediatedata | (0x1<<12);
		else
			assemblycode[2]= (EA[DEST].Aregnum<<4) | (0x1<<12);
		if(instructnum==207)
			assemblycode[2]= assemblycode[2] | 0xd000;
		else 
			assemblycode[2]= assemblycode[2] | 0xd800;
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 209:
		assemblycode[1]= assemblycode[1] | (1<<0x7);
		packasm=3;/* Note: assemblycode[2] should already equal 0 */
		break;
	case 210:
		assemblycode[1]= (assemblycode[1] | (5<<0x6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		packasm=2;
		EApackassembly(SOURCE);		
		break;
	case 211:
		assemblycode[1]= (assemblycode[1] | (4<<0x6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		packasm=2;
		EApackassembly(SOURCE);		
		break;
	case 212:
		assemblycode[1]= (assemblycode[1] | (1<<0x6) | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= predicate;
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 213:
		assemblycode[1]= (assemblycode[1] | (0x7c));
		assemblycode[2]= predicate;
		packasm=3;
		break;
	case 214:
		if(sizechar[0] == '~')
			{
			sizechar[0] = 'l';
			sizechar[1] = ENDSTR;
			}
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			{
			print("ERROR 11:immediate data/size option error ===> ");
			print(argv);
			print("\n");
			return(-1);
			}
		assemblycode[1]= (assemblycode[1] | (0xf<<0x3) | (size));
		assemblycode[2]= predicate;
		if(size==3)
			{
			assemblycode[3]= (EA[SOURCE].immediatedata>>16);
			assemblycode[4]= (EA[SOURCE].immediatedata & 0xffff);
			packasm=5;
			}
		else
			{
			assemblycode[3]= (EA[SOURCE].immediatedata & 0xffff);
			packasm=4;
			}
		break;
	case 215:
	case 216:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (0x1<<14) | (size<<10) | (EA[DEST].Aregnum<<7);
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 217:
	case 218:
		size=sizedef(instructnum,&EA[DEST].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]= (0x3<<13) | (size<<10) | (EA[SOURCE].Aregnum<<7);
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 219:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		assemblycode[2]= (EA[SOURCE].Aregnum<<10) | (EA[DEST].Aregnum<<7) ;
		packasm=3;
		break;
	case 220:
	case 221:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (0x1<<15) | (EA[DEST].Aregnum<<10);
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 222:
	case 223:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]= (0x5<<13) | (EA[SOURCE].Aregnum<<10);
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 224:
	case 225:
	case 226:
		assemblycode[1]= (assemblycode[1] | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]= (0x5<<13) | (EA[SOURCE].Aregnum<<10);
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 227:
	case 228:
	case 229:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (0x4<<13) | (EA[DEST].Aregnum<<10);
		packasm=3;
		EApackassembly(SOURCE);		
		break;

/* THIS IS WHERE DYADIC AND MONADIC INSTRUCTIONS ARE ASSEMBLED */

	case 230:
	case 231:
	case 232:
	case 233:
		if(instructnum==233 && (predicate<= 0x1 || predicate>= 0x1f))
				error=TRUE;
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		switch(instructnum)
		{
			case 230:
			case 231:
				assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
				assemblycode[2]= (0x1<<14) | (size<<10) | (EA[DEST].Aregnum<<7) | predicate;
				packasm=3;
				EApackassembly(SOURCE);		
				break;
			case 232:
				assemblycode[2]= (EA[SOURCE].Aregnum<<10) | (EA[DEST].Aregnum<<7) | predicate;
				packasm=3;
				break;
			case 233:
				assemblycode[2]= (EA[SOURCE].Aregnum<<10) | (EA[SOURCE].Aregnum<<7) | predicate;
				packasm=3;
				break;
		}
		break;

/*  End of Monadic/Dyadic Assembly */


	case 234:
	case 235:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (0x1<<14) | (size<<10) | (0x3a);
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 236:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (EA[SOURCE].Aregnum<<10) | (0x3a);
		packasm=3;
		break;
#endif
#if(COPROCESSOR==TRUE || EMULATOR==TRUE)

/* Insert other coprocessor instructions here */
/* ie. Instructions not implemented in the 68040 */

	case 237:
		size=sizedef(instructnum,&EA[DEST].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[DEST].mode<<3) | (EA[DEST].Aregnum));
		assemblycode[2]= (0x3<<13) | (size<<10) | (EA[SOURCE].Aregnum<<7) | predicate;
		packasm=3;
		EApackassembly(DEST);		
		break;
	case 238:
		assemblycode[2]= (0x17<<10) | (EA[DEST].Aregnum<<7) | (EA[SOURCE].immediatedata & 0x7f);
		packasm=3;
		break;
	case 239:
	case 240:
		size=sizedef(instructnum,&EA[SOURCE].immediatedata);
		if(error)
			break;
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (0x2<<13) | (size<<10) | (0x6<<3) | (EA[DEST].Aregnum) ;
		packasm=3;
		EApackassembly(SOURCE);		
		break;
	case 241:
		assemblycode[1]= (assemblycode[1] | (EA[SOURCE].mode<<3) | (EA[SOURCE].Aregnum));
		assemblycode[2]= (EA[SOURCE].Aregnum<<10) | (0x6<<3) | (EA[DEST].Aregnum) ;
		packasm=3;
		break;

#endif

	case 41:/* INSTRUCTION FULLY DEFINED UPON MATCH */
	case 50:
	case 51:
	case 53:
	case 54:
	case 55:
	case 56:
		packasm=2;
		break;
	default:
		break;
}

if(packasm >= 0)
	{
	returnsize=packassembly(start);
	return(returnsize);
	}
else
	return(packasm);/* AN ERROR MESSAGE HAS ALREADY BEEN PRINTED */
}

/* ************************************************************ */
/* ROUTINE: EApackassembly					*/
/* ************************************************************ */

EApackassembly(SorD)
int SorD;
{
extern int packasm;	/*the next available location in the array*/
extern int assemblycode[];
extern ACTUALFIELD_T_F Fieldoption[];
extern EASPEC EA[];
extern char sizechar[];
extern int MIPOST;
int i;

#if(DEVICE==68040 || COPROCESSOR==TRUE)
int fration_int;
#endif

switch(EA[SorD].mode)
{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		break;
	case 5:
		assemblycode[packasm]=(0xffff & EA[SorD].dis16);
		packasm++;
		break;
	case 6:
	case 7:
		if(EA[SorD].mode==7 && (EA[SorD].Aregnum==0||EA[SorD].Aregnum==2))
		{
			assemblycode[packasm]=(EA[SorD].dis16 & 0xffff);
			packasm++;
			break;
		}
		else if(EA[SorD].mode==7 && EA[SorD].Aregnum==1)
		{
			assemblycode[packasm]=((EA[SorD].dis16 >>16) & 0xffff);
			assemblycode[packasm+1]=(EA[SorD].dis16 & 0xffff);
			packasm=packasm+2;
			break;
		}
		else if(EA[SorD].mode==7 && EA[SorD].Aregnum==4)
		{
		switch(sizechar[0])
			{
			case 'b':
			case 'B':
				assemblycode[packasm]=(EA[SorD].immediatedata & 0xff);
				packasm++;
				break;
			case 'w':
			case 'W':
				assemblycode[packasm]=(EA[SorD].immediatedata & 0xffff);
				packasm++;
				break;

			case 'l':
			case 'L':
				assemblycode[packasm]=((EA[SorD].immediatedata >>16) & 0xffff);
				assemblycode[packasm+1]=(EA[SorD].immediatedata & 0xffff);
				packasm=packasm+2;
				break;
#if(DEVICE==68040 || COPROCESSOR==TRUE)
			case 's':
			case 'S':
				assemblycode[packasm]=(EA[SorD].s_fp <<15) | (EA[SorD].e_fp << 7) | (EA[SorD].f1_fp>>16);
				assemblycode[packasm+1]=(EA[SorD].f1_fp & 0xffff);
				packasm=packasm+2;
				break;
			case 'd':
			case 'D':
				assemblycode[packasm]=(EA[SorD].s_fp <<15) | (EA[SorD].e_fp << 4) | (EA[SorD].f1_fp>>16);
				assemblycode[packasm+1]=(EA[SorD].f1_fp & 0xffff);
				assemblycode[packasm+2]=(EA[SorD].f2_fp >> 16);
				assemblycode[packasm+3]=(EA[SorD].f2_fp & 0xffff);
				packasm=packasm+4;
				break;
			case 'x':
			case 'X':
				assemblycode[packasm]=(EA[SorD].s_fp <<15) | EA[SorD].e_fp;
				assemblycode[packasm+1]=0;
				assemblycode[packasm+2]=(0x1<<15) | (EA[SorD].f1_fp >> 16);
				assemblycode[packasm+3]=(EA[SorD].f1_fp & 0xffff);
				assemblycode[packasm+4]=(EA[SorD].f2_fp >> 16);
				assemblycode[packasm+5]=(EA[SorD].f2_fp & 0xffff);
				packasm=packasm+6;
				break;
			case 'p':
			case 'P':
				assemblycode[packasm]=(EA[SorD].s_fp <<15) | (EA[SorD].s1_fp << 14) | EA[SorD].e_fp;
				assemblycode[packasm+1]=EA[SorD].p_fp;
				assemblycode[packasm+2]=(0x1<<16) | (EA[SorD].f1_fp >> 16);
				assemblycode[packasm+3]=(EA[SorD].f1_fp & 0xffff);
				assemblycode[packasm+4]=(EA[SorD].f2_fp >> 16);
				assemblycode[packasm+5]=(EA[SorD].f2_fp & 0xffff);
				packasm=packasm+6;
				break;
#endif
			}
		break;
		}
		assemblycode[packasm]=((EA[SorD].indexregtype<<15) |							(EA[SorD].indexregnum<<12) |							(EA[SorD].indexsize  <<11) |							(EA[SorD].scale	    << 9 ));
		if(Fieldoption[1].option==TRUE &&						   Fieldoption[2].option==TRUE &&						   Fieldoption[3].option==TRUE &&						   Fieldoption[4].option==FALSE &&						   EA[SorD].BDdisplacement==(EA[SorD].BDdisplacement & 0xff))
			{
			assemblycode[packasm]=assemblycode[packasm]|EA[SorD].BDdisplacement;
			packasm=packasm+1;
			break;
			}
		else
			assemblycode[packasm]=(								(assemblycode[packasm])|							(0x1<<8)|										(EA[SorD].BS<<7)|								(EA[SorD].IS<<6)|								(EA[SorD].BDsize<<4)|								(EA[SorD].index_indirect_selection));
		packasm=packasm+1;/* packasm is ready for BDdis. */
/* ACCOUNT FOR BASEDISPLACEMENT IF THERE IS ONE  */
		switch(EA[SorD].BDsize)
		{
			case 1:
				break;	
			case 2:	
				assemblycode[packasm]=EA[SorD].BDdisplacement;
				packasm++;
				break;
			case 3:
				assemblycode[packasm]=(EA[SorD].BDdisplacement>>16);
				assemblycode[packasm+1]=EA[SorD].BDdisplacement & 0xffff;
				packasm=packasm+2;
				break;
		}
/* ACCOUNT FOR OUTERDISPLACEMENT IF THERE IS ONE  */
		switch(EA[SorD].index_indirect_selection & 0x3)
		{
			case 0:
			case 1:
				break;	
			case 2:	
				assemblycode[packasm]=EA[SorD].ODdisplacement;
				packasm++;
				break;
			case 3:
				assemblycode[packasm]=(EA[SorD].ODdisplacement & 0xffff0000)>>16;
				assemblycode[packasm+1]=EA[SorD].ODdisplacement & 0xffff;
				packasm=packasm+2;
				break;
		}
		break;
}
}	


/* ************************************************************ */
/* ROUTINE: packassembly					*/
/* ************************************************************ */

int packassembly(start)
int start;
{
extern int packasm;/*the amount of the assemblycode array that should
		     be put into memory (minus 1)			*/
extern int assemblycode[];
int i,size;


size=start;
for(i=1;i<packasm;i++)
	{
	put16(size,(assemblycode[i] & 0xffff));
	size=size+2;
	}
return(size-start);
}	

/* ************************************************************ */





/* ************************************************************ */
/* ROUTINE: sizedef						*/
/* ************************************************************ */

int sizedef(instructnum,immediatedata)
int instructnum;/* instruction number in the assembly instruct. table     */
int *immediatedata;/* value to be used to determine size if sizechar unknown */
{
extern OPSTRUCTURE syntax1[];
extern char sizechar[];
extern int error;
#if(COPROCESSOR==TRUE)
extern COPROCSTRUCTURE syntax2[];
#endif

error = FALSE;
while(TRUE)
{
switch(sizechar[0])
{
	case '~':
#if(COPROCESSOR==TRUE || DEVICE==68040)
		if(instructnum>=200)
			sizechar[0]='x';
		else
		{
#endif
		if(*immediatedata==(*immediatedata & 0xff) || 
				(bitnum(31,8,*immediatedata)==0xffffff))
			{
			sizechar[0]='b';
			if(bitnum(31,8,*immediatedata)==0xffffff)
				*immediatedata=*immediatedata & 0xff;
			}
		else if(*immediatedata==(*immediatedata & 0xffff) || 
				(bitnum(31,16,*immediatedata)==0xffff))
			{
			sizechar[0]='w';
			if(bitnum(31,16,*immediatedata)==0xffff)
				*immediatedata=*immediatedata & 0xffff;
			}
		else 
			sizechar[0]='l';
#if(COPROCESSOR==TRUE || DEVICE==68040)
		}
#endif

#if(COPROCESSOR==TRUE || DEVICE==68040)
		if(instructnum<200)
		{
#endif
		if(sizechar[0]=='b' && sizefield[syntax1[instructnum].sizestr].size[0]!='b'
		     && (sizefield[syntax1[instructnum].sizestr].size[1]!='b')
		     && (sizefield[syntax1[instructnum].sizestr].size[2]!='b')
		     && (sizefield[syntax1[instructnum].sizestr].size[3]!='b')
		     && (sizefield[syntax1[instructnum].sizestr].size[4]!='b'))
			sizechar[0]='w';
		if(sizechar[0]=='w' && sizefield[syntax1[instructnum].sizestr].size[0]!='w'
		     && (sizefield[syntax1[instructnum].sizestr].size[1]!='w')
		     && (sizefield[syntax1[instructnum].sizestr].size[2]!='w')
		     && (sizefield[syntax1[instructnum].sizestr].size[3]!='w')
		     && (sizefield[syntax1[instructnum].sizestr].size[4]!='w'))
			sizechar[0]='l';
		if(sizechar[0]=='l' && sizefield[syntax1[instructnum].sizestr].size[0]!='l'
		     && (sizefield[syntax1[instructnum].sizestr].size[1]!='l')
		     && (sizefield[syntax1[instructnum].sizestr].size[2]!='l')
		     && (sizefield[syntax1[instructnum].sizestr].size[3]!='l')
		     && (sizefield[syntax1[instructnum].sizestr].size[4]!='l'))
			{	
			error=TRUE;
			return;
			}
#if(COPROCESSOR==TRUE || DEVICE==68040)
		}
		else if(sizefield[syntax2[instructnum-200].sizestr].size[0]!='x'
	     	   && sizefield[syntax2[instructnum-200].sizestr].size[1]!='x'
		   && sizefield[syntax2[instructnum-200].sizestr].size[2]!='x'
		   && sizefield[syntax2[instructnum-200].sizestr].size[3]!='x'
		   && sizefield[syntax2[instructnum-200].sizestr].size[4]!='x')
			{	
			error=TRUE;
			return;
			}
#endif
		continue;
	default:
#if(COPROCESSOR==TRUE || DEVICE==68040)
		if(instructnum<200)
		{
#endif
	if(sizefield[syntax1[instructnum].sizestr].size[0]==sizechar[0])
		return(0);
	else if(sizefield[syntax1[instructnum].sizestr].size[1]==sizechar[0])
		return(1);
	else if(sizefield[syntax1[instructnum].sizestr].size[2]==sizechar[0])
		return(2);
	else if(sizefield[syntax1[instructnum].sizestr].size[3]==sizechar[0])
		return(3);
	else if(sizefield[syntax1[instructnum].sizestr].size[4]==sizechar[0])
		return(4);
	else if(sizefield[syntax1[instructnum].sizestr].size[5]==sizechar[0])
		return(5);
	else if(sizefield[syntax1[instructnum].sizestr].size[6]==sizechar[0])
		return(6);

#if(COPROCESSOR==TRUE || DEVICE==68040)
		}
		else
		{
if(sizefield[syntax2[instructnum-200].sizestr].size[0]==sizechar[0])
	return(0);
else if(sizefield[syntax2[instructnum-200].sizestr].size[1]==sizechar[0])
	return(1);
else if(sizefield[syntax2[instructnum-200].sizestr].size[2]==sizechar[0])
	return(2);
else if(sizefield[syntax2[instructnum-200].sizestr].size[3]==sizechar[0])
	return(3);
else if(sizefield[syntax2[instructnum-200].sizestr].size[4]==sizechar[0])
	return(4);
else if(sizefield[syntax2[instructnum-200].sizestr].size[5]==sizechar[0])
	return(5);
else if(sizefield[syntax2[instructnum-200].sizestr].size[6]==sizechar[0])
	return(6);
		}
#endif

}/*switch*/
}/*while*/
}/*sizedef*/

/* ************************************************************	*/


/* ************************************************************ */
/* ROUTINE: sizechar_num					*/
/* ************************************************************ */

sizechar_num(sizechar,immediatedata)
char sizechar;      /* character that was typed in on the assembly line	      */
int immediatedata;  /* value to be used to determine size if sizechar unknown */
{
int comparisonvalue;
extern error;

error=FALSE;
switch(sizechar)
	{
	case 'b':
	case 'B':
		comparisonvalue=(immediatedata & 0xff);
		break;
	case 'w':
	case 'W':
		comparisonvalue=(immediatedata & 0xffff);
		break;
	case 'l':
	case 'L':
	case '~':
		comparisonvalue=immediatedata;
		break;
	}
if(immediatedata!=comparisonvalue)
	error=TRUE;
}

/* ************************************************************ */

/* ************************************************************ */
/* ROUTINE: syntaxsetup						*/
/* ************************************************************ */
int syntaxsetup(table_syntax,EAentrynum)
char *table_syntax;
int EAentrynum;
{
extern EAALLOWED_TYPE EAtype[];
int i;
int shifted_word;


for (i=0;i<=11;i++)
	{
	shifted_word=(EAtype[EAentrynum].allowableEA>>(11-i));
	if((shifted_word & 0x1)==1)
		table_syntax[i]='1';
	else
		table_syntax[i]='0';
	}
}
/* ************************************************************ */
