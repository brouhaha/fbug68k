#include"userdef.h"

/* **************************************************** */
/*
bf [<size>] <range> <data>
Block fill -> This command fills an area of memory
described by <range> with what ever is in the <data> field.

The objective of this command is to provide a simple way to
set an area of memory to a value and check that the memory
was correctly set. 

If the size of the data and the size of the area to be filled
are not equal, the command will leave the last partial word or 
half word, or longword unchanged. This is indicated by the bf
command when it reports where it started, where it stopped and 
what data it used. If the data is larger than the size chosen,
then the begining of the data will be truncated to fit.

The subroutine first checks to see if the number of arguments on
the command line is correct. Since <size> is optional, the allowable
number of arguments on the command line may be 3 or 4.
if the number of arguments on the command line are not correct,
then drop down to the end of the program and print an error.

If the number of arguments on the command line are correct, then
we parse the command line.
If at any time an error occurs in getting an argument, then we
return to the main loop without changing any memory space.
First we get the <size> from the command line (or have it set to
the default). 
Second we get the <range> from the command line. This involves getting a 
starting address and a count of the number of units of memory that
will be filled. The range is specified as <addr>,<addr> then this is converted
to <addr>:<count>. This may mean the ending address is not the same as
the typed ending address as the block fill will not partially fill a
memory area of demension <size>, but rather abreviate the area filled.
Third we get the <data> from the command line. If there is more data
than <size> would permit, we will truncate later.

If we get this far, then there will be no more chance of errors
from the command line. To tell the user what is going on, we
print the starting address that we will fill and the the ending address.
Then we fill in the data by one of three tight loops. If the default case
is reached, then the getsize() routine made an error.

The data fill loop starts by truncating any unneeded data from the front
of the data line. Next, a printout of the data used is made. Then, the
assembly language routines are called that will put 8, 16 or 32 bits of data
into the memory area. As the data is inserted, then the address is read and
compared with the data. If the data written and the data read do not agree,
then an error message is printed and the command returns to the main loop.
If the data matches, the count is decremented and the starting address
is incremented by either 1, 2 or 4 bytes. Finally, a check is made to see
if the user has typed a cancel instruction. If so, then return with no further
memory changed, though the memory already filled will remain changed.
*/
/* **************************************************** */

bfcmd(argc,argv)
int argc;	/* number of arguments in the command line */
char *argv;	/* the command line in ASCII */
{

extern int error;	/* global flag from getnum */
register int size;	/* size of data to fill into memory*/
register int start;	/* the starting address to begin fill at */
register int count;	/* the number of 8, 16, or 32 bit increments to fill into memory */
register int data;	/* the information to fill into the memory */

	if (argc == 3 || argc == 4)
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
		{
			print(ERR02);
			return(0);
		}
		data = getnum(argv,ERR03,DEFAULTSCALE);
		if (error)
			return(0);
		print("Starting address: %c%8x\n",HEXDEL,start);
		print("Ending   address: %c%8x\n",HEXDEL,start + count * size - 1);
		switch (size)
		{
			case 1:
				data = data & MASK8;
				print("Data     entered: %c%2x\n",HEXDEL,data);
				while (count > 0)
				{
					put8(start,data);
					if(data != get8(start))
					{
						print("Memory fault at %c%8x\nData written %c%2x\nData read %c%2x\n",HEXDEL,start,HEXDEL,data,HEXDEL,get8(start));
						return(0);
					}
					count--;
					start = start + 1;
					if(check())
						break;
				}
				break;
			case 2:
				data = data & MASK16;
				print("Data     entered: %c%4x\n",HEXDEL,data);
				while (count > 0)
				{
					put16(start,data);
					if(data != get16(start))
					{
						print("Memory fault at %c%8x\nData written %c%4x\nData read %c%4x\n",HEXDEL,start,HEXDEL,data,HEXDEL,get16(start));
						return(0);
					}
					count--;
					start = start + 2;
					if(check())
						break;
				}
				break;
			case 4:
				print("Data     entered: %c%8x\n",HEXDEL,data);
				while(count >0)
				{
					put32(start,data);
					if(data != get32(start))
					{
						print("Memory fault at %c%8x\nData written %c%8x\nData read %c%8x\n",HEXDEL,start,HEXDEL,data,HEXDEL,get32(start));
						return(0);
					}
					count--;
					start = start + 4;
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
