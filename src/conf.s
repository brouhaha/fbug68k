file "conf.s"
global conf
#global confres,conf
text

conf:	rts 

# Two versions of implementation are shown below

# conf: 	mov.l	([confres]),a0
# 		long 	4000 the confidence test would reside here and 
#			     conclude with an RTS

#conf:		mov.l	([confres]),a0
#		jsr	confidence a separate routine can be written to
#			perform the confidence test
#		rts
