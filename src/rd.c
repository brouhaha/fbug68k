#include"userdef.h"
/* ********************************************** */


rdcmd(argc,argv)
int argc;
char *argv;
{
extern struct regelem mpu[];
	
	switch(argc)
	{
		case 1:
			printreg(mpu);
			break;
		case 2:
			striparg(argv);
			if (argv[0] != '-')
				print(ERR04);
			else
				switch (argv[1])
				{
					case 'm':
					case 'M':
						printreg(mpu);
						break;
#if(DEVICE==68040 || COPROCESSOR==TRUE)
					case 'f':
					case 'F':
						printfpu();
						break;
#endif
					default:
						print(ERR04);
				}
			break;
		default:	
			print(ERR01);
	}
}
	
/* ********************************************** */
	
