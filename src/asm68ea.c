
#include "userdef.h"

/* ********************************************************************* */
/* additions to userdef.h						*/

#define MAXSTR 80
#define SOURCE 0
#define DEST 1
/* ********************************************************************* */

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

			if(f_fp >= weight || 
			     (i == 23 && 
			      fraction_bits == 23 && 
			      f_fp >= (weight * 0.5)))

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
