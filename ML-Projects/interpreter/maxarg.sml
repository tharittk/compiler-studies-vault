structure MaxArg = struct

    structure S = SLP
    (* Use S.CompoundStm, S.Plus, S.IdExp, etc. to refer to
     * items defined in structure SLP (see slp.sml) *)

    exception MaxUnimplemented

    fun maxarg s = 
      case s of
        S.CompoundStm(s1, s2) => Int.max(maxarg s1, maxarg s2)
      | S.AssignStm(x, e) => maxExpArg e
      | S.PrintStm elist => List.length (elist)

    and maxExpArg e =
      case e of
      S.OpExp (e1, _, e2)=> Int.max(maxExpArg e1, maxExpArg e2)  
      | S.EseqExp (s, e) => Int.max(maxarg s, maxExpArg e)
      | _ => 0
end
