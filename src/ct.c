#include"userdef.h"

/* *************************************************** */

ctcmd(argc,argv)
int argc;	/* number of arguments on the command line */
char *argv;	/* command line */
{
extern confres;

confres=-1;
/*
conf();	
*/
switch(confres)
{
	case 0:
		print("Confidence Test Passes\n");
		break;
	case 1:
		print("Confidence Test Fails\n");
		break;
	default:
		print("ERROR 14:Unable to perform Confidence Test\n");
		break;
}
}

/* *************************************************** */

