define i32 @printd(i32 %i) #0 {
  %1 = alloca i32, align 4
  store i32 %i, i32* %1, align 4
  ret i32 1
}

define i32 @main(i32 %a) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, i32* %1
  store i32 %a, i32* %2, align 4
  store i32 450, i32* %i, align 4
  store i32 -123, i32* %j, align 4
  %3 = load i32* %i, align 4
  %4 = load i32* %j, align 4
  %5 = add nsw i32 %3, %4
  %6 = call i32 @printd(i32 %5)
  ret i32 0
}