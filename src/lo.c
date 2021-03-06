#include"userdef.h"

/* ************************************************* */
/* 
lo [<offset>] [<text>]
Load S-Record -> This command loads s records, adding <offset> to the address
in the s-record. The text is sent to the HOST computer port to start
the s-records flowing.
The "lo " part of the command line is stripped away. Then a test is made
to see if the <offset> is present or not. Then any <text> is obtained and 
sent to the HOST port.
The a skip to prime the pump is made. Then the type of s record is found.
If it is data, then the data is obtined, keeping track of the checksum, the
data is put into memory and the checksum is compared at the end of the line.
If it is the end of the s-records a message is printed and the command quits.
Otherwise, the s-record is skipped.
If the checksum doesn't match, then an error message is printed and the
control is returned to the main loop.
*/
/* ************************************************* */

locmd(argc,argv)
int argc;	/* number of arguments on the command line */
char *argv;	/* the command line */
{

extern int error;	/* global error flag from getnum */
int checksum;	/* checksum */
int offset_to_addr;	/* the offset to add to the address */
int type;	/* the type of s-record */
int length;	/* the length of the number of characters on the 
				s-record line */
int address;	/* the address to insert data into */
int data;	/* the data to insert */
int i;		/* a temporary loop counter */

	striparg(argv);
	if (argc > 1 && argv[0] != ';')
	{
		offset_to_addr = getnum(argv,ERR03,DEFAULTSCALE);
		if (error)
			return(0);
	}
	else
		offset_to_addr = 0;
print("offset==%x\n",offset_to_addr);
	if ( argc > 1 && argv[0] == ';')
	{
		for(i=1;(argv[i] != ENDSTR && i<=MAXLINE);i++)
			putch(HOST,argv[i]);
		putch(HOST,CR);
		putch(HOST,LF);
	}
	skiptos();
	while(TRUE)
	{
		type = getch(HOST,TRUE);
		putch(TERMINAL,type);
		type = type - '0';
		address = 0;
		length = getspair(0,FALSE,TRUE);
		checksum = length;/* length, address and code/data fields will
					be added	*/
		switch (type)
		{
			case 0:
				for(i=0;i<length+1;i++)
					data=getspair(0,FALSE,TRUE);
				skiptos();
				break;
			case 1:
			case 2:
			case 3:
				for(i=0;i<=type;i++)
				{
					data = getspair(0,FALSE,TRUE);
					checksum = (data + checksum) & MASK8;
					address = (address << 8) | data;
					length--;
				}
				print("\n");
				address = address + offset_to_addr;
				for(i=1;i<length;i++)
					checksum = ((getspair(address++,TRUE,FALSE) + checksum) & MASK8);
				data = (getspair(0,FALSE,FALSE) & MASK8);
				if (((data - ~checksum) & MASK8) != 0)
				{
					print(ERR08);
					print("%c%x, %c%x expected.\n",HEXDEL,data,HEXDEL,(~checksum & MASK8));
					print("length=%d\n",length);
					print("address=%x\n",address);
					return(0);
				}
				skiptos();
				break;
			case 7:
			case 8:
			case 9:
				type = 10 - type;
				while (type-- >= 0)
				{
					data = getspair(0,FALSE,TRUE);
					checksum = (data + checksum) & MASK8;
					address = (address << 8) | data;
					length--;
				}
				address = address + offset_to_addr;
				while (length-- > 1)
					checksum = ((getspair(0,FALSE,FALSE) + checksum) & MASK8);
				if (((getspair(0,FALSE,FALSE) - ~checksum) & MASK8) != 0)
				{
					print(ERR08);
					print("%c%x\n",HEXDEL,(~checksum & MASK8));
					return(0);
				}
				print("\n S-record load successful\n");
				return(0);
				break;
			default:
				print("%d S-record encountered\n",type);
				skiptos();
				break;
		}
	}
}

/* ***************************************************** */
skiptos()
{
char S;

	while (TRUE)
		{
		S = getch(HOST);
		if (S == 's' || S == 'S')
			{
			print("S");
			break;
			}
		}
}
/* ***************************************************** */
/* 
getspair gets 2 characters in the s-record format. If insert is
true it will insert them into memory at addr.
*/
/* ***************************************************** */

getspair(addr,insert,print)
int addr;	/* address to insert information */
int insert;	/* flag to insert information or not */
int print;	/* flag to print information or not */
{
register char ch1;	/* first charachter */
register char ch2;	/* second charchter */
int returnvalue;
	
	ch1 = getch(HOST,TRUE);
	if(print)
		putch(TERMINAL,ch1);
	ch2 = getch(HOST,TRUE);
	if(print)
		putch(TERMINAL,ch2);
	if (ch1 >= '0' && ch1 <= '9')
		ch1 = ch1 - '0';
	else if (ch1 >= 'A' && ch1 <= 'F')
		ch1 = ch1 - 'A' + 10;
	else if (ch1 >= 'a' && ch1 <= 'f')
		ch1 = ch1 - 'a' + 10;
	if (ch2 >= '0' && ch2 <= '9')
		ch2 = ch2 - '0';
	else if (ch2 >= 'A' && ch2 <= 'F')
		ch2 = ch2 - 'A' + 10;
	else if (ch2 >= 'a' && ch2 <= 'f')
		ch2 = ch2 - 'a' + 10;
	returnvalue = ((ch1 << 4) | ch2) & MASK8;
	if (insert == TRUE)
		put8(addr,returnvalue);
	return(returnvalue);
}
