bipush 10
istore x
bipush 15
istore y
iload y
iload x
isub
bipush 5
if_icmpeq l1
goto l2
l1 bipush 1
l2 bipush 2
