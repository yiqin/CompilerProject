; Define function 'foo'
define i32 @foo(i32 %count, i8* %s) {
entry:
%count.pointer = alloca i32
store i32 %count, i32* %count.pointer
%s.pointer = alloca i8*
store i8* %s, i8** %s.pointer
%str1 = alloca i8*, align 8
%i = alloca i32, align 4
%sum = alloca i32, align 4
%sum2 = alloca i32, align 4
store i32 0, i32* %sum
store i32 1, i32* %count.pointer
store i32 2, i32* %count.pointer
store i32 4, i32* %count.pointer

; For_Instruction

store i32 1, i32* %i
br label %Label_0

Label_0:
%i.1 = load i32* %i
%count.pointer.1 = load i32* %count.pointer
%tmp.10 = icmp sle i32 %i.1, %count.pointer.1
br i1 %tmp.10, label %Label_1, label %Label_3

Label_1:
%sum.1 = load i32* %sum
store i32 %sum.1, i32* %sum
br label %Label_2

Label_2:
%i.2 = load i32* %i
%tmp.16 = add i32 %i.2, 1
store i32 %tmp.16, i32* %i
br label %Label_0

Label_3:
store i32 1, i32* %sum2
%str.0 = getelementptr inbounds [6 x i8]* @.str.0, i32 0, i32 0
store i8* %str.0, i8** %str1
%str.1 = getelementptr inbounds [5 x i8]* @.str.1, i32 0, i32 0
%str.2 = getelementptr inbounds [5 x i8]* @.str.2, i32 0, i32 0
%sum.2 = load i32* %sum
ret i32 %sum.2
}
@.str.0 = private unnamed_addr constant [6 x i8] c"hello\00"
@.str.1 = private unnamed_addr constant [5 x i8] c"word\00"
@.str.2 = private unnamed_addr constant [5 x i8] c"word\00"

; Define function 'main'
define i32 @main() {
entry:
%i = alloca i32, align 4
%s = alloca i8*, align 8
%str.3 = getelementptr inbounds [6 x i8]* @.str.3, i32 0, i32 0
store i8* %str.3, i8** %s
%s.1 = load i8** %s
%tmp.29 = call i32 (i32, i8*)* @foo(i32 2, i8* %s.1)
store i32 %tmp.29, i32* %i
%i.1 = load i32* %i
ret i32 %i.1
}
@.str.3 = private unnamed_addr constant [6 x i8] c"world\00"

