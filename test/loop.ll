; ModuleID = 'test/loop.bc'
source_filename = "test/loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @foo(i32 noundef %c, i32 noundef %z) #0 {
entry:
  %c.addr = alloca i32, align 4
  %z.addr = alloca i32, align 4
  %a = alloca i32, align 4
  %h = alloca i32, align 4
  %m = alloca i32, align 4
  %n = alloca i32, align 4
  %q = alloca i32, align 4
  %r = alloca i32, align 4
  %y = alloca i32, align 4
  store i32 %c, ptr %c.addr, align 4
  store i32 %z, ptr %z.addr, align 4
  store i32 9, ptr %a, align 4
  store i32 0, ptr %m, align 4
  store i32 0, ptr %n, align 4
  store i32 0, ptr %r, align 4
  store i32 0, ptr %y, align 4
  br label %LOOP

LOOP:                                             ; preds = %if.end8, %entry
  %0 = load i32, ptr %z.addr, align 4
  %add = add nsw i32 %0, 1
  store i32 %add, ptr %z.addr, align 4
  %1 = load i32, ptr %c.addr, align 4
  %add1 = add nsw i32 %1, 3
  store i32 %add1, ptr %y, align 4
  %2 = load i32, ptr %c.addr, align 4
  %add2 = add nsw i32 %2, 7
  store i32 %add2, ptr %q, align 4
  %3 = load i32, ptr %z.addr, align 4
  %cmp = icmp slt i32 %3, 5
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %LOOP
  %4 = load i32, ptr %a, align 4
  %add3 = add nsw i32 %4, 2
  store i32 %add3, ptr %a, align 4
  %5 = load i32, ptr %c.addr, align 4
  %add4 = add nsw i32 %5, 3
  store i32 %add4, ptr %h, align 4
  br label %if.end8

if.else:                                          ; preds = %LOOP
  %6 = load i32, ptr %a, align 4
  %sub = sub nsw i32 %6, 1
  store i32 %sub, ptr %a, align 4
  %7 = load i32, ptr %c.addr, align 4
  %add5 = add nsw i32 %7, 4
  store i32 %add5, ptr %h, align 4
  %8 = load i32, ptr %z.addr, align 4
  %cmp6 = icmp sge i32 %8, 10
  br i1 %cmp6, label %if.then7, label %if.end

if.then7:                                         ; preds = %if.else
  br label %EXIT

if.end:                                           ; preds = %if.else
  br label %if.end8

if.end8:                                          ; preds = %if.end, %if.then
  %9 = load i32, ptr %y, align 4
  %add9 = add nsw i32 %9, 7
  store i32 %add9, ptr %m, align 4
  %10 = load i32, ptr %h, align 4
  %add10 = add nsw i32 %10, 2
  store i32 %add10, ptr %n, align 4
  %11 = load i32, ptr %c.addr, align 4
  %add11 = add nsw i32 %11, 7
  store i32 %add11, ptr %y, align 4
  %12 = load i32, ptr %q, align 4
  %add12 = add nsw i32 %12, 5
  store i32 %add12, ptr %r, align 4
  br label %LOOP

EXIT:                                             ; preds = %if.then7
  %13 = load i32, ptr %a, align 4
  %14 = load i32, ptr %h, align 4
  %15 = load i32, ptr %m, align 4
  %16 = load i32, ptr %n, align 4
  %17 = load i32, ptr %q, align 4
  %18 = load i32, ptr %r, align 4
  %19 = load i32, ptr %y, align 4
  %20 = load i32, ptr %z.addr, align 4
  %call = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %13, i32 noundef %14, i32 noundef %15, i32 noundef %16, i32 noundef %17, i32 noundef %18, i32 noundef %19, i32 noundef %20)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  call void @foo(i32 noundef 0, i32 noundef 4)
  call void @foo(i32 noundef 0, i32 noundef 12)
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 17.0.6"}
