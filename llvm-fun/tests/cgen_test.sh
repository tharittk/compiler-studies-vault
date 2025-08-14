#!/bin/bash

echo "Test all.func"
../Debug+Asserts/bin/codegen all.fun
llc all.ll
clang all.s -no-pie -o allt
./allt
rm allt
