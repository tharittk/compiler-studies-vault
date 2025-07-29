fun f0(x:<>):<>=  a;b
fun f1(x:<>):<>=  if a then b else c
fun f2(x:<>):<>=  if a then b else c ; d
fun f3(x:<>):<>=  if a then b ; d
fun f4(x:<>):<>=  if a then if b then c else d
fun f5(x:<>):<>=  if if a then b else c then if b then c else d
fun f6(x:<>):<>=  a*b+c*d
fun f7(x:<>):<>=  a+b-c
fun f8(x:<>):<>=  !a+!b
fun f9(x:<>):<>=  a := b ; c := d
fun f10(x:<>):<>=  (f)(x)
fun f11(x:<>):<>=  a + b : int ref
fun f12(x:<>):<>=  if a = b then c else d : int
fun f13(x:<>):<>=  <if a then b , c>
fun f14(x:<>):<>=  !a*ref b+#8 c=-d &not e||h:int := j:int ref;<>
fun f15(x:<>):<>=  /* comment */ a
fun f16(x:<>):<>=  /* comment /* nested comment */ */ a
fun main(x:int):int = 5
