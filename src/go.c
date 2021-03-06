#include"userdef.h"

/* *************************************************** */
/* 
Go -> this command hands control over to a program in memory.

The objective of this command is to give the user a way to go from
the monitor to a program they have in memory. Return to the monitor is
through a breakpoint, interrupt or simmilar exception.

The "go " part of the command line is stripped off. Then a check is made
on the number of arguments. If argc equals 1, then insert breakpoints and use
the value in the pc set by rmcmd for a starting address.
If argc equals 2, then either the -b option and no register was selected, or
a register was selected. If argc equals 3, then -b and a register were
selected. If no register is selected, then a message prompting the user for
a register name is issued. The register selected, either from the command line
or after the prompt, is then used to get a newvalue into that register. 
*/
/* *************************************************** */

gocmd(argc,argv)
int argc;	/* number of arguments on the command line */
char *argv;	/* command line */
{
extern struct regelem mpu[];	/* table of mpu registers */
register int start;		/* starting address to go to */
extern int error;		/* global error flag from getnum */
extern int regdata;

	striparg(argv);
	switch(argc)
	{
		case 1:
			insertbr();
			break;
		case 2:
			if (argv[0] != OPTDEL)
			{
				start = getnum(argv,ERR02,DEFAULTSCALE);
				if (error)
					return(0);
				mpu[PC].value = start;
				insertbr();
			}
			else if (argv[1] != 'b')
			{
				print(ERR04);
				return(0);
			}
			break;
		case 3:
			if (argv[0] != OPTDEL || argv[1] != 'b')
			{
				print(ERR04);
				return(0);
			}
			striparg(argv);
			start = getnum(argv,ERR02,DEFAULTSCALE);
			if (error)
				return(0);
			mpu[PC].value = start;
			break;
		default:
			print(ERR01);
			return(0);
	}
	prepreg(mpu);
	putmpu();
}

/* *************************************************** */

