; ModuleID = 'superblock.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [11 x i8] c"i:%d y=%d\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %y = alloca i32, align 4
  %z = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %1
  store i32 5, i32* %i, align 4
  br label %2

; <label>:2:                                       ; preds = %15, %0
  %3 = load i32* %i, align 4
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %5, label %21

; <label>:5                                       ; preds = %2
  %6 = load i32* %i, align 4
  %7 = icmp sgt i32 %6, 1
  br i1 %7, label %8, label %9

; <label>:8                                       ; preds = %5
  store i32 4, i32* %y, align 4
  br label %10

; <label>:9                                       ; preds = %5
  store i32 3, i32* %y, align 4
  br label %10

; <label>:10                                      ; preds = %9, %8
  %11 = load i32* %y, align 4
  %12 = sdiv i32 %11, 2
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %14, label %15

; <label>:14                                      ; preds = %10
  store i32 1, i32* %z, align 4
  br label %15

; <label>:15                                      ; preds = %14, %10
  %16 = load i32* %i, align 4
  %17 = load i32* %y, align 4
  %18 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([11 x i8]* @.str, i32 0, i32 0), i32 %16, i32 %17)
  %19 = load i32* %i, align 4
  %20 = add nsw i32 %19, -1
  store i32 %20, i32* %i, align 4
  br label %2

; <label>:21                                      ; preds = %2
  ret i32 0
}

declare i32 @printf(i8*, ...) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
