#include"userdef.h"

/* ************************************************************** */
/*
This is the main routine. The warm restart will come here. 
The routine consists of an infinite loop, exited by a go, go direct
or trace command.
A prompt is printed, a new command line is obtained. The start of the
command line is checked to see if it matches with a known command. If
so then the program branches to the new command. If the command is
unknown then an error message is printed.  
*/
/* ************************************************************** */

mainloop()
{
register int i,j;	/* loop counters */
extern char argv[];	/* the command line */
extern char lastcmd[];	/* the last command line */
extern int argc;	/* number of arguments on the command line */
extern int lastargc;	/* number of arguments on the last command line */
extern struct table_element f[];	/* table of commands */

	while (TRUE)
	{
		print(PROMPT);
		argc = getline(argv);
		if (argc == 0) 
		{
			if((lastcmd[0]=='t' || lastcmd[0]=='T') &&
					(lastcmd[1]=='r' || lastcmd[1]=='R'))
			{
			/* ********************************************	*/
			/* reset to the last command line if it was tr	*/
				argv[0]='t';
				argv[1]='r';
				argv[2]=ENDSTR;
				argc=1;
			/* ********************************************	*/
			}
		}
		if (argc != 0) 
		{
			/* **************************************** */
			/* set up the last command line		    */
			for(i=0;(argv[i]!=ENDSTR && i<=MAXLINE);i++)
				lastcmd[i]=argv[i];
			/* **************************************** */
			for (i=0;f[i].nptr != LASTCMD;i++)
			{
				for(j=0;f[i].nptr[j] != ENDCMD && argv[j] == f[i].nptr[j];j++)
					;
				if (f[i].nptr[j] == ENDCMD && whitesp(argv[j]))
					break;
			}
			if (f[i].nptr != LASTCMD)
				f[i].fptr(argc,argv);
			else
				print(ERR07);
		}	
		else
			print("\n");
	}
} 

/* *************************************************************** */

