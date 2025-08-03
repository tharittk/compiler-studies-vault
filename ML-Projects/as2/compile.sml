structure Compile =
 
struct 

  exception Stop
fun still_ok () = 
  if (!ErrorMsg.anyErrors) then raise Stop else ()

fun parse (filename: string) : Absyn.prog = 
     Parse.parse (filename, TextIO.openIn filename)

fun run (p: Absyn.prog) =
(print "Program:\n"; 
	    FunPP.print_prog p;
	    print "Running Program...\n";
	    let val v = Eval.eval_prog p
             in print "\nProgram Returns:\n";
	        FunPP.print_val v;
	        print "\n"
            end)

fun compile filename = 
    let 
        val absyn = parse filename
        val () = still_ok()
        val () = print "ORIGINAL PROGRAM\n"
        val () = run absyn
        val absyn1 = Rewrite.rename_prog absyn
        val () = print "ALPHA CONVERSION\n"
        val () = run absyn1
        val () = print "CONSTANT FOLDING\n"
        val absyn2 = Rewrite.cfold_prog absyn
        val () = run absyn2
    in ()
    end
 handle ErrorMsg.Error => print "\nCompiler bug.\n\n" 
      | Stop => print "\nCompilation Failed.\n\n" 
end
