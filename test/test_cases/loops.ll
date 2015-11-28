; ModuleID = 'loops.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @printd(i32 %ii) #0 {
entry:
  %i = alloca i32, align 4
  
 ; For_Instruction
 
  %R.11 = alloca i32, align 4
  store i32 -10, i32* %R.11
  %V.11 = load i32* %R.11, align 4
  store i32 %V.11, i32* %i
  br label %Label_0
  
  Label_0:
  %V.12 = load i32* %i, align 4
  %R.13 = alloca i32, align 4
  store i32 10, i32* %R.13
  %V.13 = load i32* %R.13, align 4
  %V.10 = icmp sle i32 %V.12, %V.13
  br i1 %V.10, label %Label_1, label %Label_3
  
  Label_1:
  ; /undefine Expression - Node Class/ 
  br label %Label_2
  
  Label_2:
  %V.15 = load i32* %i, align 4
  %R.15 = alloca i32, align 4
  store i32 1, i32* %R.15
  %V.16 = load i32* %R.15, align 4
  %V.17 = add i32 %V.15, %V.16
  %R.16 = alloca i32, align 4
  store i32 %V.17, i32* %R.16
  %V.14 = load i32* %R.16, align 4
  store i32 %V.14, i32* %i
  br label %Label_0
  
  Label_3:

  
  %R.0 = alloca i32, align 4
  store i32 0, i32* %R.0
  %V.1 = load i32* %R.0, align 4
  ret i32 %V.1

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
