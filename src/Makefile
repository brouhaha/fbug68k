CFLGS = -g


unixbug : coldunix.o main.o printunix.o getlineunix.o general.o \
	bf.o bm.o br.o bs.o dc.o he.o lo.o rd.o rm.o tm.o \
	mm.o md.o go.o tr.o sd.o as.o asm68.o disasm68.o asmcode68.o \
	handler.o fproutines.o 68def.o asmprocunix.o 
	cc coldunix.o  68def.o \
	printunix.o getlineunix.o general.o asmprocunix.o main.o bf.o bm.o \
	bs.o dc.o he.o lo.o rd.o rm.o tm.o mm.o md.o go.o tr.o br.o \
	sd.o as.o disasm68.o asm68.o asmcode68.o handler.o fproutines.o \
	-g -o cbug040TT 

portbug : coldport.o main.o printport.o getlineport.o general.o \
	asmprocport.o \
	bf.o bm.o br.o bs.o dc.o he.o lo.o rd.o rm.o tm.o \
	mm.o md.o go.o tr.o sd.o as.o asm68.o disasm68.o asmcode68.o \
	asmhandler.o handler.o  asmstartport.o conf.o fproutines.o 68def.o 
	ld ./ifilemb  asmstartport.o asmhandler.o coldport.o \
	printport.o getlineport.o general.o asmprocport.o main.o bf.o bm.o \
	bs.o dc.o he.o lo.o rd.o rm.o tm.o mm.o md.o go.o tr.o br.o \
	sd.o as.o disasm68.o asm68.o asmcode68.o handler.o conf.o \
	fproutines.o  68def.o \
	-m -o fbug040TT >memmap
	ubuilds fbug040TT

coldport.o :  textdef.h vardef.h userdef.h targetsys.h
coldunix.o :  textdef.h vardef.h userdef.h targetsys.h

asmstartport.o : textdef.h vardef.h userdef.h targetsys.h
		/lib/cpp asmstartport.s  > temp.s
		as  -o asmstartport.o temp.s
		rm temp.s
asmcode68.o : textdef.h vardef.h userdef.h targetsys.h
		/lib/cpp asmcode68.s  > temp.s
		as  -o asmcode68.o temp.s
		rm temp.s
asmhandler.o : textdef.h vardef.h userdef.h targetsys.h
		/lib/cpp asmhandler.s  > temp.s
		as  -o asmhandler.o temp.s
		rm temp.s

