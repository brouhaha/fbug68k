#include"userdef.h"

/* ***************************************************** */

mmcmd(argc,argv)
int argc;
char *argv;
{

register int num, size, data, start;
extern int error;
	
	if (argc == 2 || argc == 3)
	{
		striparg(argv);
		size = getsize(argv,ERR04);
		if (size < 0)
			return(0);
		start = getnum(argv,ERR02,DEFAULTSCALE);
		if (error)
			return(0);
		print(MMASHELPMSG);
		while (TRUE)	
		{
			switch (size)
			{
				case 1:
					data = get8(start);
					print("%c%8x %c%2X ? ",HEXDEL,start,HEXDEL,data);
					break;
				case 2:	
					data = get16(start);
					print("%c%8x %c%4X ? ",HEXDEL,start,HEXDEL,data);
					break;
				default:	
					data = get32(start);
					print("%c%8x %c%8X ? ",HEXDEL,start,HEXDEL,data);
					break;
			}
			data = getline(argv);
			if (data == 0)
				start = start + size;
			else if (data > 1)
			{
				print(ERR01);
			}
			else
			{
				switch (argv[0])
				{
					case BACKUP:
						shiftarg(argv,1);
						if (data == 1)
						{
							num = getnum(argv,ERR03,DEFAULTSCALE);
							if (error)
								num = 0;
						}
						else
							num = size;
						start = start - num;
						continue;
					case FINISH:
					case 'q':
						return(0);
					case NEXT:
						shiftarg(argv,1);
						if (data == 1)
						{
							num = getnum(argv,ERR03,DEFAULTSCALE);
							if (error)
								num = 0;
						}
						else
							num = size;
						start = start + num;
						continue;
					case HELP:
						print(MMASHELPMSG);
						continue;
					default:
						data = getnum(argv,ERR03,DEFAULTSCALE);
						if (error)
							break;
						else
						{
							switch (size)
							{
								case 1:
									put8(start,data & MASK8);
									break;
								case 2:	
									put16(start,data & MASK16);
									break;
								case 4:	
									put32(start,data);
									break;
							}
							start = start + size;
						}
						break;
					}
			}
		}
	}
	else
		print(ERR01);	
}

/* ***************************************************** */

