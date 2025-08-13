#!/bin/bash

echo "Test small.func"
../Debug+Asserts/bin/codegen small.fun
llc small.ll
clang small.s -no-pie
./a.out
rm a.out

echo "Test all.func"
../Debug+Asserts/bin/codegen all.fun
llc all.ll
clang all.s -no-pie -o allt
./allt
rm allt
