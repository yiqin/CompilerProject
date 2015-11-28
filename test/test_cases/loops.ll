; ModuleID = 'loops.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @printd(i32 %i) #0 {
  %1 = alloca i32, align 4
  ; store i32 %i, i32* %1, align 4
  ; ret i32 1
  
  %a = alloca i32, align 4
  %R.11 = load i32* %a, align 4
  %R.5 = alloca i32, align 4
  store i32 1, i32* %R.5
  %R.12 = load i32* %R.5, align 4
  %R.13 = add i32 %R.11, %R.12
  %R.7 = alloca i32, align 4
  store i32 %R.13, i32* %R.7
  %R.10 = load i32* %R.7, align 4
  %R.6 = alloca i32, align 4
  store i32 2, i32* %R.6
  %R.14 = load i32* %R.6, align 4
  %R.15 = sub i32 %R.10, %R.14
  %R.8 = alloca i32, align 4
  store i32 %R.15, i32* %R.8
  %R.9 = load i32* %R.8, align 4
  ret i32 %R.9
}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %1
  store i32 0, i32* %i, align 4
  
  %R.15 = alloca i32, align 4
  store i32 450, i32* %R.15
;  store i32 %R.15, i32* %R.17
;  %i = load i32* %R.15, align 4
  %V.17 = load i32* %R.15, align 4
  store i32 %V.17, i32* %i, align 4

  
  store i32 -10, i32* %i, align 4
  
  
  br label %2

; <label>:2                                       ; preds = %8, %0
  %3 = load i32* %i, align 4
  %4 = icmp sle i32 %3, 10
  
  br i1 %4, label %5, label %11

; <label>:5                                       ; preds = %2
  %6 = load i32* %i, align 4
  %7 = call i32 @printd(i32 %6)
  br label %8

; <label>:8                                       ; preds = %5
  %9 = load i32* %i, align 4
  %10 = add nsw i32 %9, 1
  store i32 %10, i32* %i, align 4
  br label %2

; <label>:11                                      ; preds = %2


  ret i32 0
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.6.2-1 (tags/RELEASE_362/final) (based on LLVM 3.6.2)"}
