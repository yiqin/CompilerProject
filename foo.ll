; Define function 'foo'
define i32 @foo(i32 %count) {
entry:
%sum = alloca i32
store i32 0, i32* %sum
%sum2 = alloca i32
store i32 1, i32* %sum2

; For_Instruction

%i = alloca i32
store i32 1, i32* %i
br label %Label_0

Label_0:
%i.3 = load i32* %i
%tmp.6 = icmp sle i32 %i.3, %count
br i1 %tmp.6, label %Label_1, label %Label_3

Label_1:
%sum.4 = load i32* %sum
store i32 %sum.4, i32* %sum
br label %Label_2

Label_2:
%i.5 = load i32* %i
%tmp.12 = add i32 %i.5, 1
store i32 %tmp.12, i32* %i
br label %Label_0

Label_3:
%sum.5 = load i32* %sum
ret i32 %sum.5
}
; Define function 'main'
define i32 @main() {
entry:
%i = alloca i32
%tmp.17 = call i32 (i32)* @foo(i32 2)
store i32 %tmp.17, i32* %i
%i.3 = load i32* %i
ret i32 %i.3
}
