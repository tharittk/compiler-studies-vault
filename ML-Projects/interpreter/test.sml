structure Test = struct

    fun interp () = Interp.interp TestProg.prog
    fun maxarg () = MaxArg.maxarg TestProg.prog

end
val _ = print ("Max args: " ^ Int.toString (Test.maxarg ()) ^ "\n");
Test.interp();
