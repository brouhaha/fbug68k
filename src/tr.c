#include"userdef.h"

/* ****************************************************** */

trcmd(argc,argv)
int argc;
char *argv;
{
extern struct regelem mpu[];
register int start;
extern int error;
extern int trinst1;
extern int trinst2;
extern int trinst3;
extern int traddr1;
extern int traddr2;
extern int traddr3;
extern int tracecount;

	if (argc > 3)
		print(ERR01);
	else
	{
		striparg(argv);
		if (argc >= 2)
		{
			if (argv[0] != COUNTDEL)
			{
				start = getnum(argv,ERR02,DEFAULTSCALE);
				if (error)
					return(0);
				mpu[PC].value = start;
			}
			if (argv[0] == COUNTDEL)
			{
				shiftarg(argv,1);
			 	tracecount = getnum(argv,ERR02,COUNTSCALE);
				if (error)
					return(0);
				if (tracecount < 0)
				{
					print(ERR02);
					return(0);
				}
			}
			else
				tracecount = 0;
		}
		mpu[SR].value = mpu[SR].value | TRACEON;
		prepreg(mpu);
		putmpu();
	}
}
