#include "targetsys.h"
file "asmcode68.s"
text
global asmput8,asmput16,asmput32,asmget8,asmget16,asmget32
global putmpu,regdata,asmaddr,asmdata

RPC:		long	regdata
RSR:		long	regdata+4
RUSP:		long	regdata+8
RMSP:		long	regdata+12
RISP:		long	regdata+16
RVBR:		long	regdata+20
RCACR:		long	regdata+24
RCAAR:		long	regdata+28
RSFC:		long	regdata+32
RDFC:		long	regdata+36
RD0:		long	regdata+40
#if(DEVICE==68030)
SCRP:		long	regdata+104
SSRP:		long	regdata+112
STC:		long	regdata+120
STT0:		long	regdata+124
STT1:		long	regdata+128
SMMUSR:		long	regdata+134
#endif
#if(DEVICE==68040)
SURP:		long	regdata+104
SSRP:		long	regdata+112
STC:		long	regdata+120
SDTT0:		long	regdata+124
SDTT1:		long	regdata+128
SITT0:		long	regdata+132
SITT1:		long	regdata+136
SMMUSR:		long	regdata+142
#endif
#if(DEVICE==68040 || COPROCESSOR==TRUE)
FPCR:		long	regdata+144
FPSR:		long	regdata+148
FPIAR:		long	regdata+152
FP0:		long	regdata+156
#endif

asmput8: 	mov.b	asmdata+3,([asmaddr])
		rts

asmput16: 	mov.w	asmdata+2,([asmaddr])
		rts

asmput32: 	mov.l	asmdata,([asmaddr])
		rts

asmget8: 	clr.l	asmdata
		mov.b	([asmaddr]),asmdata+3
		bpl.b 	nosi8
		or.l   	&0xffffff00,asmdata
nosi8:		rts

asmget16: 	clr.l	asmdata
		mov.w	([asmaddr]),asmdata+2
		bpl.b 	nosi16
		or.l   	&0xffff0000,asmdata
nosi16:		rts

asmget32: 	mov.l	([asmaddr]),asmdata
		rts

putmpu:		
		mov.l	([RUSP]),%d0
#if(DEVICE>=68010)
		mov.l	([RVBR]),%d1
#endif
#if(DEVICE>=68020)
		mov.l	([RISP]),%d2
		mov.l	([RMSP]),%d3
		mov.l	([RCAAR]),%d4
		mov.l	([RCACR]),%d5
		mov.l	([RSFC]),%d6
		mov.l	([RDFC]),%d7
#endif
		mov.l	%d0,%usp
#if(DEVICE>=68010)
		mov.l	%d1,%vbr
#endif
#if(DEVICE>=68020)
		mov.l	%d2,%isp
		mov.l	%d3,%msp
		mov.l	%d4,%caar
		mov.l	%d5,%cacr
		mov.l	%d6,%sfc
		mov.l	%d7,%dfc
#endif
#if(DEVICE==68030)
		lea.l 	([SCRP]),%a0
		short	0xf010
		short	0x4c00		#pmove (a0),crp
		lea.l 	([SSRP]),%a0
		short	0xf010
		short	0x4800		#pmove (a0),srp
		lea.l 	([STC]),%a0
		short	0xf010
		short	0x4000		#pmove (a0),tc
		lea.l 	([STT0]),%a0
		short	0xf010
		short	0x0800		#pmove (a0),tt0
		lea.l 	([STT1]),%a0
		short	0xf010
		short	0x0c00		#pmove (a0),tt1
		lea.l 	([SMMUSR]),%a0
		short	0xf010
		short	0x6000		#pmove (a0),psr
#endif
#if(DEVICE==68040)
					#68040 instructions to effect supervisor
					#programer model support should be
					#inserted here and below !
#endif
#if(DEVICE==68040 || COPROCESSOR==TRUE)
		lea.l 	([FPCR]),%a0
		short	0xf210
		short	0x9000 		#fmove.x (a0),fpcr
		lea.l 	([FPSR]),%a0
		short	0xf210
		short	0x8800 		#fmove.x (a0),fpsr
		lea.l 	([FPIAR]),%a0
		short	0xf210
		short	0x8400 		#fmove.x (a0),fpiar
		lea.l 	([FP0]),%a0
		short	0xf210
		short	0xd0ff 		#fmovem.x (a0),fp0-fp7
#endif
		mov.w	&0x0024,-(%a7)
		mov.l	([RPC]),-(%a7)
		mov.l	([RSR]),%d0
		mov.w	%d0,-(%a7)
		movm.l	([RD0]),&0x7fff
		rte

