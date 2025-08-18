#!/bin/bash

echo "Test all.func"
../Debug+Asserts/bin/codegen all.fun
opt -load ../Debug+Asserts/lib/Profile.so -bb-profiler -S -o all_opt.ll all.ll
lli all_opt.ll
