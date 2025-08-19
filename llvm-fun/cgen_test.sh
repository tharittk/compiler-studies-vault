#!/bin/bash

echo "** Profiler Pass **"
./Debug+Asserts/bin/codegen ./tests/all.fun
opt -load ./Debug+Asserts/lib/Profile.so -bb-profiler -S -o all_opt.ll all.ll
lli all_opt.ll

echo "** Test ProfileLoader Pass **"
opt -load ./Debug+Asserts/lib/Profile.so -profile-printer  -dump-bb-info -disable-output all.ll

rm all.ll all_opt.ll
