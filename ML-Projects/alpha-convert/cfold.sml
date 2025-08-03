signature REWRITE =
sig
  val rename_prog: Absyn.prog -> Absyn.prog
  val cfold_prog : Absyn.prog -> Absyn.prog
  val my_test: Absyn.prog
  val run: unit -> unit

end

structure Rewrite :> REWRITE =
struct

 structure A = Absyn
 structure S = Symbol
     
 type env = S.symbol S.table

 fun bind (rho:env, id : S.symbol) : env * S.symbol =
   (* Keep adding prime to the name until find the one that is not in use 
    your environment looks like
      x ->
      x'->
      ...
      x''' -> then you return this one with x4
   *)
    let 
      fun fresh s = 
        case S.look(rho, S.symbol s) of
          NONE => s
          | SOME _ => fresh (s ^ "'")
      
      val base_name = S.name id
      val new_name = fresh base_name
      val new_sym = S.symbol new_name
    in
      (S.enter(rho, id, new_sym), new_sym) 
    end

 fun rename_prog (fds : A.prog): A.prog  = 
    let 
      val env0: env = S.empty
      fun rename_exp (env, e) =
        case e of
        A.Int (i) => A.Int i
        | A.Id (id) => 
          (* Look for naming symbol in the current environment *)
          let 
            val SOME current_sym = S.look(env, id)
          in
            A.Id current_sym
          end
        | A.Op (oper, expList) => A.Op (oper, unroll_exp(env, expList))
        | A.Tuple (expList) => A.Tuple (unroll_exp(env, expList))
        | A.Proj (col, e1) => A.Proj (col, rename_exp(env, e1))
        | A.If (e1, e2, e3) => A.If (rename_exp(env, e1), rename_exp(env, e2), rename_exp(env, e3))
        | A.While (e1, e2) => A.While (rename_exp(env, e1), rename_exp(env, e2))
        | A.Call (e1, e2) => A.Call (rename_exp(env, e1), rename_exp(env, e2))
        | A.Constrain (e1, t1) => A.Constrain (rename_exp(env, e1), t1)
        | A.Pos (err, e1) =>  A.Pos (err, rename_exp(env, e1))
        | A.Let (id, e1, e2) => 
          case S.look(env, id) of
          (* Id not yet exists, no need to rename, use the current environment *)
          NONE => A.Let (id, rename_exp(env, e1), rename_exp(env, e2))
          (* Create new environment *)
          | SOME _ =>
            let 
              val (new_env, new_sym) = bind(env, id)
            in
              (* e1 uses the original environment but e2 use new  *)
              A.Let (new_sym, e1, rename_exp(new_env, e2))
            end

      and unroll_exp (env, e) =
        case e of
        [] => []
        | head::e' => rename_exp(env, head)::unroll_exp(env, e')

      fun rename_fundec env (pos, (fname, argname, t1, t2, e)) =
        let 
          val env_with_f = S.enter(env, fname, fname)
          val (env_with_f_arg, arg_sym) = bind(env_with_f, argname)
        in
          (pos, (fname, arg_sym, t1, t2, rename_exp(env_with_f_arg, e)))
        end 
    in
      List.map (rename_fundec env0) fds
    end

 fun cfold_prog p = 
   (* replace this stub with something useful *)
        p

  (* fun f(x:int) : int =
    let a = x + 2 in
      let x = a*a in
        x + 1 
   *)

  open Absyn
  val p : ErrorMsg.pos*ErrorMsg.pos = (0,0)
  val id = Symbol.symbol

  val my_test = [(p, (id "myfunctiontest", id "x", Inttp, Inttp, Let (id "a", Op(Add, [Id(id "x"), Int 2]),
      Let(id "x", Op(Mul, [Id(id "a"), Id (id "a")]), Op (Add, [Id (id "x"), Int 1])))) )]

  fun run() = Test.runprog (rename_prog my_test)
end
