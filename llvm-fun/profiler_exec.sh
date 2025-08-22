#!/bin/bash

echo "*** Test BB and Edges Profiler ***"

opt -break-crit-edges -S -o bench.ll ./tests/all.ll
opt -load Debug+Asserts/lib/Profile.so -bb-profiler -edge-profiler  -S -o bench.prof.ll bench.ll
lli bench.prof.ll
opt -load Debug+Asserts/lib/Profile.so -bb-profile-loader -edge-profile-loader -profile-printer -dump-bb-info -dump-edge-info -stats -disable-output bench.ll
