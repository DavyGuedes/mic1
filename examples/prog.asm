   bipush 10
   istore j
   bipush 10
   istore k
   iload j
   iload k
   iadd
   istore i
   iload i
   bipush 25
   if_icmpeq l1
   iload j
   bipush 1
   isub
   istore j
   goto l2
l1 bipush 13
   istore k
l2 bipush 25
