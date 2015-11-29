; ModuleID = 'first_string.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"



; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %s = alloca i8*, align 8
  %t = alloca i8*, align 8
  ret i32 0
}

; Function Attrs: nounwind uwtable
define i8* @foo() #0 {
  %s = alloca i8*, align 8
  
  %R.1 = alloca i8*, align 8
  store i8* getelementptr inbounds ([12 x i8]* @.str, i32 0, i32 0), i8** %R.1, align 8
  
  %V.2 = load i8** %R.1, align 8
  store i8* %V.2, i8** %s, align 8
  
  %V.3 = load i8** %s, align 8
  ret i8* %V.3
}

@.str = private unnamed_addr constant [12 x i8] c"hello world\00", align 1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.6.2-1 (tags/RELEASE_362/final) (based on LLVM 3.6.2)"}
