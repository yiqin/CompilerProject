; ModuleID = 'src/string_lib.cpp'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define zeroext i1 @__string_equal__(i8* %lhs, i8* %rhs) #0 {
  %1 = alloca i8*, align 8
  %2 = alloca i8*, align 8
  store i8* %lhs, i8** %1, align 8
  store i8* %rhs, i8** %2, align 8
  %3 = load i8*, i8** %1, align 8
  %4 = load i8*, i8** %2, align 8
  %5 = call i32 @strcmp(i8* %3, i8* %4) #3
  %6 = icmp ne i32 %5, 0
  %7 = xor i1 %6, true
  ret i1 %7
}

; Function Attrs: nounwind readonly
declare i32 @strcmp(i8*, i8*) #1

; Function Attrs: nounwind uwtable
define zeroext i1 @__string_not_equal__(i8* %lhs, i8* %rhs) #0 {
  %1 = alloca i8*, align 8
  %2 = alloca i8*, align 8
  store i8* %lhs, i8** %1, align 8
  store i8* %rhs, i8** %2, align 8
  %3 = load i8*, i8** %1, align 8
  %4 = load i8*, i8** %2, align 8
  %5 = call i32 @strcmp(i8* %3, i8* %4) #3
  %6 = icmp ne i32 %5, 0
  ret i1 %6
}

; Function Attrs: nounwind uwtable
define i8* @__string_copy__(i8* %lhs) #0 {
  %1 = alloca i8*, align 8
  %lhs_len = alloca i64, align 8
  %result = alloca i8*, align 8
  store i8* %lhs, i8** %1, align 8
  %2 = load i8*, i8** %1, align 8
  %3 = call i64 @strlen(i8* %2) #3
  store i64 %3, i64* %lhs_len, align 8
  %4 = load i64, i64* %lhs_len, align 8
  %5 = add i64 %4, 1
  %6 = mul i64 1, %5
  %7 = call noalias i8* @malloc(i64 %6) #4
  store i8* %7, i8** %result, align 8
  %8 = load i8*, i8** %result, align 8
  %9 = load i8*, i8** %1, align 8
  %10 = call i8* @strcpy(i8* %8, i8* %9) #4
  %11 = load i64, i64* %lhs_len, align 8
  %12 = load i8*, i8** %result, align 8
  %13 = getelementptr inbounds i8, i8* %12, i64 %11
  store i8 0, i8* %13, align 1
  %14 = load i8*, i8** %result, align 8
  ret i8* %14
}

; Function Attrs: nounwind readonly
declare i64 @strlen(i8*) #1

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #2

; Function Attrs: nounwind
declare i8* @strcpy(i8*, i8*) #2

; Function Attrs: nounwind uwtable
define i8* @__string_concat__(i8* %lhs, i8* %rhs) #0 {
  %1 = alloca i8*, align 8
  %2 = alloca i8*, align 8
  %lhs_len = alloca i64, align 8
  %rhs_len = alloca i64, align 8
  %result = alloca i8*, align 8
  store i8* %lhs, i8** %1, align 8
  store i8* %rhs, i8** %2, align 8
  %3 = load i8*, i8** %1, align 8
  %4 = call i64 @strlen(i8* %3) #3
  store i64 %4, i64* %lhs_len, align 8
  %5 = load i8*, i8** %2, align 8
  %6 = call i64 @strlen(i8* %5) #3
  store i64 %6, i64* %rhs_len, align 8
  %7 = load i64, i64* %lhs_len, align 8
  %8 = load i64, i64* %rhs_len, align 8
  %9 = add i64 %7, %8
  %10 = add i64 %9, 1
  %11 = mul i64 1, %10
  %12 = call noalias i8* @malloc(i64 %11) #4
  store i8* %12, i8** %result, align 8
  %13 = load i8*, i8** %result, align 8
  %14 = load i8*, i8** %1, align 8
  %15 = call i8* @strcpy(i8* %13, i8* %14) #4
  %16 = load i8*, i8** %result, align 8
  %17 = load i64, i64* %lhs_len, align 8
  %18 = getelementptr inbounds i8, i8* %16, i64 %17
  %19 = load i8*, i8** %2, align 8
  %20 = call i8* @strcpy(i8* %18, i8* %19) #4
  %21 = load i64, i64* %lhs_len, align 8
  %22 = load i64, i64* %rhs_len, align 8
  %23 = add i64 %21, %22
  %24 = load i8*, i8** %result, align 8
  %25 = getelementptr inbounds i8, i8* %24, i64 %23
  store i8 0, i8* %25, align 1
  %26 = load i8*, i8** %result, align 8
  ret i8* %26
}

; Function Attrs: nounwind uwtable
define void @__string_free__(i8* %ptr) #0 {
  %1 = alloca i8*, align 8
  store i8* %ptr, i8** %1, align 8
  %2 = load i8*, i8** %1, align 8
  call void @free(i8* %2) #4
  ret void
}

; Function Attrs: nounwind
declare void @free(i8*) #2

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readonly "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind readonly }
attributes #4 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.7.0 (tags/RELEASE_370/final)"}
