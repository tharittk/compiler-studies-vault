structure Interp = struct

    structure S = SLP
    (* Use S.CompoundStm, S.Plus, S.IdExp, etc. to refer to
     * items defined in structure SLP (see slp.sml) *)
    
    type table = (S.id * int) list

    fun update(tbl, ident, newval) = (ident, newval)::tbl
    (*assume the first occurrence of any id takes precedence
      over any later occurrences in the table*)

    exception UndefinedVariable of string

    fun lookup (tbl:table, ident) = 
          case tbl of 
            nil => raise UndefinedVariable(ident)
          | (x, xval)::xs => if ident=x then xval
			     else lookup(xs, ident)

    exception InterpUnimplemented

    fun interpOp S.Plus  = Int.+
      | interpOp S.Minus = Int.-
      | interpOp S.Times = Int.*
      | interpOp S.Div   = Int.div

    fun interpStm (s:S.stm, tbl:table) = 
      case s of
        S.CompoundStm(s1, s2) =>
        let val tbl1 = interpStm(s1, tbl)
            val tbl2 = interpStm(s2, tbl1)
        in
          tbl2
        end
      | S.AssignStm (iden, e1) =>
        let val (e1_val, tbl1) = interpExp(e1, tbl)
        in
        update(tbl1, iden, e1_val)
        end
      | S.PrintStm elist => 
        case elist of
        [] => tbl
        | head::elist' => 
          (* iterate over element in the list *)
          let val (tmp, tbl1) = interpExp (head, tbl)
          in
          print(Int.toString(tmp) ^ "\n");
          interpStm(S.PrintStm(elist'), tbl1)
          end

    and interpExp (e:S.exp, tbl:table) = 
      case e of 
        S.IdExp id => (lookup(tbl, id), tbl)
      | S.NumExp n => (n, tbl)
      | S.OpExp (e1, ops, e2) =>
        let val (e1_val, tbl1) = interpExp(e1, tbl)
        val (e2_val, tbl2) = interpExp(e2, tbl1)
        in
          (interpOp ops (e1_val, e2_val), tbl2)
        end
      | S.EseqExp (stm, e1) =>
      (* evaluate the stetement for side effect used in e1 `first *)
        let val tbl1 = interpStm(stm, tbl)
        in
        interpExp(e1, tbl1)
        end

    fun interp s = 
          (interpStm(s, nil); ())
end
