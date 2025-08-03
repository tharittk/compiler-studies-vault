structure Myfun = struct
open Absyn Eval

val p : ErrorMsg.pos*ErrorMsg.pos = (0,0)
val id = Symbol.symbol

(*
fun f (x: int) : <int,int> = <x,x+1>
fun main(y: int) : int = #1(f(y))
*)

val myfun = [ 
  (p, (id "f", id "x", Inttp, Tupletp [Inttp, Inttp], Tuple[
    Id(id "x"), Op(Add, [Id(id "x"), Int 1])])),
  (p, (id "main", id "y", Inttp, Inttp, Proj(1, Call(Id(id "f"), Id(id "y")))))
  ] 

fun run() = Test.runprog myfun

end
