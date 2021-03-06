#include "userdef.h"

/* **************************************************************** */
/* **************************************************************** */

ascmd(argc,argv)
int argc;	/* number of arguments on the command line */
char *argv;	/* the command line */
{
extern int error;	/* global flag for error from getnum() */
register int num;	/* the amount to back up or advance */
register int start;	/* the address at which assembly/disassembly is done */
register int size;	/* the size of the code assembled/disassembled */

if (argc != 2)
	print(ERR01);
else
	{
	striparg(argv);
	start = getnum(argv,ERR02,DEFAULTSCALE);
	if (error)
		return(0);
while (TRUE)
	{
	error = FALSE;
	size = disasm(start);
	num = getlineNR(argv);
	switch(argv[0])
	{
		case BACKUP:
			shiftarg(argv,1);
			if (num == 1)
			{
				num = getnum(argv,ERR02,DEFAULTSCALE);
				if (error || num == 0)
					num = 1;
			}
		else
				num = 1;
			start = start - num;
			break;
		case FINISH:
			return(0);
		case NEXT:
			shiftarg(argv,1);
			if (num == 1)
			{
				num = getnum(argv,ERR02,DEFAULTSCALE);
				if (error || num == 0)
					num = 1;
			}
			else
				num = 1;
			start = start + num;
			break;
		case HELP:
			print("\nMC%d Assembler/Disassembler",DEVICE);
			print(MMASHELPMSG);
			break;
		default:
			if (num != 0)
				{
				print("                                                                               \r");
				size = assembly(start,argv);
				}
			else
				print("\n");
			if (size == 0)
			{
				print(ERR09);
				print(argv);
				print("\n");
			}
			else if(size < 0)
				size=0;
			start = start + size;
			break;
	}
	}
	}
}

/* **************************************************************** */

