#include"userdef.h"

/* ****************************************************** */
/* 
He [<name>]||[<syntax>]
Help -> This command prints out the commands and their syntax.

The object of this command is to provide the user with help
in inputing commands by providing the syntax expected by each
command.

The "he " is stripped off. If only one argument is on the command
line, then a list of all the commands is printed. If more than one
argument on the command line, the command list and syntax list is
searched for a match. If none is found, then an error message is printed.
If a match is found, then a message describing the command or syntax is
printed.
While printing out the command list, if the number of commands exceeds the
number the screen can hold, then a message is printed and output is halted
until the user hits any key.
*/
/* ****************************************************** */

hecmd(argc,argv)
int argc;	/* number of arguments on command line */
char *argv;	/* command line */
{
extern struct table_element f[];	/* table of commands */
extern struct syntax_element syn[];	/* table of syntaxes */
register int i,j;			/* loop counters */

	striparg(argv);
	switch (argc)
	{
		case 1:
			for (j=0,i=0;f[i].nptr != LASTCMD;i++,j++)
			{
				print(f[i].nptr);
				if(check())
					break;
				if (j == MAXSCREEN)
				{
					j = 0;
					print(HITKEYMSG);
					getch(TERMINAL,FALSE);
				}
			}
			break;
		case 2:	
			for (i=0;f[i].nptr != LASTCMD;i++)
			{
				for (j=0;f[i].nptr[j] != ENDCMD && argv[j] == f[i].nptr[j];j++)
					;
				if (f[i].nptr[j] == ENDCMD && whitesp(argv[j]))
					break;
			}
			if (f[i].nptr != LASTCMD)
			{
				print(f[i].nptr);
				print(f[i].syntax);
			}
			else
			{
				print("[] => optional, <> => required, || => or, ... => repeated\n");
				for(i=0;syn[i].desc != LASTCMD;i++)
				{
					for(j=0;syn[i].desc[j] != ENDCMD && argv[j] == syn[i].desc[j];j++)
						;
					if (syn[i].desc[j] == ENDCMD && whitesp(argv[j]))
						break;
				}
				if (syn[i].desc != LASTCMD)
					print(syn[i].desc);
				else
					print(UNKNOWNMSG);
			}
			break;
		default:
			print(ERR01);
	}
}

/* ************************************************* */

