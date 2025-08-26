#!/bin/bash

echo "*** Test BB and Edges Profiler ***"

rm prof_dump.txt
opt -break-crit-edges -mem2reg -instnamer -S -o ./tests/namer.ll ./tests/superblock.ll
opt -load Debug+Asserts/lib/Profile.so -bb-profiler -edge-profiler -S -o ./tests/prof.ll ./tests/namer.ll
# interpret to get the text output
lli ./tests/prof.ll
opt -load Debug+Asserts/lib/Profile.so -bb-profile-loader -edge-profile-loader -profile-printer -dump-bb-info -dump-edge-info -stats -disable-output ./tests/namer.ll
