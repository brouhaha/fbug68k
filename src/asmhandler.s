#include "targetsys.h"
file "asmhandler.s"
text
global stackptr,handler,asmhandler,regdata,handlerflag

SUSP:		long	regdata+8
SMSP:		long	regdata+12
SISP:		long	regdata+16
SVBR:		long	regdata+20
SCACR:		long	regdata+24
SCAAR:		long	regdata+28
SSFC:		long	regdata+32
SDFC:		long	regdata+36
SD0:		long	regdata+40

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

asmhandler:	
		mov.l	%a7,stackptr
		movm.l	&0xffff,([SD0])
		mov.l	%usp,%d0
#if(DEVICE>=68010)
		mov.l	%vbr,%d1
		mov.l	%sfc,%d2
		mov.l	%dfc,%d3
#endif
#if(DEVICE>=68020)
		mov.l	%isp,%d4
		mov.l	%msp,%d5
		mov.l	%caar,%d6
		mov.l	%cacr,%d7
#endif
		mov.l	%d0,([SUSP])
#if(DEVICE>=68010)
		mov.l	%d1,([SVBR])
		mov.l	%d2,([SSFC])
		mov.l	%d3,([SDFC])
#endif
#if(DEVICE>=68020)
		mov.l	%d4,([SISP])
		mov.l	%d5,([SMSP])
		mov.l	%d6,([SCAAR])
		mov.l	%d7,([SCACR])
#endif
#if(DEVICE==68030)
		lea	(regdata),%a0
		short	0xf028
		short	0x4e00
		short	0x0068		#pmove crp,(104,a0)
		short	0xf028
		short	0x4a00
		short 	0x0070		#pmove msrp,(112,a0)
		short	0xf028
		short	0x4200
		short	0x0078		#pmove mtc,(120,a0)
		short	0xf028
		short	0x0a00
		short	0x007c		#pmove mtt0,(124,a0)
		short	0xf028
		short	0x0e00
		short	0x0080		#pmove mtt1,(128,a0)
		short	0xf028
		short	0x6200
		short	0x0086		#pmove mmusr,(134,a0)
#endif
#if(DEVICE==68040)
		lea	(regdata),%a0
					#68040 instructions to effect supervisor
					#programer model support should be
					#inserted here and below !
#endif
#if(DEVICE==68040 || COPROCESSOR==TRUE)
		lea	(regdata),%a0
		short	0xf228
		short	0xb000
		short	0x0090 		#fmove.l fpcr,(&144,a0)
		short	0xf228
		short	0xa800
		short	0x0094 		#fmove.l fpsr,(&148,a0)
		short	0xf228
		short	0xa400
		short	0x0098 		#fmove.l fpiar,(&152,a0)
		short	0xf228
		short	0xf0ff
		short	0x009c 		#fmovem.l fp0-fp7,(&156,a0)
#endif
		jsr	handler
		cmp.b	handlerflag,&0
		blt	backtomonitor
		mov.l	([SUSP]),%d0
#if(DEVICE>=68010)
		mov.l	([SVBR]),%d1
		mov.l	([SSFC]),%d2
		mov.l	([SDFC]),%d3
#endif
#if(DEVICE>=68020)
		mov.l	([SISP]),%d4
		mov.l	([SMSP]),%d5
		mov.l	([SCAAR]),%d6
		mov.l	([SCACR]),%d7
#endif
		mov.l	%d0,%usp
#if(DEVICE>=68010)
		mov.l	%d1,%vbr
		mov.l	%d2,%sfc
		mov.l	%d3,%dfc
#endif
#if(DEVICE>=68020)
		mov.l	%d4,%isp
		mov.l	%d5,%msp
		mov.l	%d6,%caar
		mov.l	%d7,%cacr
#endif
		movm.l	([SD0]),&0xffff
#if(DEVICE==68030)
		lea.l 	(SCRP),%a0
		short	0xf010
		short	0x4c00		#pmove (a0),crp
		lea.l 	(SSRP),%a0
		short	0xf010
		short	0x4800		#pmove (a0),srp
		lea.l 	(STC),%a0
		short	0xf010
		short	0x4000		#pmove (a0),tc
		lea.l 	(STT0),%a0
		short	0xf010
		short	0x0800		#pmove (a0),tt0
		lea.l 	(STT1),%a0
		short	0xf010
		short	0x0c00		#pmove (a0),tt1
		lea.l 	(SMMUSR),%a0
		short	0xf010
		short	0x6000		#pmove (a0),psr
#endif
#if(DEVICE==68040)
					#68040 instructions to effect supervisor
					#programer model support should be
					#inserted here !
#endif
#if(DEVICE==68040 || COPROCESSOR==TRUE)
		lea.l 	(FPCR),%a0
		short	0xf210
		short	0x9000 		#fmove.x (a0),fpcr
		lea.l 	(FPSR),%a0
		short	0xf210
		short	0x8800 		#fmove.x (a0),fpsr
		lea.l 	(FPIAR),%a0
		short	0xf210
		short	0x8400 		#fmove.x (a0),fpiar
		lea.l 	(FP0),%a0
		short	0xf210
		short	0xd0ff 		#fmovem.x (a0),fp0-fp7
#endif
		rte
backtomonitor:	mov.w	&0x2700,%sr
		jmp	abortstart
