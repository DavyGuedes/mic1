bipush 7
istore j
bipush 3
istore k

loop iload j
bipush 7
iadd
istore j

iload k
bipush 1
isub
istore k

iload k
bipush 1
if_icmpeq l1
goto loop
l1 iload j


