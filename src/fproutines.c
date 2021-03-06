#include "userdef.h"

#define MAXSTR 80
#define SOURCE 0
#define DEST 1

/* ********************************************************************* */
/* ROUTINE:assembly{}							 */
/* ********************************************************************* */
assembly(start,str)
int start;
char *str;

{
extern int instructnum;
extern predicate;

#if(DEVICE==68040)
extern optionSorD;/* intialized in initEAstructures() */
extern EASPEC EA[];

int i;
char teststring[MAXLINE];
#endif

int size;

instructnum=-1;
initEAstructures();
instrmatch(str);		/* should set instructnum */

/* ****************************************************** */
/* COPROCESSOR determines the extent of the search	  */

if(instructnum==-1 && (COPROCESSOR==TRUE || DEVICE==68040))
	{
	instrmatchfp(str);	/* This will check the entire extent of the
			   	   floating point table for a match.  The
				   extent of this table is determined at
				   compile time based on the COPROCESSOR,
				   EMULATOR and DEVICE defines. */


/* ********************************************************************	*/
/* Check specifically for a match on intstructnum 33 which is not	*/
/* allowed for the dyadic instructnums					*/
	if(instructnum==33 &&
		((predicate >=0x20 && predicate <=0x28) || predicate ==0x38))
			instructnum= -1;	
/*									*/
/* ********************************************************************	*/

	if(instructnum!=-1)
		{
		instructnum=instructnum + 200;	
		/* asm/disasm decodings start at 200 + instructnum */
		}
	}

#if (DEVICE==68040)
if(instructnum == -1)
	{
	if(str[1]=='s' || str[1]=='S')
		optionSorD='s';
	else if(str[1]=='d' || str[1]=='D')
		optionSorD='d';
	else
		optionSorD=ENDSTR;

	if(optionSorD!=ENDSTR)
		{
		for(i=0;str[i]!=ENDSTR;i++)
			teststring[i]=str[i];
		teststring[i]=ENDSTR;
		teststring[1]=teststring[0];
		shiftarg(teststring,1);
		instrmatchfp(teststring);
		if(instructnum!=-1)
			{
			if((instructnum>=15 && instructnum<=23) 
			  || (instructnum>=30 && instructnum<=33))

/* NOTE: This still leaves the fcmp ==> fscmp or fdcmp options allowable
	 and will have to eliminate these possibilities in instrmatchfp */

				instructnum=instructnum + 200;	
			else
				instructnum = -1;
			/* asm/disasm decodings start at 200 and have 
			   conditionals built into them to check the 
			   SorDfp variable */ 
			}
		}
	}
#endif
/* ********************************************** */


if(instructnum!=-1)
{
	size=codeassembly(start,instructnum);
	if(size > 0)
	{
		disasm(start);
		print("\n");
	}
	return(size);
}
return(0);
}
/*  0 = unidentifiable error/no assembly made encountered on assembly 	*/
/* -1 = illegal flow change						*/
/* assembly end{}							*/




#if ((COPROCESSOR==TRUE) || (DEVICE==68040))

/* ******************************************************************* */
/* ROUTINE:instrmatchfp{}					       */
/* ******************************************************************* */
instrmatchfp(str)
char *str;
{


/* MAXCOPROCINST is the entire extent of table here 	*/
/* predicate should be used here 			*/

extern COPROCSTRUCTURE syntax2[];
extern MONADICDYADIC MONAorDYA[];
#if (DEVICE==68040)
extern optionSorD;
#endif
extern COPROCCONDITIONALS typepredicate[];
extern COPROCCONTROLREG COPROCCRreg[];
extern SIZETYPES sizefield[];
extern EAALLOWED_TYPE EAtype[];
extern int predicate;
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
int maxentry,g,h,i,j,k,l,cr1,cr2;
int int1,int2;
int a,Bmatch,instructmatch;
int bitfieldvar;
char table_syntax[MAXLINE];
char teststring[MAXLINE];
char teststringfc[MAXLINE];
/* ******************************** */


instructmatch=0;
for (i=0;i<=MAXCOPROCINST;i++)	/* this uses that value to tell how 
					   many entries are in that table */
{
	predicate=-1;
	Bmatch=FALSE;
	for(j=0;(syntax2[i].name[j] == str[j]) && (syntax2[i].name[j] !=ENDSTR);j++)
		;
	if(i==15)
		predicate=0;
	if(syntax2[i].name[j]=='~') 
		{
		if(i==4 || i==12 || i==3 || i==13 || i==14)
			{
			for(k=0;(k<=31 && predicate==-1);k++)
			{
			if((k==0 || k==15) 
		&& ((str[j]==typepredicate[k].condition[0]) 
		|| (str[j]==typepredicate[k].condition[0] + 0x20)))
				{
				j++;
				predicate=k;
				break;
				}
			else if((k==1 || k==7 || k==8 || k==14 || k==16 || k==18
	 	|| k==19 || k==20 || k==21 || k==22 || k==31)
		&& (str[j]==typepredicate[k].condition[0] 
		|| str[j]==typepredicate[k].condition[0]+0x20) 
		&& (str[j+1]==typepredicate[k].condition[1] 
		|| str[j+1]==typepredicate[k].condition[1]+0x20))
					{
					j++;
					j++;
					predicate=k;
					break;
					}
			else if((k==24)
		&& (str[j]==typepredicate[k].condition[0] 
		|| str[j]==typepredicate[k].condition[0]+0x20) 
		&& (str[j+1]==typepredicate[k].condition[1] 
		|| str[j+1]==typepredicate[k].condition[1]+0x20) 
		&& (str[j+2]==typepredicate[k].condition[2] 
		|| str[j+2]==typepredicate[k].condition[2]+0x20) 
		&& (str[j+3]==typepredicate[k].condition[3] 
		|| str[j+3]==typepredicate[k].condition[3]+0x20))
					{
					j++;
					j++;
					j++;
					j++;
					predicate=k;
					break;
					}
			else if(
		(str[j]==typepredicate[k].condition[0] 
		|| str[j]==typepredicate[k].condition[0]+0x20) 
		&& (str[j+1]==typepredicate[k].condition[1] 
		|| str[j+1]==typepredicate[k].condition[1]+0x20) 
		&& (str[j+2]==typepredicate[k].condition[2] 
		|| str[j+2]==typepredicate[k].condition[2]+0x20))
					{
					j++;
					j++;
					j++;
					predicate=k;
					break;
					}
			}/* end for */
			/*NOTE: j now points to the '.' position of the syntax*/
			if((str[j]==' ' || str[j]=='\t' || str[j]==ENDSTR) && predicate !=-1)
			{
				Bmatch=TRUE;
				sizechar[0]='~';
				sizechar[1]=ENDSTR;
			}
			else if(str[j]=='.' && instructnum!=13 && str[j+1]!='~' &&		    	predicate != -1
&&		       (str[j+1]== sizefield[syntax2[i].sizestr].size[0] ||
			str[j+1]== sizefield[syntax2[i].sizestr].size[1] ||
			str[j+1]== sizefield[syntax2[i].sizestr].size[2] ||
			str[j+1]== sizefield[syntax2[i].sizestr].size[3] ||
			str[j+1]== sizefield[syntax2[i].sizestr].size[4] ||
			str[j+1]== sizefield[syntax2[i].sizestr].size[5] ||
			str[j+1]== sizefield[syntax2[i].sizestr].size[6]))
			{
				Bmatch=TRUE;
				sizechar[0]=str[j+1];
				sizechar[1]=ENDSTR;
			}
			j=0;/* ie. a point where there will never be a '.' or
				ENDSTR so that it won't match the next 2 if 
				statements				*/
		}/* end if 4 etc... */
		else if(i>=30 && i<=33)
		{
		for(g=0; g<=MONADICSRCH; g++)
			{
			for(h=0;(MONAorDYA[g].instruction[h] == str[h]) && (MONAorDYA[g].instruction[h] !=ENDSTR);h++)
				;
			if(MONAorDYA[g].instruction[h]==ENDSTR && 
				(str[h]==' ' || str[h]=='\t'))
				{
				Bmatch=TRUE;
				sizechar[0]='~';
				sizechar[1]=ENDSTR;
				predicate=MONAorDYA[g].extension;
				}
			else if(MONAorDYA[g].instruction[h]==ENDSTR && 
				str[h]=='.' && 
		       (str[h+1]== sizefield[syntax2[i].sizestr].size[0]||
			str[h+1]== sizefield[syntax2[i].sizestr].size[1]||
			str[h+1]== sizefield[syntax2[i].sizestr].size[2]||
			str[h+1]== sizefield[syntax2[i].sizestr].size[3]||
			str[h+1]== sizefield[syntax2[i].sizestr].size[4]||
			str[h+1]== sizefield[syntax2[i].sizestr].size[5]||
			str[h+1]== sizefield[syntax2[i].sizestr].size[6]) &&
			(str[h+2]==' ' || str[h+2]=='\t'))
				{
				Bmatch=TRUE;
				sizechar[0]=str[h+1];
				sizechar[1]=ENDSTR;
				predicate=MONAorDYA[g].extension;
				}
			j=0;/* ie. a point where there will never be a '.' or
				ENDSTR so that it won't match the next 2 if 
				statements				*/
		}/* end for */
		}/* end else MONADIC or DYADIC search */
		}/* end conditional check */

	if((syntax2[i].name[j]=='.' || syntax2[i].name[j]==ENDSTR) && 
		       (str[j]==' ' || 
		        str[j]=='\t' || 
			str[j]==ENDSTR))
		{
		Bmatch=TRUE;
		sizechar[0]='~';
		sizechar[1]=ENDSTR;
		}
	else if(syntax2[i].name[j]==ENDSTR && syntax2[i].name[j-1]=='.' && 
		       (str[j]== sizefield[syntax2[i].sizestr].size[0] ||
			str[j]== sizefield[syntax2[i].sizestr].size[1] ||
			str[j]== sizefield[syntax2[i].sizestr].size[2] ||
			str[j]== sizefield[syntax2[i].sizestr].size[3] ||
			str[j]== sizefield[syntax2[i].sizestr].size[4] ||
			str[j]== sizefield[syntax2[i].sizestr].size[5] ||
			str[j]== sizefield[syntax2[i].sizestr].size[6]))
		{
		Bmatch=TRUE;
		sizechar[0]=str[j];
		sizechar[1]=ENDSTR;
		}

#if (DEVICE==68040)
/* ************************************************************************ */
/* This is the check to determine if a single or double precision operation
	match has been made to an instruction that is not allowed */

	if(optionSorD != ENDSTR && 
		predicate != 0x18 &&
		predicate != 0x22 &&
		predicate != 0x20 &&
		predicate != 0x00 &&
		predicate != 0x23 &&
		predicate != 0x1a &&
		predicate != 0x04 &&
		predicate != 0x28) 
				Bmatch=FALSE;
/* ************************************************************************ */
	if(optionSorD != ENDSTR && predicate==0x04)
		predicate = predicate - 0x3;
	if (optionSorD == 's')
		predicate = predicate + 0x40;
	if (optionSorD == 'd')
		predicate = predicate + 0x44;
#endif


	if(Bmatch==TRUE)
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
			int2=syntax2[instructmatch].source;
		else
			int2=syntax2[instructmatch].dest;

		if(int1 == 1)
			table_syntax[l++]=',';

		if(EAtype[int2].allowableEA > 0xfff)
		{

			switch(EAtype[int2].allowableEA >> 12)
			{
				case 4:/* 'N' for NONE */
					if(int1==0)
						table_syntax[l++]='N';
					break;
				case 20:/* 'H' for FP RegList */
					table_syntax[l++]='H';
					break;
				case 21:/* 'I' for FPn or FPm register */
					table_syntax[l++]='I';
					break;
				case 22:/* 'J' <ea>{Dn} or <ea>{#k}  */
					table_syntax[l++]='J';
					syntaxsetup(&table_syntax[l],0);
					l=l+12;
					break;
				case 23:/* 'K' for a single FP control reg */
					table_syntax[l++]='K';
					break;
				case 24:/* 'L' for FPIAR only */
					table_syntax[l++]='L';
					break;
				case 25:/* FPm:FPn */
					table_syntax[l++]='P';
					break;
				case 26:/* FPcr List */
					table_syntax[l++]='O';
					break;
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
		SorD=SOURCE;		/* set to source */
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
			SorD=DEST;
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
				for(k=0;COPROCCRreg[j].reg[k]!=ENDSTR;k++)
					{
					if(COPROCCRreg[j].reg[k]!=teststring[k])
						cr1=FALSE;
					if(COPROCCRreg[j].reg[k]!=teststring[l+k])
						cr2=FALSE;
					}
				if(cr1==TRUE || cr2==TRUE)
					break;
				}
			if(((teststring[0]=='a'||teststring[0]=='A') ||
			    (teststring[0]=='d'||teststring[0]=='D'))
				&& teststring[1]>='0' 
				&& teststring[1]<='7' 
				&& teststring[2]==','
				&& cr2)
				{
				EA[SOURCE].immediatedata=((teststring[0] & 0x1)<<15) | (teststring[1]-'0'<<12) | (COPROCCRreg[j].field);
				EA[SOURCE].Aregnum=1;
				}
			else if(((teststring[l]=='a'||teststring[l]=='A') ||
			    (teststring[l]=='d'||teststring[l]=='D'))
				&& teststring[l+1]>='0' 
				&& teststring[l+1]<='7' 
				&& teststring[l+2]==ENDSTR
				&& cr1)
				{
				EA[SOURCE].immediatedata=((teststring[l] & 0x1)<<15) | (teststring[l+1]-'0'<<12) | (COPROCCRreg[j].field);
				EA[SOURCE].Aregnum=0;
				}
			else
				{
					error=TRUE;
					continue;
				}
			shiftarg(table_syntax,1);
			}
		else if(table_syntax[0]=='H')
			{
			if((teststring[0]=='f' || teststring[0]=='F') &&
			(teststring[1]=='p' || teststring[1]=='P') &&	
				(teststring[2]>='0' && teststring[2]<='7') &&
				(teststring[3]=='/' || teststring[3]==',' || teststring[3]==ENDSTR))
			{
				EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<7-(teststring[2]-'0'));	
				shiftarg(&teststring[0],3);
			}
			else if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[4]=='f' || teststring[4]=='F') &&
				(teststring[5]=='p' || teststring[5]=='P') &&
				(teststring[2]>='0' && teststring[2]<='7') &&
				(teststring[3]=='-') &&
				(teststring[6]>='0' && teststring[6]<='7') &&
				(teststring[7]=='/' || teststring[7]==',' || teststring[7]==ENDSTR))
			{
				for(l=(teststring[2]-'0');l<=(teststring[6]-'0');l++)
					EA[SorD].immediatedata=EA[SorD].immediatedata | (0x1<<(7-l));	
				shiftarg(&teststring[0],7);
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
				(teststring[2]>='0' && teststring[2]<='7') &&
				(teststring[3]==','|| teststring[3]==ENDSTR || teststring[3]=='{'))
				{
				EA[SorD].Aregnum= teststring[2]-'0';
				shiftarg(table_syntax,1);		
				shiftarg(&teststring[0],4);		
				}
			else
			{
				error=TRUE;
				continue;
			}
			}
		else if(table_syntax[0]=='J')
			{
			for(l=0;l<12;l++)
				allowableEA[l]=table_syntax[1+l];	
			allowableEA[l]=ENDSTR;	
			shiftarg(table_syntax,13);
			EAallowed(&teststring[0],SorD,allowableEA);		
			if(error)
				{
				error=TRUE;
				continue;
				}
			if(teststring[0]=='#')
				{
				shiftarg(&teststring[0],1);
				predicate = getnum(&teststring[0],NOERR,DEFAULTSCALE);
				}
			else if((teststring[0] == 'd' || teststring[0]=='D') &&
				teststring[1] >= '0' && teststring[1] <= '7')
				{
				predicate = (teststring[1] - '0') <<4;
				shiftarg(&teststring[0],3);
				}
			else
				{
				error=TRUE;
				continue;
				}
			}
		else if(table_syntax[0]=='L')
			{
			if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[2]=='i' || teststring[2]=='I') &&
				(teststring[3]=='a' || teststring[3]=='A') &&
				(teststring[4]=='r' || teststring[4]=='R') &&
				(teststring[5]==','|| teststring[5]==ENDSTR || teststring[5]=='{'))
				{
				EA[SorD].Aregnum=0x1;	
				shiftarg(&teststring[0],6);
				shiftarg(table_syntax,1);
				}
			else
				{
				error=TRUE;
				continue;
				}
			}
		else if(table_syntax[0]=='K')
			{
			if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[2]=='c' || teststring[2]=='C') &&
				(teststring[3]=='r' || teststring[3]=='R') &&
				(teststring[4]==','|| teststring[4]==ENDSTR || teststring[4]=='{'))
				{
				EA[SorD].Aregnum=0x4;	
				shiftarg(&teststring[0],5);
				shiftarg(table_syntax,1);
				}
			else if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[2]=='s' || teststring[2]=='S') &&
				(teststring[3]=='r' || teststring[3]=='R') &&
				(teststring[4]==','|| teststring[4]==ENDSTR || teststring[4]=='{'))
				{
				EA[SorD].Aregnum=0x2;	
				shiftarg(&teststring[0],5);
				shiftarg(table_syntax,1);
				}
			else if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[2]=='i' || teststring[2]=='I') &&
				(teststring[3]=='a' || teststring[3]=='A') &&
				(teststring[4]=='r' || teststring[4]=='R') &&
				(teststring[5]==','|| teststring[5]==ENDSTR || teststring[5]=='{'))
				{
				EA[SorD].Aregnum=0x1;	
				shiftarg(&teststring[0],6);
				shiftarg(table_syntax,1);
				}
			else
				{
				error=TRUE;
				continue;
				}
			}
		else if(table_syntax[0]=='O')
			{
			if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[2]=='c' || teststring[2]=='C') &&
				(teststring[3]=='r' || teststring[3]=='R') &&
				(teststring[4]==','|| teststring[4]==ENDSTR || teststring[4]=='/'))
				{
				EA[SorD].Aregnum=EA[SorD].Aregnum | 0x4;	
				shiftarg(&teststring[0],4);
				}
			else if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[2]=='s' || teststring[2]=='S') &&
				(teststring[3]=='r' || teststring[3]=='R') &&
				(teststring[4]==','|| teststring[4]==ENDSTR || teststring[4]=='/'))
				{
				EA[SorD].Aregnum=EA[SorD].Aregnum | 0x2;	
				shiftarg(&teststring[0],4);
				}
			else if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[2]=='i' || teststring[2]=='I') &&
				(teststring[3]=='a' || teststring[3]=='A') &&
				(teststring[4]=='r' || teststring[4]=='R') &&
				(teststring[5]==','|| teststring[5]==ENDSTR || teststring[5]=='/'))
				{
				EA[SorD].Aregnum=EA[SorD].Aregnum | 0x1;	
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
		else if(table_syntax[0]=='P')
			{
			if((teststring[0]=='f' || teststring[0]=='F') &&
				(teststring[1]=='p' || teststring[1]=='P') &&
				(teststring[2]>='0' && teststring[2]<='7') &&
				(teststring[3]==':') &&
				(teststring[4]=='f' || teststring[4]=='F') &&
				(teststring[5]=='p' || teststring[5]=='P') &&
				(teststring[6]>='0' && teststring[6]<='7') &&
				(teststring[7]==','|| teststring[7]==ENDSTR || teststring[7]=='{'))
				{
				EA[SorD].Aregnum=teststring[6]-'0'<<7 | teststring[2]-'0';	
				shiftarg(&teststring[0],8);
				shiftarg(table_syntax,1);
				}
			else
				{
				error=TRUE;
				continue;
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
		if(table_syntax[0]==ENDSTR && !error)
			{
			if(teststring[0] != ENDSTR && teststring[0] != ' ' && 
				instructmatch == 17)
				instructnum=-1;/* fmove mismatch on {Dn}/{#k} */
			else
				instructnum=instructmatch;
			return;
			}
		}/*end while*/
	}			/* end of if Bmatch=1    */
}
}

#endif
