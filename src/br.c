#include"userdef.h"

/* ****************************************************** */
/* 
br [<addr>][<COUNTDEL><count>]
or
br -r [<addr>]
Break point -> this command either lists all the known break points ("br"),
inserts a break point ("br <addr>[<COUNTDEL><count>]), changes the count
of an existing break point ("br <addr>[<COUNTDEL><count>]"), removes all break
points ("br -r") or remove a specific breakpoint ("br -r <addr>).

The objective of this command is to provide a way for users to stop their
programs when they reach a certain address (<addr>) a specified number of times
(<count>). If no count is specified, then the default of 0 is used. This means
the breakpoint will occur the first time the breakpoint is encountered.

The way the breakpoint works is to remove a section of code, insert a trap
instruction, then each time the trap is executed, 1 is subtracted from count
until it is negative. This means the count of zero will cause an immediate
break when hit. Negative counts are illegal.

The subroutine first checks to see if to many arguments are on the command
line. If so, print an error message and leave. If the number of arguments is 1
then we know all the user wants is a printout of the break points known.

If the number of arguments are 2 or 3, known by virtue of getting through
the above to tests, then we strip off the "br " from the front of the command.
Then a check is made for an option delimeter. 

If an option delimeter is present, then we need to make sure the option is to
remove ("-r"). Also, we need to know if the number of arguments are 2 or 3.
If 3, then the address of the specific break point is found by getnum. The
breakpoint table is searched  for a match of the address, the match has its
breakpoint changed to -1 (so that it will be ignored later) and all the other
breakpoints are printed. If the number of arguments was 2, then the entire
breakpoint table is searched, and breakpoint with a count greater than zero
has its count changed to zero. In either the 2 or 3 argument case, the
command has done its duty and will fall through to the end to return to
the main loop.

If an option delimeter is not present, then we are going to insert or change
a specific break point at address <addr>. First, we get the address. If there
is an error, then we leave. Next, we see if a count delimiter is present. If
so, then we know there will be a <count> and use getnum() to get the count.
If the <COUNTDEL> is absent, then the default of 0 is used for the count.
Now that the <count> is obtained, we go through the list doing three things.
One, we see if we can find a match of the address, indicating we should
change the count. Two, we see if we can find an open slot to stick the 
break point in the table if no match is found. Finally, we print out all
the breakpoints that exist. If no match is made and the table is full, then
a error message is displayed.
*/
/* ****************************************************** */

brcmd(argc,argv)
int argc;	/* number of arguments on a command line */
char *argv;	/* the command line */
{
extern struct breakelem brtable[];	/* the break table */
extern int error;	/* global flag for an error from getnum() */
register int num;	/* the number of an entry in the break table */
register int open;	/* the number of a free spot in the break table */
register int start;	/* a flag that the new address (or count) is done */
register int addr;	/* an address to be inserted in the break table */
register int count;	/* the count to be inserted in the break table */

	if (argc > 3)
		print(ERR01);
	else if (argc == 1)
	{
		print(BRKMSG);
		for (num=0;num < MAXBR;num++)
			if (brtable[num].count >= 0)
				print("%c%8x:%c%d\n",HEXDEL,brtable[num].address,DECDEL,brtable[num].count);
	}
	else
	{
		striparg(argv);
		if (argv[0] == OPTDEL)
		{
			if (argv[1] == 'r' && argc == 3)
			{
				striparg(argv);
				addr = getnum(argv,ERR02, DEFAULTSCALE);
				if (error)
					return(0);
				print(BRKMSG);
				for (num=0; num < MAXBR;num++)
				{
					if(brtable[num].address == addr)
						brtable[num].count = -1;
					if (brtable[num].count >= 0)
						print("%c%8x:%c%d\n",HEXDEL,brtable[num].address,DECDEL,brtable[num].count);
				}
			}
			else if (argv[1] == 'r' && argc == 2)
			{
				striparg(argv);
				print(BRKMSG);
				for (num=0; num < MAXBR;num++)
					if (brtable[num].count >= 0)
						brtable[num].count = -1;
			}
		}	
		else
		{
			addr = getnum(argv,ERR02,DEFAULTSCALE);
			if (error || addr == 0)
				return(0);
			if (argv[0] == COUNTDEL)
			{
				shiftarg(argv,1);
				count = getnum(argv,ERR02,COUNTSCALE);
				if (count < 0)
				{
					print(ERR02);
					return(0);
				}
			}
			else
				count = 0;
			start = FALSE;
			open = -1;
			print(BRKMSG);
			for (num = 0; num < MAXBR; num++)
			{
				if (brtable[num].address == addr)
				{
					start = TRUE;
					brtable[num].count = count;
					open = num;
				}
				if (open < 0 && brtable[num].count < 0)
					open = num;
				if (brtable[num].count >= 0)
					print("%c%8x:%c%d\n",HEXDEL,brtable[num].address,DECDEL,brtable[num].count);
			}
			if (!start && open >= 0)
			{
				brtable[open].address = addr;
				brtable[open].count = count;
				brtable[open].inst1 = get32(addr);
				print("%c%8x:%c%d\n",HEXDEL,brtable[open].address,DECDEL,brtable[open].count);
				start = TRUE;
			}
			if (!start)
				print(ERR06);
		}
	}
}

/* ****************************************************** */

