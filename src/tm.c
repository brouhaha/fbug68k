#include"userdef.h"

/* ************************************************* */
			
tmcmd(argc,argv)
int argc;
char *argv;
{

register char termch,hostch,escch;

	if (argc > 2)
	{
		print(ERR01);
		return(1);
	}
	if (argc > 1)
	{
		striparg(argv);
		escch = argv[0] & 0x3f;
	}
	else
		escch = ESCDEF;
	print("Escape character: ^a\n");
	while (TRUE)
	{
		if ((get8(TERMINAL+SRX) & 1) == 1)
		{
			termch = get8(TERMINAL+RBX);
			if (termch == escch)
				break;
			else
				putch(HOST,termch);
		}
		if ((get8(HOST+SRX) & 1) == 1)
		{
			hostch = get8(HOST+RBX);
			putch(TERMINAL,hostch);
		}
	}
}	

/* ************************************************* */

