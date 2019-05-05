bipush 20
istore j
bipush 5
istore k
bipush 0
istore q
loop iload j
iload k
isub
istore aux
iload q
bipush 1
iadd
istore q
ifeq l1
iload aux
istore j
iload k
isub
goto loop
l1 iload q 

