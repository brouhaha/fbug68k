#include"userdef.h"

handler()
{
extern int stackptr;
extern int regdata[];
extern int tracecount;
extern int oldbraddr;
extern int handlerflag;
extern int abortstart;
extern struct breakelem brtable[];
extern struct regelem mpu[];
register int vector;
register int stacktype;
register int instaddr;
register int i;
register int ispdelta;

	regdata[SR] = get16(stackptr);
	regdata[PC] = get32(stackptr + 0x2);
	vector = get16(stackptr + 0x6);
	stacktype = (vector >> 12) & MASK4;
	vector = (vector & MASK12) / 4;
	handlerflag = 0;
	switch (stacktype)
	{
		case 0:
		case 1:
			instaddr = regdata[PC];
			ispdelta = 0x8;
			break;
		case 2:
			instaddr = get32(stackptr + 0x8);
			ispdelta = 0xc;
			break;
		case 9:
			instaddr = get32(stackptr + 0x8);
			ispdelta = 0x14;
			break;
		case 10:
		case 11:
			instaddr = regdata[PC];
			ispdelta = 0;
			break;
	}
	storereg(mpu);
	switch (vector)
	{
		case 2:
		case 3:
			if (vector == 3)
				print("Address Error\n");
			else
				{
				print("Bus Error\n");
				print("SSW=$%x   ",get16(stackptr + 0xa));
				if(bitnum(8,8,get16(stackptr + 0xa))==1)
					print("Data Cycle Fault Address=$%x\n",get32(stackptr + 0x10));
				}
			print("\n");
			printreg(mpu);
			/* PRINT DISASM68.C OF PC VALUE HERE */
			disasm(instaddr);
			print("\n");
			handlerflag = -1;
			break;
		case 4:
			for (i = 0; i < MAXBR; i++)
			{
				if ((brtable[i].address == instaddr) && (brtable[i].count != -1))
				{
					if (brtable[i].tempcount == 0)
					{
						mpu[ISP].value = mpu[ISP].value + ispdelta;
						mpu[A7].value = mpu[A7].value + ispdelta;
						print("Breakpoint at %c%8x\n",HEXDEL,instaddr);
						print("\n");
						printreg(mpu);
					/* ********************************* */
					/* PRINT DISASM68.C OF PC VALUE HERE */
						put16(instaddr,brtable[i].inst1);
						disasm(instaddr);
						put16(instaddr, BRTRAP);
						print("\n");
					/* ********************************* */
						brtable[i].tempcount = -1;
						for (i = 0; i < MAXBR;i++)
							if (brtable[i].count >= 0)
								put16(brtable[i].address,brtable[i].inst1);
						i = 0;
						handlerflag = -1;
						break;
					}
					else if (brtable[i].tempcount == -1)
					{
						oldbraddr = brtable[i].address;
						put16(oldbraddr,brtable[i].inst1);
						put16(stackptr,regdata[SR] | TRACEON);
						tracecount = -1;
						i = 0;
						handlerflag = 0;
						break;
					}
					else
					{
						brtable[i].tempcount--;
						i = 0;
						handlerflag = 0;
						break;
					}
					break;
				}
			}
			if (i > 0)
			{
				mpu[ISP].value = mpu[ISP].value + ispdelta;
				mpu[A7].value = mpu[A7].value + ispdelta;
				print("Illegal instruction at %c%8x\n",HEXDEL,instaddr);
				print("\n");
				printreg(mpu);
				handlerflag = -1;
			}
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 55:
		case 59:
		case 60:
		case 61:
		case 62:
		case 63:
		case 64:
			switch(vector)
			{
				case 5:
					print("Zero Divide\n");
					break;
				case 6:
					print("CHK,CHK2 Instruction\n");
					break;
				case 7:
					print("cpTRAPcc,TRAPcc,TRAPV Instruction\n");
					break;
				case 8:
					print("Priivilege Violation\n");
					break;
				case 10:
					print("A-Line \n");
					break;
				case 11:
					print("F-Line \n");
					break;
				case 12:
				case 16:
				case 17:
				case 18:
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 55:
				case 59:
				case 60:
				case 61:
				case 62:
				case 63:
					print("(Unassigned, Reserved\n");
					break;
				case 64:
					print("Abort \n");
					break;
				case 13:
					print("Coprocessor Protocol Violation\n");
					break;
				case 14:
					print("Format Error\n");
					break;
				case 15:
					print("Uninitialized Interrupt\n");
					break;
			}
			print("\n");
			printreg(mpu);
			/* PRINT DISASM68.C OF PC VALUE HERE */
			disasm(instaddr);
			print("\n");
			handlerflag = -1;
			break;
		case 9:
			if (tracecount == -1)
			{
				for (i = 0; i < MAXBR; i++)
				{
					if (brtable[i].address == oldbraddr)
					{
						put16(oldbraddr,BRTRAP);
						put16(stackptr,regdata[SR] & ~TRACEON);
						brtable[i].tempcount == brtable[i].count;
						break;
					}
				}
			}
			else if (tracecount > 0)
				tracecount--;
			else
			{
				mpu[ISP].value = mpu[ISP].value + ispdelta;
				mpu[SR].value = mpu[SR].value & ~TRACEON;
				mpu[A7].value = mpu[A7].value + ispdelta;
				print("\n");
				for (i = 0; i < MAXBR; i++)
				{
				if ((brtable[i].address == regdata[PC]) && (brtable[i].count != -1))
					{
					print("Trace at Breakpoint (%x)\n",brtable[i].address);
					break;
					}
				}
				if(i == MAXBR)
					print("Trace at %c%8x\n",HEXDEL,regdata[PC]);
				printreg(mpu);
				/* PRINT DISASM68.C OF PC VALUE HERE */
				disasm(regdata[PC]);
				print("\n");
				handlerflag = -1;
			}
			break;
		case 24:
			print("Spurious Interrupt\n");
			print("\n");
			printreg(mpu);
			/* PRINT DISASM68.C OF PC VALUE HERE */
			disasm(instaddr);
			print("\n");
			handlerflag = -1;
			break;
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
			print("Autovector interrupt level %d\n",vector - 24);
			print("\n");
			printreg(mpu);
			/* PRINT DISASM68.C OF PC VALUE HERE */
			disasm(instaddr);
			print("\n");
			handlerflag = -1;
			break;
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			print("TRAP #%d\n",vector - 32);
			print("\n");
			printreg(mpu);
			/* PRINT DISASM68.C OF PC VALUE HERE */
			disasm(instaddr);
			print("\n");
			handlerflag = -1;
			break;
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
			print("FPCP Exception vector #%d\n",vector);
			print("\n");
			printreg(mpu);
			/* PRINT DISASM68.C OF PC VALUE HERE */
			disasm(instaddr);
			print("\n");
			handlerflag = -1;
			break;
		case 56:
		case 57:
		case 58:
			print("PMMU Exception vector #%d\n",vector);
			print("\n");
			printreg(mpu);
			/* PRINT DISASM68.C OF PC VALUE HERE */
			disasm(instaddr);
			print("\n");
			handlerflag = -1;
			break;
		default:
			print("User Defined Vector #%d\n",vector);
			print("\n");
			printreg(mpu);
			/* PRINT DISASM68.C OF PC VALUE HERE */
			disasm(instaddr);
			print("\n");
			handlerflag = -1;
			break;
	}
}
