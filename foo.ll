; Define function 'foo'
define i32 @foo(i32 %count) {
entry:
%count.pointer = alloca i32
store i32 %count, i32* %count.pointer
%sum = alloca i32
store i32 0, i32* %sum
%sum2 = alloca i32
store i32 1, i32* %sum2
store i32 1, i32* %count.pointer
store i32 2, i32* %count.pointer

; For_Instruction

%i = alloca i32
store i32 1, i32* %i
br label %Label_0

Label_0:
%i.3 = load i32* %i
%count.pointer.4 = load i32* %count.pointer
%tmp.10 = icmp sle i32 %i.3, %count.pointer.4
br i1 %tmp.10, label %Label_1, label %Label_3

Label_1:
%sum.4 = load i32* %sum
store i32 %sum.4, i32* %sum
br label %Label_2

Label_2:
%i.5 = load i32* %i
%tmp.16 = add i32 %i.5, 1
store i32 %tmp.16, i32* %i
br label %Label_0

Label_3:
%str1 = alloca i8*
%str.0 = getelementptr inbounds [6 x i8]* @.str.0, i32 0, i32 0
%sum.5 = load i32* %sum
ret i32 %sum.5
}
; Define function 'main'
@.str.0 = private unnamed_addr constant [6 x i8] c"hello\00"
define i32 @main() {
entry:
%i = alloca i32
%tmp.23 = call i32 (i32)* @foo(i32 2)
store i32 %tmp.23, i32* %i
%i.3 = load i32* %i
ret i32 %i.3
}
