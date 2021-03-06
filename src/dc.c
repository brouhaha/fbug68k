#include"userdef.h"

/* ***************************************************** */
/* 
dc <exp>
Data conversion -> This command converts an input expression into
the hexadecimal and decimal versions.

The object of this command is to allow users to easily get a value
without resorting to a calculator.

If an error occurs in evaluating the expression, an error message
is printed.
*/
/* ***************************************************** */

dccmd(argc,argv)
int argc;	/* number of arguments on command line */
char *argv;	/* the command line */
{

register int data;	/* results of the expression */
extern int error;	/* global error flag from getnum */

	if (argc != 2)
	{
		print(ERR01);                            
		return(0);
	}
	else
	{
		striparg(argv);
		data = getnum(argv,ERR02,DEFAULTSCALE);
		if (error)
			return(0);
		if (data < 0) /* if sum is negative, print signed and unsigned versions */
		{
			print("Unsigned :  %c%x =  %c%u \n",HEXDEL,data,DECDEL,data);
			print("Signed   : -%c%x = -%c%d \n",HEXDEL,-data,DECDEL,-data);
		}
		else
			print("%c%x = %c%d \n",HEXDEL,data,DECDEL,data);
	}
}

/* *********************************************** */

