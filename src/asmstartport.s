#include "targetsys.h"
file "asmstart.s"
global start,abortstart
text


start:		
		mov.l	&0x1,(SYSRAMLOC+0x1000)
abortstart:	mov.l	&SYSRAMLOC+0x1b00,%d0
#if(DEVICE>=68010)
		mov.l	&SYSRAMLOC+0x1000,%d1
#endif
#if(DEVICE>=68020)
		mov.l	&SYSRAMLOC+0x2000,%d2
		mov.l	&SYSRAMLOC+0x1c00,%d3
#endif
		mov.l	&0x0,%d4
		mov.l	&0x0,%d5
		mov.l	&0x0,%d6
		mov.l	&0x0,%d7
		mov.l	&0x0,%a0
		mov.l	&0x0,%a1
		mov.l	&0x0,%a2
		mov.l	&0x0,%a3
		mov.l	&0x0,%a4
		mov.l	&0x0,%a5
		mov.l	&0x0,%a6
#if(DEVICE>=68020)
		mov.l	%d4,%caar
		mov.l	%d4,%cacr
		mov.l	%d4,%sfc
		mov.l	%d4,%dfc
#endif
		mov.l	%d0,%usp
#if(DEVICE>=68010)
		mov.l	%d1,%vbr
#endif
#if(DEVICE>=68020)
		mov.l	%d2,%isp
		mov.l	%d3,%msp
#endif
		mov.l	&0x0,%d0
		mov.l	&0x0,%d1
		mov.l	&0x0,%d2
		mov.l	&0x0,%d3
#if(DEVICE==68030)
		lea.l 	CRP_INIT,%a0
		short	0xf010
		short	0x4c00		#pmove (a0),crp
		short	0xf010
		short	0x4800		#pmove (a0),srp
		lea.l 	LONG_0,%a0
		short	0xf010
		short	0x4000		#pmove (a0),tc
		short	0xf010
		short	0x0800		#pmove (a0),tt0
		short	0xf010
		short	0x0c00		#pmove (a0),tt1
		short	0xf010
		short	0x6000		#pmove (a0),psr
#endif
#if(DEVICE==68040 || COPROCESSOR==TRUE)
		short	0xf23c
		short	0x9000
		short	0x0000
		short	0x0000 		#fmove.l #0,fpcr
		short	0xf23c
		short	0x8800
		short	0x0000
		short	0x0000 		#fmove.l #0,fpsr
		short	0xf23c
		short	0x8400
		short	0x0000
		short	0x0000 		#fmove.l #0,fpiar
		short	0xf23c
		short	0x4000
		short	0x1234
		short	0x5678 		#fmove.x #0,fp0
		short	0xf23c
		short	0x4080
		short	0x0000
		short	0x0000 		#fmove.x #0,fp1
		short	0xf23c
		short	0x4100
		short	0x0000
		short	0x0000 		#fmove.x #0,fp2
		short	0xf23c
		short	0x4180
		short	0x0000
		short	0x0000 		#fmove.x #0,fp3
		short	0xf23c
		short	0x4200
		short	0x0000
		short	0x0000 		#fmove.x #0,fp4
		short	0xf23c
		short	0x4280
		short	0x0000
		short	0x0000 		#fmove.x #0,fp5
		short	0xf23c
		short	0x4300
		short	0x0000
		short	0x0000 		#fmove.x #0,fp6
		short	0xf23c
		short	0x4380
		short	0x0000
		short	0x0000 		#fmove.x #0,fp7
#endif
		jmp	main

#if(DEVICE==68030)
CRP_INIT:	long	0x1
LONG_0:		long	0x0
#endif



