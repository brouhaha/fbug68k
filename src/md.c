#include"userdef.h"

/* ************************************************* */
/* 
md [<size>] <range>
Memory Display -> This command displays memory on the screen in <size>
large units.

A check is first made for to many arguments on the command line. If not then
a check is made for size option. Next the range is determined in two parts,
the starting address and the count of "size" large units to get.
Finally the data is displayed, with a check made to see if the screen is 
overflowing. If it is overflowing, then the HITKEYMSG message is printed
out and output is suspended until any key is hit.
*/
/* ************************************************* */

mdcmd(argc,argv)
int argc;
char *argv;
{
extern int error;
register int start,count, addr, data, size, num;
register int j;
char ch;

	if (argc > 3)
		print(ERR01);
	else
		{
		striparg(argv);
		if(argv[0] == '-' && 
			(argv[1] == 'd' || argv[1] == 'D') &&
			(argv[2] == 'i' || argv[2] == 'I'))
			{
			striparg(argv);
			start = getnum(argv,ERR02,DEFAULTSCALE);
			if(error)
				return(0);
	
			for (j=0;;j++)
			{
				size = disasm(start);
				print("\n");
				start = start + size;
				if(check())
					break;
				j = j + size/6;
				if (j >= MAXSCREEN)
				{
					j = 0;
					print("\nType 's' to Halt (any other key to continue...)\n");
					ch = getch(TERMINAL,FALSE);
					if(ch == 's' || ch == 'S')	
						return;
				}
			}
			}
		else
			{
			size = getsize(argv,ERR04);
			if (size < 0)
				return(0);
			start = getnum(argv,ERR02,DEFAULTSCALE);
			if (error)
				return(0);
			count = getcount(argv,size,start,NOERR);
			if (count < 0)
				count = DEFCOUNT;
			addr = 0;
			num = 0;
			while (count >0)
			{
				if (addr == 0)
					print("%c%8x:  ",HEXDEL,start);
				switch (size)
				{
					case 1:
						data = get8(start);
						print("%c%2X  ",HEXDEL,data);
						start = start + 1;
						count--;
						break;
					case 2:
						data = get16(start);
						print("%c%4X  ",HEXDEL,data);
						start = start + 2;
						count--;
						break;
					case 4:
						data = get32(start);
						print("%c%8X  ",HEXDEL,data);
						start = start + 4;
						count--;
						break;
					default:
						print(ERR01);
						return(0);	
				}
				if (++addr == MEMLINEDIS)
				{
					print("\n");
					addr = 0;
					if (check())
					{
						print("\n");
						return(0);
					}
					if (num++ == MAXSCREEN)
					{
						num = 0;
						print(HITKEYMSG);
						getch(TERMINAL,FALSE);
					}
				}
			}/* end while */
			print("\n");
		}/* end else */
	}/* end else */
}/* end function */
