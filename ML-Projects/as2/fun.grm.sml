functor FunLrValsFun(structure Token : TOKEN)
 : sig structure ParserData : PARSER_DATA
       structure Tokens : Fun_TOKENS
   end
 = 
struct
structure ParserData=
struct
structure Header = 
struct
(* 
   Authors:Yitzhak Mandelbaum, David Walker, Andrew Appel, etc.
   Description: Yacc file for "fun" language.
*)


structure F = Absyn


end
structure LrTable = Token.LrTable
structure Token = Token
local open LrTable in 
val table=let val actionRows =
"\
\\001\000\001\000\000\000\000\000\
\\001\000\002\000\046\000\003\000\045\000\007\000\044\000\010\000\043\000\
\\013\000\042\000\014\000\041\000\018\000\040\000\020\000\039\000\
\\022\000\038\000\023\000\037\000\025\000\036\000\028\000\035\000\000\000\
\\001\000\003\000\008\000\000\000\
\\001\000\003\000\010\000\000\000\
\\001\000\003\000\015\000\007\000\014\000\014\000\013\000\000\000\
\\001\000\003\000\057\000\000\000\
\\001\000\005\000\056\000\006\000\089\000\007\000\055\000\009\000\054\000\
\\010\000\053\000\011\000\052\000\014\000\051\000\015\000\050\000\
\\017\000\049\000\019\000\048\000\024\000\047\000\000\000\
\\001\000\005\000\056\000\007\000\055\000\008\000\085\000\009\000\054\000\
\\010\000\053\000\011\000\052\000\014\000\051\000\015\000\050\000\
\\017\000\049\000\019\000\048\000\024\000\047\000\000\000\
\\001\000\005\000\056\000\007\000\055\000\008\000\086\000\009\000\054\000\
\\010\000\053\000\011\000\052\000\014\000\051\000\015\000\050\000\
\\017\000\049\000\019\000\048\000\024\000\047\000\000\000\
\\001\000\005\000\056\000\007\000\055\000\009\000\054\000\010\000\053\000\
\\011\000\052\000\014\000\051\000\015\000\050\000\017\000\049\000\
\\019\000\048\000\021\000\082\000\024\000\047\000\000\000\
\\001\000\005\000\056\000\007\000\055\000\009\000\054\000\010\000\053\000\
\\011\000\052\000\014\000\051\000\015\000\050\000\017\000\049\000\
\\019\000\048\000\024\000\047\000\026\000\080\000\000\000\
\\001\000\005\000\056\000\007\000\055\000\009\000\054\000\010\000\053\000\
\\011\000\052\000\014\000\051\000\015\000\050\000\017\000\049\000\
\\019\000\048\000\024\000\047\000\027\000\093\000\000\000\
\\001\000\005\000\056\000\007\000\055\000\009\000\054\000\010\000\053\000\
\\011\000\052\000\014\000\051\000\015\000\050\000\017\000\049\000\
\\019\000\048\000\024\000\047\000\029\000\092\000\000\000\
\\001\000\006\000\011\000\000\000\
\\001\000\006\000\024\000\000\000\
\\001\000\007\000\009\000\000\000\
\\001\000\007\000\060\000\000\000\
\\001\000\008\000\018\000\022\000\017\000\031\000\016\000\000\000\
\\001\000\008\000\027\000\022\000\017\000\031\000\016\000\000\000\
\\001\000\008\000\097\000\022\000\017\000\031\000\016\000\000\000\
\\001\000\015\000\030\000\022\000\017\000\031\000\016\000\000\000\
\\001\000\015\000\079\000\000\000\
\\001\000\016\000\025\000\000\000\
\\001\000\016\000\083\000\000\000\
\\001\000\030\000\006\000\000\000\
\\099\000\000\000\
\\100\000\000\000\
\\101\000\030\000\006\000\000\000\
\\102\000\000\000\
\\103\000\005\000\056\000\007\000\055\000\009\000\054\000\010\000\053\000\
\\011\000\052\000\014\000\051\000\015\000\050\000\017\000\049\000\
\\019\000\048\000\024\000\047\000\000\000\
\\104\000\000\000\
\\105\000\000\000\
\\106\000\000\000\
\\107\000\000\000\
\\108\000\000\000\
\\109\000\000\000\
\\110\000\000\000\
\\111\000\007\000\055\000\009\000\054\000\010\000\053\000\011\000\052\000\
\\014\000\051\000\015\000\050\000\017\000\049\000\019\000\048\000\
\\024\000\047\000\000\000\
\\112\000\000\000\
\\113\000\000\000\
\\114\000\005\000\056\000\007\000\055\000\009\000\054\000\010\000\053\000\
\\011\000\052\000\014\000\051\000\015\000\050\000\017\000\049\000\
\\019\000\048\000\024\000\047\000\000\000\
\\115\000\005\000\056\000\007\000\055\000\009\000\054\000\010\000\053\000\
\\011\000\052\000\014\000\051\000\015\000\050\000\017\000\049\000\
\\019\000\048\000\024\000\047\000\000\000\
\\116\000\005\000\056\000\007\000\055\000\009\000\054\000\010\000\053\000\
\\011\000\052\000\014\000\051\000\015\000\050\000\017\000\049\000\
\\019\000\048\000\024\000\047\000\000\000\
\\117\000\000\000\
\\118\000\004\000\084\000\005\000\056\000\007\000\055\000\009\000\054\000\
\\010\000\053\000\011\000\052\000\014\000\051\000\015\000\050\000\
\\017\000\049\000\019\000\048\000\024\000\047\000\000\000\
\\119\000\000\000\
\\120\000\002\000\046\000\003\000\045\000\007\000\044\000\010\000\043\000\
\\013\000\042\000\014\000\041\000\018\000\040\000\020\000\039\000\
\\022\000\038\000\023\000\037\000\025\000\036\000\028\000\035\000\000\000\
\\121\000\007\000\055\000\009\000\054\000\010\000\053\000\011\000\052\000\
\\014\000\051\000\015\000\050\000\017\000\049\000\019\000\048\000\000\000\
\\122\000\000\000\
\\123\000\007\000\055\000\009\000\054\000\010\000\053\000\011\000\052\000\
\\014\000\051\000\015\000\050\000\000\000\
\\124\000\000\000\
\\125\000\007\000\055\000\011\000\052\000\000\000\
\\126\000\007\000\055\000\011\000\052\000\000\000\
\\127\000\007\000\055\000\000\000\
\\128\000\007\000\055\000\009\000\054\000\010\000\053\000\011\000\052\000\
\\014\000\051\000\015\000\050\000\000\000\
\\129\000\007\000\055\000\009\000\054\000\010\000\053\000\011\000\052\000\
\\014\000\051\000\015\000\050\000\000\000\
\\130\000\007\000\055\000\009\000\054\000\010\000\053\000\011\000\052\000\000\000\
\\131\000\007\000\055\000\009\000\054\000\010\000\053\000\011\000\052\000\000\000\
\\132\000\007\000\055\000\009\000\054\000\010\000\053\000\011\000\052\000\
\\014\000\051\000\015\000\050\000\017\000\049\000\019\000\048\000\000\000\
\\133\000\000\000\
\\134\000\000\000\
\\135\000\022\000\017\000\031\000\016\000\000\000\
\\136\000\000\000\
\\137\000\000\000\
\\138\000\000\000\
\\139\000\004\000\026\000\022\000\017\000\031\000\016\000\000\000\
\\140\000\000\000\
\\141\000\003\000\015\000\007\000\014\000\014\000\013\000\000\000\
\"
val actionRowNumbers =
"\024\000\028\000\027\000\025\000\
\\002\000\026\000\015\000\003\000\
\\013\000\004\000\017\000\067\000\
\\004\000\059\000\004\000\063\000\
\\014\000\022\000\066\000\065\000\
\\018\000\061\000\004\000\062\000\
\\004\000\060\000\020\000\064\000\
\\001\000\036\000\035\000\030\000\
\\029\000\005\000\001\000\001\000\
\\016\000\001\000\001\000\046\000\
\\001\000\001\000\001\000\032\000\
\\033\000\001\000\001\000\001\000\
\\001\000\001\000\001\000\001\000\
\\001\000\001\000\001\000\021\000\
\\010\000\047\000\001\000\009\000\
\\049\000\023\000\045\000\044\000\
\\039\000\050\000\007\000\058\000\
\\055\000\054\000\056\000\057\000\
\\053\000\052\000\051\000\008\000\
\\037\000\001\000\001\000\006\000\
\\001\000\038\000\001\000\031\000\
\\034\000\012\000\011\000\004\000\
\\041\000\043\000\001\000\001\000\
\\019\000\042\000\040\000\048\000\
\\000\000"
val gotoT =
"\
\\001\000\096\000\002\000\003\000\003\000\002\000\004\000\001\000\000\000\
\\000\000\
\\002\000\005\000\003\000\002\000\004\000\001\000\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\011\000\010\000\000\000\
\\000\000\
\\011\000\019\000\012\000\018\000\013\000\017\000\000\000\
\\011\000\020\000\000\000\
\\000\000\
\\011\000\021\000\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\011\000\026\000\000\000\
\\000\000\
\\011\000\019\000\012\000\027\000\000\000\
\\000\000\
\\000\000\
\\000\000\
\\005\000\032\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\005\000\056\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\057\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\000\000\
\\005\000\059\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\060\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\063\000\006\000\031\000\007\000\062\000\008\000\061\000\
\\009\000\030\000\010\000\029\000\000\000\
\\005\000\064\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\065\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\066\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\000\000\
\\000\000\
\\005\000\067\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\068\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\069\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\070\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\071\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\072\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\073\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\074\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\075\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\076\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\000\000\
\\000\000\
\\000\000\
\\005\000\079\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\005\000\085\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\086\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\000\000\
\\005\000\088\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\000\000\
\\005\000\063\000\006\000\031\000\007\000\089\000\009\000\030\000\
\\010\000\029\000\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\011\000\092\000\000\000\
\\000\000\
\\000\000\
\\005\000\093\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\005\000\094\000\006\000\031\000\009\000\030\000\010\000\029\000\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\\000\000\
\"
val numstates = 97
val numrules = 43
val s = ref "" and index = ref 0
val string_to_int = fn () => 
let val i = !index
in index := i+2; Char.ord(String.sub(!s,i)) + Char.ord(String.sub(!s,i+1)) * 256
end
val string_to_list = fn s' =>
    let val len = String.size s'
        fun f () =
           if !index < len then string_to_int() :: f()
           else nil
   in index := 0; s := s'; f ()
   end
val string_to_pairlist = fn (conv_key,conv_entry) =>
     let fun f () =
         case string_to_int()
         of 0 => EMPTY
          | n => PAIR(conv_key (n-1),conv_entry (string_to_int()),f())
     in f
     end
val string_to_pairlist_default = fn (conv_key,conv_entry) =>
    let val conv_row = string_to_pairlist(conv_key,conv_entry)
    in fn () =>
       let val default = conv_entry(string_to_int())
           val row = conv_row()
       in (row,default)
       end
   end
val string_to_table = fn (convert_row,s') =>
    let val len = String.size s'
        fun f ()=
           if !index < len then convert_row() :: f()
           else nil
     in (s := s'; index := 0; f ())
     end
local
  val memo = Array.array(numstates+numrules,ERROR)
  val _ =let fun g i=(Array.update(memo,i,REDUCE(i-numstates)); g(i+1))
       fun f i =
            if i=numstates then g i
            else (Array.update(memo,i,SHIFT (STATE i)); f (i+1))
          in f 0 handle Subscript => ()
          end
in
val entry_to_action = fn 0 => ACCEPT | 1 => ERROR | j => Array.sub(memo,(j-2))
end
val gotoT=Array.fromList(string_to_table(string_to_pairlist(NT,STATE),gotoT))
val actionRows=string_to_table(string_to_pairlist_default(T,entry_to_action),actionRows)
val actionRowNumbers = string_to_list actionRowNumbers
val actionT = let val actionRowLookUp=
let val a=Array.fromList(actionRows) in fn i=>Array.sub(a,i) end
in Array.fromList(map actionRowLookUp actionRowNumbers)
end
in LrTable.mkLrTable {actions=actionT,gotos=gotoT,numRules=numrules,
numStates=numstates,initialState=STATE 0}
end
end
local open Header in
type pos = ErrorMsg.pos
type arg = unit
structure MlyValue = 
struct
datatype svalue = VOID | ntVOID of unit ->  unit
 | PROJ of unit ->  (int) | ID of unit ->  (string)
 | INT of unit ->  (int) | tps of unit ->  (F.tp list)
 | some_tps of unit ->  (F.tp list) | tp of unit ->  (F.tp)
 | binOpExp of unit ->  (F.exp) | unOpExp of unit ->  (F.exp)
 | exps of unit ->  (F.exp list) | some_exps of unit ->  (F.exp list)
 | rexp of unit ->  (F.exp) | exp of unit ->  (F.exp)
 | rfundec of unit ->  (F.func) | fundec of unit ->  (F.fundec)
 | fundecs of unit ->  (F.fundec list) | program of unit ->  (F.prog)
end
type svalue = MlyValue.svalue
type result = F.prog
end
structure EC=
struct
open LrTable
infix 5 $$
fun x $$ y = y::x
val is_keyword =
fn _ => false
val preferred_change : (term list * term list) list = 
nil
val noShift = 
fn (T 0) => true | _ => false
val showTerminal =
fn (T 0) => "EOF"
  | (T 1) => "INT"
  | (T 2) => "ID"
  | (T 3) => "COMMA"
  | (T 4) => "SEMICOLON"
  | (T 5) => "COLON"
  | (T 6) => "LPAREN"
  | (T 7) => "RPAREN"
  | (T 8) => "PLUS"
  | (T 9) => "MINUS"
  | (T 10) => "TIMES"
  | (T 11) => "UMINUS"
  | (T 12) => "PROJ"
  | (T 13) => "LT"
  | (T 14) => "EQ"
  | (T 15) => "GT"
  | (T 16) => "AND"
  | (T 17) => "NOT"
  | (T 18) => "OR"
  | (T 19) => "WHILE"
  | (T 20) => "DO"
  | (T 21) => "REF"
  | (T 22) => "BANG"
  | (T 23) => "ASSIGN"
  | (T 24) => "IF"
  | (T 25) => "THEN"
  | (T 26) => "ELSE"
  | (T 27) => "LET"
  | (T 28) => "IN"
  | (T 29) => "FUN"
  | (T 30) => "ARROW"
  | (T 31) => "REF_TP"
  | _ => "bogus-term"
local open Header in
val errtermvalue=
fn (T 2) => MlyValue.ID(fn () => ("bogus")) | 
(T 1) => MlyValue.INT(fn () => (1)) | 
(T 12) => MlyValue.PROJ(fn () => (1)) | 
_ => MlyValue.VOID
end
val terms : term list = nil
 $$ (T 31) $$ (T 30) $$ (T 29) $$ (T 28) $$ (T 27) $$ (T 26) $$ (T 25)
 $$ (T 24) $$ (T 23) $$ (T 22) $$ (T 21) $$ (T 20) $$ (T 19) $$ (T 18)
 $$ (T 17) $$ (T 16) $$ (T 15) $$ (T 14) $$ (T 13) $$ (T 11) $$ (T 10)
 $$ (T 9) $$ (T 8) $$ (T 7) $$ (T 6) $$ (T 5) $$ (T 4) $$ (T 3) $$ (T 
0)end
structure Actions =
struct 
exception mlyAction of int
local open Header in
val actions = 
fn (i392,defaultPos,stack,
    (()):arg) =>
case (i392,stack)
of  ( 0, ( ( _, ( MlyValue.fundecs fundecs1, fundecs1left, 
fundecs1right)) :: rest671)) => let val  result = MlyValue.program (fn
 _ => let val  (fundecs as fundecs1) = fundecs1 ()
 in (fundecs)
end)
 in ( LrTable.NT 0, ( result, fundecs1left, fundecs1right), rest671)

end
|  ( 1, ( ( _, ( MlyValue.fundecs fundecs1, _, fundecs1right)) :: ( _,
 ( MlyValue.fundec fundec1, fundec1left, _)) :: rest671)) => let val  
result = MlyValue.fundecs (fn _ => let val  (fundec as fundec1) = 
fundec1 ()
 val  (fundecs as fundecs1) = fundecs1 ()
 in (fundec::fundecs)
end)
 in ( LrTable.NT 1, ( result, fundec1left, fundecs1right), rest671)

end
|  ( 2, ( ( _, ( MlyValue.fundec fundec1, fundec1left, fundec1right))
 :: rest671)) => let val  result = MlyValue.fundecs (fn _ => let val 
 (fundec as fundec1) = fundec1 ()
 in ([fundec])
end)
 in ( LrTable.NT 1, ( result, fundec1left, fundec1right), rest671)
end
|  ( 3, ( ( _, ( MlyValue.rfundec rfundec1, (rfundecleft as 
rfundec1left), (rfundecright as rfundec1right))) :: rest671)) => let
 val  result = MlyValue.fundec (fn _ => let val  (rfundec as rfundec1)
 = rfundec1 ()
 in ((rfundecleft,rfundecright), rfundec)
end)
 in ( LrTable.NT 2, ( result, rfundec1left, rfundec1right), rest671)

end
|  ( 4, ( ( _, ( MlyValue.exp exp1, _, exp1right)) :: _ :: ( _, ( 
MlyValue.tp tp2, _, _)) :: _ :: _ :: ( _, ( MlyValue.tp tp1, _, _)) ::
 _ :: ( _, ( MlyValue.ID ID2, _, _)) :: _ :: ( _, ( MlyValue.ID ID1, _
, _)) :: ( _, ( _, FUN1left, _)) :: rest671)) => let val  result = 
MlyValue.rfundec (fn _ => let val  ID1 = ID1 ()
 val  ID2 = ID2 ()
 val  tp1 = tp1 ()
 val  tp2 = tp2 ()
 val  (exp as exp1) = exp1 ()
 in ((Symbol.symbol ID1,Symbol.symbol ID2,tp1,tp2,exp))
end)
 in ( LrTable.NT 3, ( result, FUN1left, exp1right), rest671)
end
|  ( 5, ( ( _, ( MlyValue.rexp rexp1, (rexpleft as rexp1left), (
rexpright as rexp1right))) :: rest671)) => let val  result = 
MlyValue.exp (fn _ => let val  (rexp as rexp1) = rexp1 ()
 in (F.Pos((rexpleft,rexpright),rexp))
end)
 in ( LrTable.NT 4, ( result, rexp1left, rexp1right), rest671)
end
|  ( 6, ( ( _, ( _, _, RPAREN1right)) :: ( _, ( MlyValue.exp exp1, _,
 _)) :: ( _, ( _, LPAREN1left, _)) :: rest671)) => let val  result = 
MlyValue.rexp (fn _ => let val  (exp as exp1) = exp1 ()
 in (exp)
end)
 in ( LrTable.NT 5, ( result, LPAREN1left, RPAREN1right), rest671)
end
|  ( 7, ( ( _, ( MlyValue.ID ID1, ID1left, ID1right)) :: rest671)) =>
 let val  result = MlyValue.rexp (fn _ => let val  (ID as ID1) = ID1
 ()
 in (F.Id (Symbol.symbol ID))
end)
 in ( LrTable.NT 5, ( result, ID1left, ID1right), rest671)
end
|  ( 8, ( ( _, ( MlyValue.INT INT1, INT1left, INT1right)) :: rest671))
 => let val  result = MlyValue.rexp (fn _ => let val  (INT as INT1) = 
INT1 ()
 in (F.Int INT)
end)
 in ( LrTable.NT 5, ( result, INT1left, INT1right), rest671)
end
|  ( 9, ( ( _, ( _, _, RPAREN1right)) :: ( _, ( MlyValue.exp exp2, _,
 _)) :: _ :: ( _, ( MlyValue.exp exp1, exp1left, _)) :: rest671)) =>
 let val  result = MlyValue.rexp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Call (exp1,exp2))
end)
 in ( LrTable.NT 5, ( result, exp1left, RPAREN1right), rest671)
end
|  ( 10, ( ( _, ( MlyValue.unOpExp unOpExp1, unOpExp1left, 
unOpExp1right)) :: rest671)) => let val  result = MlyValue.rexp (fn _
 => let val  (unOpExp as unOpExp1) = unOpExp1 ()
 in (unOpExp)
end)
 in ( LrTable.NT 5, ( result, unOpExp1left, unOpExp1right), rest671)

end
|  ( 11, ( ( _, ( MlyValue.binOpExp binOpExp1, binOpExp1left, 
binOpExp1right)) :: rest671)) => let val  result = MlyValue.rexp (fn _
 => let val  (binOpExp as binOpExp1) = binOpExp1 ()
 in (binOpExp)
end)
 in ( LrTable.NT 5, ( result, binOpExp1left, binOpExp1right), rest671)

end
|  ( 12, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.rexp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Let (Symbol.symbol "$anon",exp1,exp2))
end)
 in ( LrTable.NT 5, ( result, exp1left, exp2right), rest671)
end
|  ( 13, ( ( _, ( _, _, GT1right)) :: ( _, ( MlyValue.exps exps1, _, _
)) :: ( _, ( _, LT1left, _)) :: rest671)) => let val  result = 
MlyValue.rexp (fn _ => let val  (exps as exps1) = exps1 ()
 in (F.Tuple exps)
end)
 in ( LrTable.NT 5, ( result, LT1left, GT1right), rest671)
end
|  ( 14, ( ( _, ( MlyValue.exp exp1, _, exp1right)) :: ( _, ( 
MlyValue.PROJ PROJ1, PROJ1left, _)) :: rest671)) => let val  result = 
MlyValue.rexp (fn _ => let val  (PROJ as PROJ1) = PROJ1 ()
 val  (exp as exp1) = exp1 ()
 in (F.Proj (PROJ,exp))
end)
 in ( LrTable.NT 5, ( result, PROJ1left, exp1right), rest671)
end
|  ( 15, ( ( _, ( MlyValue.exp exp3, _, exp3right)) :: _ :: ( _, ( 
MlyValue.exp exp2, _, _)) :: _ :: ( _, ( MlyValue.exp exp1, _, _)) :: 
( _, ( _, IF1left, _)) :: rest671)) => let val  result = MlyValue.rexp
 (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 val  exp3 = exp3 ()
 in (F.If (exp1,exp2,exp3))
end)
 in ( LrTable.NT 5, ( result, IF1left, exp3right), rest671)
end
|  ( 16, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, _, _)) :: ( _, ( _, WHILE1left, _)) :: rest671)) =>
 let val  result = MlyValue.rexp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.While (exp1,exp2))
end)
 in ( LrTable.NT 5, ( result, WHILE1left, exp2right), rest671)
end
|  ( 17, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, _, _)) :: _ :: ( _, ( MlyValue.ID ID1, _, _)) :: (
 _, ( _, LET1left, _)) :: rest671)) => let val  result = MlyValue.rexp
 (fn _ => let val  (ID as ID1) = ID1 ()
 val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Let (Symbol.symbol ID,exp1,exp2))
end)
 in ( LrTable.NT 5, ( result, LET1left, exp2right), rest671)
end
|  ( 18, ( ( _, ( MlyValue.some_exps some_exps1, _, some_exps1right))
 :: _ :: ( _, ( MlyValue.exp exp1, exp1left, _)) :: rest671)) => let
 val  result = MlyValue.some_exps (fn _ => let val  (exp as exp1) = 
exp1 ()
 val  (some_exps as some_exps1) = some_exps1 ()
 in (exp::some_exps)
end)
 in ( LrTable.NT 6, ( result, exp1left, some_exps1right), rest671)
end
|  ( 19, ( ( _, ( MlyValue.exp exp1, exp1left, exp1right)) :: rest671)
) => let val  result = MlyValue.some_exps (fn _ => let val  (exp as 
exp1) = exp1 ()
 in ([exp])
end)
 in ( LrTable.NT 6, ( result, exp1left, exp1right), rest671)
end
|  ( 20, ( ( _, ( MlyValue.some_exps some_exps1, some_exps1left, 
some_exps1right)) :: rest671)) => let val  result = MlyValue.exps (fn
 _ => let val  (some_exps as some_exps1) = some_exps1 ()
 in (some_exps)
end)
 in ( LrTable.NT 7, ( result, some_exps1left, some_exps1right), 
rest671)
end
|  ( 21, ( rest671)) => let val  result = MlyValue.exps (fn _ => (nil)
)
 in ( LrTable.NT 7, ( result, defaultPos, defaultPos), rest671)
end
|  ( 22, ( ( _, ( MlyValue.exp exp1, _, exp1right)) :: ( _, ( _, 
BANG1left, _)) :: rest671)) => let val  result = MlyValue.unOpExp (fn
 _ => let val  (exp as exp1) = exp1 ()
 in (F.Op (F.Get, [exp]))
end)
 in ( LrTable.NT 8, ( result, BANG1left, exp1right), rest671)
end
|  ( 23, ( ( _, ( _, _, RPAREN1right)) :: ( _, ( MlyValue.tp tp1, _, _
)) :: _ :: ( _, ( MlyValue.exp exp1, _, _)) :: _ :: ( _, ( _, REF1left
, _)) :: rest671)) => let val  result = MlyValue.unOpExp (fn _ => let
 val  (exp as exp1) = exp1 ()
 val  (tp as tp1) = tp1 ()
 in (F.Op (F.Ref, [F.Constrain(exp,tp)]))
end)
 in ( LrTable.NT 8, ( result, REF1left, RPAREN1right), rest671)
end
|  ( 24, ( ( _, ( MlyValue.exp exp1, _, exp1right)) :: ( _, ( _, 
NOT1left, _)) :: rest671)) => let val  result = MlyValue.unOpExp (fn _
 => let val  (exp as exp1) = exp1 ()
 in (F.If (exp,F.Int 0,F.Int 1))
end)
 in ( LrTable.NT 8, ( result, NOT1left, exp1right), rest671)
end
|  ( 25, ( ( _, ( MlyValue.exp exp1, _, exp1right)) :: ( _, ( _, 
MINUS1left, _)) :: rest671)) => let val  result = MlyValue.unOpExp (fn
 _ => let val  (exp as exp1) = exp1 ()
 in (F.Op (F.Sub,[F.Int 0, exp]))
end)
 in ( LrTable.NT 8, ( result, MINUS1left, exp1right), rest671)
end
|  ( 26, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.binOpExp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Op (F.Add,[exp1,exp2]))
end)
 in ( LrTable.NT 9, ( result, exp1left, exp2right), rest671)
end
|  ( 27, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.binOpExp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Op (F.Sub,[exp1,exp2]))
end)
 in ( LrTable.NT 9, ( result, exp1left, exp2right), rest671)
end
|  ( 28, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.binOpExp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Op (F.Mul,[exp1,exp2]))
end)
 in ( LrTable.NT 9, ( result, exp1left, exp2right), rest671)
end
|  ( 29, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.binOpExp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.If (exp1,F.If (exp2,F.Int 1,F.Int 0),F.Int 0))
end)
 in ( LrTable.NT 9, ( result, exp1left, exp2right), rest671)
end
|  ( 30, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.binOpExp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.If (exp1,F.Int 1,F.If (exp2,F.Int 1,F.Int 0)))
end)
 in ( LrTable.NT 9, ( result, exp1left, exp2right), rest671)
end
|  ( 31, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.binOpExp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Op (F.Eq,[exp1,exp2]))
end)
 in ( LrTable.NT 9, ( result, exp1left, exp2right), rest671)
end
|  ( 32, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.binOpExp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Op (F.LT,[exp1,exp2]))
end)
 in ( LrTable.NT 9, ( result, exp1left, exp2right), rest671)
end
|  ( 33, ( ( _, ( MlyValue.exp exp2, _, exp2right)) :: _ :: ( _, ( 
MlyValue.exp exp1, exp1left, _)) :: rest671)) => let val  result = 
MlyValue.binOpExp (fn _ => let val  exp1 = exp1 ()
 val  exp2 = exp2 ()
 in (F.Op (F.Set,[exp1,exp2]))
end)
 in ( LrTable.NT 9, ( result, exp1left, exp2right), rest671)
end
|  ( 34, ( ( _, ( MlyValue.ID ID1, (IDleft as ID1left), (IDright as 
ID1right))) :: rest671)) => let val  result = MlyValue.tp (fn _ => let
 val  (ID as ID1) = ID1 ()
 in (
if ID="int" then () 
                          else ErrorMsg.error((IDleft,IDright),"Expected int");
                          F.Inttp
)
end)
 in ( LrTable.NT 10, ( result, ID1left, ID1right), rest671)
end
|  ( 35, ( ( _, ( _, _, RPAREN1right)) :: ( _, ( MlyValue.tp tp1, _, _
)) :: ( _, ( _, LPAREN1left, _)) :: rest671)) => let val  result = 
MlyValue.tp (fn _ => let val  (tp as tp1) = tp1 ()
 in (tp)
end)
 in ( LrTable.NT 10, ( result, LPAREN1left, RPAREN1right), rest671)

end
|  ( 36, ( ( _, ( MlyValue.tp tp2, _, tp2right)) :: _ :: ( _, ( 
MlyValue.tp tp1, tp1left, _)) :: rest671)) => let val  result = 
MlyValue.tp (fn _ => let val  tp1 = tp1 ()
 val  tp2 = tp2 ()
 in (F.Arrowtp (tp1,tp2))
end)
 in ( LrTable.NT 10, ( result, tp1left, tp2right), rest671)
end
|  ( 37, ( ( _, ( _, _, GT1right)) :: ( _, ( MlyValue.tps tps1, _, _))
 :: ( _, ( _, LT1left, _)) :: rest671)) => let val  result = 
MlyValue.tp (fn _ => let val  (tps as tps1) = tps1 ()
 in (F.Tupletp tps)
end)
 in ( LrTable.NT 10, ( result, LT1left, GT1right), rest671)
end
|  ( 38, ( ( _, ( _, _, REF1right)) :: ( _, ( MlyValue.tp tp1, tp1left
, _)) :: rest671)) => let val  result = MlyValue.tp (fn _ => let val 
 (tp as tp1) = tp1 ()
 in (F.Reftp tp)
end)
 in ( LrTable.NT 10, ( result, tp1left, REF1right), rest671)
end
|  ( 39, ( ( _, ( MlyValue.some_tps some_tps1, _, some_tps1right)) ::
 _ :: ( _, ( MlyValue.tp tp1, tp1left, _)) :: rest671)) => let val  
result = MlyValue.some_tps (fn _ => let val  (tp as tp1) = tp1 ()
 val  (some_tps as some_tps1) = some_tps1 ()
 in (tp::some_tps)
end)
 in ( LrTable.NT 11, ( result, tp1left, some_tps1right), rest671)
end
|  ( 40, ( ( _, ( MlyValue.tp tp1, tp1left, tp1right)) :: rest671)) =>
 let val  result = MlyValue.some_tps (fn _ => let val  (tp as tp1) = 
tp1 ()
 in ([tp])
end)
 in ( LrTable.NT 11, ( result, tp1left, tp1right), rest671)
end
|  ( 41, ( ( _, ( MlyValue.some_tps some_tps1, some_tps1left, 
some_tps1right)) :: rest671)) => let val  result = MlyValue.tps (fn _
 => let val  (some_tps as some_tps1) = some_tps1 ()
 in (some_tps)
end)
 in ( LrTable.NT 12, ( result, some_tps1left, some_tps1right), rest671
)
end
|  ( 42, ( rest671)) => let val  result = MlyValue.tps (fn _ => (nil))
 in ( LrTable.NT 12, ( result, defaultPos, defaultPos), rest671)
end
| _ => raise (mlyAction i392)
end
val void = MlyValue.VOID
val extract = fn a => (fn MlyValue.program x => x
| _ => let exception ParseInternal
	in raise ParseInternal end) a ()
end
end
structure Tokens : Fun_TOKENS =
struct
type svalue = ParserData.svalue
type ('a,'b) token = ('a,'b) Token.token
fun EOF (p1,p2) = Token.TOKEN (ParserData.LrTable.T 0,(
ParserData.MlyValue.VOID,p1,p2))
fun INT (i,p1,p2) = Token.TOKEN (ParserData.LrTable.T 1,(
ParserData.MlyValue.INT (fn () => i),p1,p2))
fun ID (i,p1,p2) = Token.TOKEN (ParserData.LrTable.T 2,(
ParserData.MlyValue.ID (fn () => i),p1,p2))
fun COMMA (p1,p2) = Token.TOKEN (ParserData.LrTable.T 3,(
ParserData.MlyValue.VOID,p1,p2))
fun SEMICOLON (p1,p2) = Token.TOKEN (ParserData.LrTable.T 4,(
ParserData.MlyValue.VOID,p1,p2))
fun COLON (p1,p2) = Token.TOKEN (ParserData.LrTable.T 5,(
ParserData.MlyValue.VOID,p1,p2))
fun LPAREN (p1,p2) = Token.TOKEN (ParserData.LrTable.T 6,(
ParserData.MlyValue.VOID,p1,p2))
fun RPAREN (p1,p2) = Token.TOKEN (ParserData.LrTable.T 7,(
ParserData.MlyValue.VOID,p1,p2))
fun PLUS (p1,p2) = Token.TOKEN (ParserData.LrTable.T 8,(
ParserData.MlyValue.VOID,p1,p2))
fun MINUS (p1,p2) = Token.TOKEN (ParserData.LrTable.T 9,(
ParserData.MlyValue.VOID,p1,p2))
fun TIMES (p1,p2) = Token.TOKEN (ParserData.LrTable.T 10,(
ParserData.MlyValue.VOID,p1,p2))
fun UMINUS (p1,p2) = Token.TOKEN (ParserData.LrTable.T 11,(
ParserData.MlyValue.VOID,p1,p2))
fun PROJ (i,p1,p2) = Token.TOKEN (ParserData.LrTable.T 12,(
ParserData.MlyValue.PROJ (fn () => i),p1,p2))
fun LT (p1,p2) = Token.TOKEN (ParserData.LrTable.T 13,(
ParserData.MlyValue.VOID,p1,p2))
fun EQ (p1,p2) = Token.TOKEN (ParserData.LrTable.T 14,(
ParserData.MlyValue.VOID,p1,p2))
fun GT (p1,p2) = Token.TOKEN (ParserData.LrTable.T 15,(
ParserData.MlyValue.VOID,p1,p2))
fun AND (p1,p2) = Token.TOKEN (ParserData.LrTable.T 16,(
ParserData.MlyValue.VOID,p1,p2))
fun NOT (p1,p2) = Token.TOKEN (ParserData.LrTable.T 17,(
ParserData.MlyValue.VOID,p1,p2))
fun OR (p1,p2) = Token.TOKEN (ParserData.LrTable.T 18,(
ParserData.MlyValue.VOID,p1,p2))
fun WHILE (p1,p2) = Token.TOKEN (ParserData.LrTable.T 19,(
ParserData.MlyValue.VOID,p1,p2))
fun DO (p1,p2) = Token.TOKEN (ParserData.LrTable.T 20,(
ParserData.MlyValue.VOID,p1,p2))
fun REF (p1,p2) = Token.TOKEN (ParserData.LrTable.T 21,(
ParserData.MlyValue.VOID,p1,p2))
fun BANG (p1,p2) = Token.TOKEN (ParserData.LrTable.T 22,(
ParserData.MlyValue.VOID,p1,p2))
fun ASSIGN (p1,p2) = Token.TOKEN (ParserData.LrTable.T 23,(
ParserData.MlyValue.VOID,p1,p2))
fun IF (p1,p2) = Token.TOKEN (ParserData.LrTable.T 24,(
ParserData.MlyValue.VOID,p1,p2))
fun THEN (p1,p2) = Token.TOKEN (ParserData.LrTable.T 25,(
ParserData.MlyValue.VOID,p1,p2))
fun ELSE (p1,p2) = Token.TOKEN (ParserData.LrTable.T 26,(
ParserData.MlyValue.VOID,p1,p2))
fun LET (p1,p2) = Token.TOKEN (ParserData.LrTable.T 27,(
ParserData.MlyValue.VOID,p1,p2))
fun IN (p1,p2) = Token.TOKEN (ParserData.LrTable.T 28,(
ParserData.MlyValue.VOID,p1,p2))
fun FUN (p1,p2) = Token.TOKEN (ParserData.LrTable.T 29,(
ParserData.MlyValue.VOID,p1,p2))
fun ARROW (p1,p2) = Token.TOKEN (ParserData.LrTable.T 30,(
ParserData.MlyValue.VOID,p1,p2))
fun REF_TP (p1,p2) = Token.TOKEN (ParserData.LrTable.T 31,(
ParserData.MlyValue.VOID,p1,p2))
end
end
