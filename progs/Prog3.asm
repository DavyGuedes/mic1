bipush 10
istore j
bipush 4
istore k
iload j
iload k
iadd
istore i
iload i
bipush 3
if_icmpeq l1
iload j
bipush 1
isub
istore i
goto L2
bipush 0
l1 bipush 0
istore k
l2 bipush 3