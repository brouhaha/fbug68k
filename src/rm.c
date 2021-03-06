#include"userdef.h"

/* ************************************************ */

rmcmd(argc,argv)
int argc;
char *argv;
{
extern struct regelem mpu[];

	striparg(argv);
	switch(argc)
	{
		case 1:
			print(WHICHREGMSG);
			if (1 != getline(argv))
			{
				print(ERR01);
				return(0);
			}
			findreg(mpu,argv,FALSE);
			break;
		case 2:
			findreg(mpu,argv,FALSE);
			break;
		case 3:	/* Store data into the register and return */
			findreg(mpu,argv,TRUE,argv);
			break;
		default:
			print(ERR01);
			return(0);
	}
}

/* ************************************************ */

