#include"userdef.h"

/*
This routine inputs a string from the terminal, strips of leading
blanks, allows for destructive backspacing while on the line, and returns 
the number of arguments seperated by a space, along with the line.
*/

getline(buffer)
char *buffer;	/* string to put the typed line into */
{
register char ch;	/* charachter inputed */
register char *bufptr;	/* pointer to within gbuffer */
register int length;	/* length of string inputed */
register int intext;	/* flag for being between text delimiters or blanks */
register int i;		/* loop counter */

	length = 0;
	ch = ' ';
	bufptr = buffer;
	*bufptr = ch;
	intext = FALSE;
	while ((bufptr < buffer+MAXLINE)&&((ch = getch(TERMINAL,intext)) != CR && ch != LF))
	{
		if (ch == '\b')
		{
			if( bufptr > buffer)
				*bufptr--; 
		}
		else
		{
			if (bufptr == buffer && ch == ' ')
				;
			else
			{
				*bufptr++ = ch;
				if (ch == TEXTDEL)
					intext = TRUE;
			}
		}
	}
	putch(TERMINAL,LF);
	*bufptr = ENDSTR;
	if (buffer[0] == ENDSTR)
		return(0);
	while (whitesp(buffer[0]))
		shiftarg(buffer,1);
	for (i = 0,intext = FALSE;buffer[i] != ENDSTR;i++)
		if (whitesp(buffer[i]))
			intext = FALSE;
		else if (intext == FALSE)
		{
			intext = TRUE;
			++length;
		}
	return(length);
}


/*
This routine inputs a string from the terminal, strips of leading
blanks, allows for destructive backspacing while on the line, and returns 
the number of arguments seperated by a space, along with the line.
IT DOES NOT ECHO THE RETURN CHARACTER TO THE TERMINAL!!!
*/


getlineNR(buffer)
char *buffer;	/* string to put the typed line into */
{
register char ch;	/* charachter inputed */
register char *bufptr;	/* pointer to within gbuffer */
register int length;	/* length of string inputed */
register int intext;	/* flag for being between text delimiters or blanks */
register int i;		/* loop counter */

	length = 0;
	ch = ' ';
	bufptr = buffer;
	*bufptr = ch;
	intext = FALSE;
	while ((bufptr < buffer+MAXLINE)&&((ch = getch(TERMINAL,intext)) != CR && ch != 0xa))
	{
		if (ch == '\b')
		{
			if( bufptr > buffer)
				*bufptr--; 
		}
		else
		{
			if (bufptr == buffer && ch == ' ')
				;
			else
			{
				*bufptr++ = ch;
				if (ch == TEXTDEL)
					intext = TRUE;
			}
		}
	}
	*bufptr = ENDSTR;
	if (buffer[0] == ENDSTR)
		return(0);
	while (whitesp(buffer[0]))
		shiftarg(buffer,1);
	for (i = 0,intext = FALSE;buffer[i] != ENDSTR;i++)
		if (whitesp(buffer[i]))
			intext = FALSE;
		else if (intext == FALSE)
		{
			intext = TRUE;
			++length;
		}
	return(length);
}

/* **************************************************** */

getch(addr,intext)
int addr,intext;
{
register char ch;

	ch = getchar();
	if (!intext && CONVCAP && (ch >= 'A') && (ch <= 'Z'))
		ch = ch + 'a' - 'A';
	return(ch);
}
