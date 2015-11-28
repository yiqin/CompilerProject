; ModuleID = 'cond.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, i32* %1
  store i32 450, i32* %i, align 4
  store i32 -123, i32* %j, align 4
  %2 = load i32* %i, align 4
  %3 = add nsw i32 %2, 1
  %4 = load i32* %j, align 4
  %5 = add nsw i32 %4, 0
  %6 = icmp slt i32 %3, %5
  br i1 %6, label %7, label %10

; <label>:7                                       ; preds = %0
  %8 = load i32* %i, align 4
  %9 = call i32 @printd(i32 %8)
  br label %13

; <label>:10                                      ; preds = %0
  %11 = load i32* %j, align 4
  %12 = call i32 @printd(i32 %11)
  br label %13

; <label>:13                                      ; preds = %10, %7
  ret i32 0
}

define i32 @printd(i32 %i) #0 {
  ret i32 0;
}

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.6.2-1 (tags/RELEASE_362/final) (based on LLVM 3.6.2)"}
