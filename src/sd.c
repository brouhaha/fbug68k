#include "userdef.h"

/* *************************************************************** */

sdcmd(argc,argv)
int argc;
char *argv;
{
int i,j;
extern int error;
extern struct symbelem symb[];

	switch (argc)
	{
		case 1:
			print(SYMBMSG);
			for (i = 0; i < MAXSYMBOL;i++)
				if (symb[i].name[0] == SYMBOLDEL)
				{
					print(symb[i].name);
					print("\t%c%8x\n",HEXDEL,symb[i].value);
				}
			break;
		case 2:
			print(SYMBMSG);
			for (i = 0; i < MAXSYMBOL;i++)
			{
				for (j = 0; j < MAXSYMBLEN; j++)
				{
					if (whitesp(argv[j]) || symb[i].name[j] !=  argv[j])
						break;
				}
				if (whitesp(argv[j]) && argv[j] == symb[i].name[j])
				{
					print(symb[i].name);
					print("\t%c%8x\n",HEXDEL,symb[i].value);
					break;
				}
			}
			break;
		case 3:
			striparg(argv);
			if (argv[0] == OPTDEL && argv[1] == 'r')
			{
				striparg(argv);
				if (argv[0] == SYMBOLDEL && !whitesp(argv[1]))
				{
					for (i = 0; i < MAXSYMBOL; i++)
					{
						for (j = 0; j < MAXSYMBLEN; j++)
						{
							if (whitesp(argv[j]) || symb[i].name[j] !=  argv[j])
								break;
						}
						if (whitesp(argv[j]) && argv[j] == symb[i].name[j])
						{
							symb[i].name[0] = ' ';
							symb[i].value = 0;
							break;
						}
					}
					if (i == MAXSYMBOL)
						print(UNKNOWNMSG);
				}
			}
			else if (argv[0] == SYMBOLDEL)
			{
				for (i = 0; i < MAXSYMBOL; i++)
					if (symb[i].name[0] != SYMBOLDEL)
					{
						for (j=0;!whitesp(argv[j]);j++)
							symb[i].name[j] = argv[j];
						symb[i].name[j] = ENDSTR;
						striparg(argv);
						symb[i].value = getnum(argv,ERR01,DEFAULTSCALE);
						if (error)
						{
							symb[i].name[0] = 'X';
							symb[i].value = 0;
						}
						break;
					}
				if (i == MAXSYMBOL)
					print("Symbol table full.\n");
			}
			print(SYMBMSG);
			for (i = 0; i < MAXSYMBOL;i++)
				if (symb[i].name[0] == SYMBOLDEL)
				{
					print(symb[i].name);
					print("\t%c%8x\n",HEXDEL,symb[i].value);
				}
			break;
		default:
			print(ERR01);
	}
}
	
/* ************************************************************ */
	
