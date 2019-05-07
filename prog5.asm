bipush 18
istore j
bipush 3
istore k
iload j
iload k
isub
istore i
iload i
bipush 14
if_icmpeq l1
iload j
bipush 4
isub
istore j
goto l2
l1 bipush 8
istore k
l2 iload k
