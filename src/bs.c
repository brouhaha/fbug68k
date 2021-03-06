#include"userdef.h"

/* ****************************************************** */
/* 
bs [<size>] <range> (<data>[ <mask>])||(<text>)
Block search -> Search an area of memory within the area bounded by <range>
for the pattern shown by <data> or <text>.

The objective of this command is to provide a way to find a pattern in
memory. The end of the area searched may extend beyond the <range> if
a pattern is started within the <range>.

An exact match is searched for text. The data may be qualified by a
pattern. This pattern has don't cares for every 0 on the <mask>.

The command first checks to see if the number of arguments is proper.
If the number of arguments on the command line is proper, then the
"bs " is striped from the front of the command line. The size for
<range> is found, if any. The the starting address from <range> and
the <count> of the number of 1,2 or 4 byte increments for the <count>.
The ending address is then determined. The starting and stopping
address are then printed. If the size was not 1 byte, then the count
is adjusted so the search will be in 1 byte increments. 

Now the command will find out if it is searching for <text> or <data>.
If <text>, then strip off the text delimeter and begin the search
until count equals 0. The search is in two stages. If the first
charachter of the search string matches the the byte currently being
pointed to by start address, then the second stage of the search is
entered. Otherwise, the starting address is saved
*/
/* ****************************************************** */

bscmd(argc,argv)
int argc;	/* number of arguments on the command line */
char *argv;	/* the command line */
{

register int start;	/* the address to start the search at */
register int addr;	/* the ending address being seached */
register int size;	/* the size of the increments for <count> in <range> */
register int mask;	/* data at a given address */
register int count;	/* number of bytes to be searched */
register int data;	/* */
register int i;		/* */
extern int error;	/* global flag from getnum() indicating an error */

	if (argc < 3 || argc > 5)
		print(ERR01);
	else
	{
		striparg(argv);
		size = getsize(argv,ERR04);
		if (size < 0)
			return(0);
		start = getnum(argv,ERR02,DEFAULTSCALE);
		if (error)
			return(0);
		count = getcount(argv,size,start);
		if (count < 0)
			count = DEFCOUNT;
 		addr = start + size * count - 1;
		print("Starting address: %c%8x\n",HEXDEL,start);
		print("Ending   address: %c%8x\n",HEXDEL,addr);
		if (argv[0] == TEXTDEL)
		{
			switch (size)
			{
				case 4:
					count = count * 4;
					break;
				case 2:	
					count = count * 2;
					break;
				case 1:
					break;
				default:
					return(0);
			}
			size = 1;
			shiftarg(argv,1);
			while (count > 0)
			{
				if (get8(start) != argv[0])
				{
					start++;
					count--;
				}
				else
				{
					addr = start;
					for(i=1;argv[i] != ENDSTR && get8(++addr) == argv[i];i++,count--)
						;
					if (argv[i] == ENDSTR)
						print("Found at: %c%8x to: %c%8x \n",HEXDEL,start,HEXDEL,addr);
					start++;
				}
				if(check())
					break;
			}
		}
		else
		{
			data = getnum(argv,ERR03,DEFAULTSCALE);
			if (error)
				return(0);
			mask = MASK32;
			if (argv[0] != ENDSTR) 
			{
				mask = getnum(argv,ERR03,DEFAULTSCALE);
				if (error)
					return(0);
			}
			data = data & mask;
			switch(size)
			{
				case 1:
					mask = MASK8 & mask;
					while (count > 0)
					{
						count--;
						if ((get8(start) & mask) == data)
							print("Found at: %c%8x:%c%2x\n",HEXDEL,start,HEXDEL,get8(start));
						start++;
						if (check())
							break;
					}
					break;
				case 2:
					mask = MASK16 & mask;
					while (count > 0)
					{
						count--;
						if ((get16(start) & mask) == data)
							print("Found at: %c%8x:%c%4x\n",HEXDEL,start,HEXDEL,get16(start));
						start++;
						if (check())
							break;
					}
					break;
				case 4: 
					mask = MASK32 & mask;
					while (count > 0)
					{
						count--;
						if ((get32(start) & mask) == data)
							print("Found at: %c%8x:%c%8x\n",HEXDEL,start,HEXDEL,get32(start));
						start++;
						if (check())
							break;
					}
					break;
				default:
					return(0);
			}
		}
	}
}		
				
/* ****************************************************** */

