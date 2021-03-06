#include"userdef.h"

/* ****************************************************** */
/*
bm [<size>] <range> <addr>
Block Move -> This command moves a block of memory bounded by <range>
to memory starting at <addr> in increments of <size>.

The objective of this command is to allow you to shift data up or down
in memory. For example, if you needed to insert a line of code into memory,
then you could clear out the appropriate amount of space with a block
move, then use the line assembler to insert the new line. The move is 
not destructive to the area moved from, unless the <addr> moved to falls within
the <range> moved from.

The <size> is only used if the <range> is described as <addr>:count, not
<addr>,<addr>. If <addr>,<addr> is used, then the <size> defaults to
byte. This means a faster block move can be made by using <addr>,<count>
and specifying the <size> as 32 bits (L for 68k, W for 88K).

The first thing done is to check to see if the right number of arguments
are on the command line. To many or to few results in an error.

If the number of arguments on the command line are correct, then the "bm "
is stripped from the start of the command line. Next the size option, if any,
is obtained. A check is made to see if <range> is in the form of
<addr>,<count>. If not then the size defaults to 1 byte so that getcount
won't truncate any of the <range>. The starting address, then the count
is obtained. Finally, an address is obtained from the command line.
If any of the command line arguments are incorrect, then an error
message is displayed and no memory is moved. If all are correct, then
the starting and ending address of the area from which the memory
will be used to the area the memory will fill.

Now that the command line has been successfully parsed, the data will
actually be moved. To ensure proper transfer, if the starting address
is lower than the address the memory data is being moved to, then the
data will be moved from high memory to low memory. Otherwise, the
memory will be moved from low memory to high memory.

The loop that does the actual memory move gets memory from the source location,
puts it in the destination location. Moves the from address (start)
towards the destination address. The count of the number if 1, 2 or 4 byte
increments of data is decremented and a check is made to see if the user
has type a halt or cancel. If a cancel is typed, then the move is stopped
where it is at but the memory already moved remains moved.

the default case should never be reached if the getsize routine works properly.
*/
/* ****************************************************** */

bmcmd(argc,argv)
int argc;
char *argv;
{

extern int error;	/* global variable used by getnum to signal an error */
register int size;	/* 1, 2 or 4 byte increments inwhich the memory will be moved*/
register int count;	/* number of 1,2 or 4 byte increments that will be moved */
register int start;	/* the starting address of the block move */
register int addr;	/* the address to which the meory is to be moved */
register int data;	/* a unit of data to be moved */

	if (argc == 3 || argc == 4)
	{
		striparg(argv);
		size = getsize(argv,ERR04);
		if (size < 0)
			return(0);
		for (count=0;argv[count] != COUNTDEL && !whitesp(argv[count]);++count);
			;
		if (whitesp(argv[count]))
			size = BYTE;
		start = getnum(argv,ERR02,DEFAULTSCALE);
		if (error)
			return(0);
		count = getcount(argv,size,start);
		if (count < 0)
		{
			print(ERR02);
			return(0);
		}
		addr = getnum(argv,ERR02,DEFAULTSCALE);
		if (error)
			return(0);
		print("From : %c%8x - %c%8x\n",HEXDEL,start,HEXDEL,start + count * size - 1);
		print("To   : %c%8x - %c%8x\n",HEXDEL,addr,HEXDEL,addr + count * size - 1);
		if (start < addr)
		{
			start = start + count * size - size;
			addr = addr + count * size - size;
			size = -size;
		}
		switch (size)
		{
			case 1:
			case -1:
				while (count > 0)
				{
					data = get8(start);
					put8(addr,data);
					start = start + size;
					addr = addr + size;
					--count;
					if(check())
						break;
				}
				break;
			case 2:
			case -2:
				while (count > 0)
				{
					data = get16(start);
					put16(addr,data);
					start = start + size;
					addr = addr + size;
					--count;
					if(check())
						break;
				}
				break;
			case 4:
			case -4:
				while (count > 0)
				{
					data = get32(start);
					put32(addr,data);
					start = start + size;
					addr = addr + size;
					--count;
					if(check())
						break;
				}
				break;
			default :
				return(0);
		}
	}
	else
		print(ERR01);
}
				
/* ****************************************************** */

