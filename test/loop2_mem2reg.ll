; ModuleID = 'test/loop2_mem2reg.bc'
source_filename = "test/loop2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [6 x i8] c"%d%d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @foo(i32 noundef %a, i32 noundef %b) #0 {
entry:
  %mul = mul nsw i32 %a, 2
  %cmp = icmp sgt i32 %mul, 0
  br i1 %cmp, label %if.then, label %if.end3

if.then:                                          ; preds = %entry
  br label %body

body:                                             ; preds = %if.end, %if.then
  %a.addr.0 = phi i32 [ %a, %if.then ], [ %inc, %if.end ]
  %inc = add nsw i32 %a.addr.0, 1
  %add = add nsw i32 %mul, 5
  %cmp1 = icmp sgt i32 %inc, 10
  br i1 %cmp1, label %if.then2, label %if.end

if.then2:                                         ; preds = %body
  br label %exit

if.end:                                           ; preds = %body
  br label %body

if.end3:                                          ; preds = %entry
  br label %exit

exit:                                             ; preds = %if.end3, %if.then2
  %b.addr.0 = phi i32 [ %add, %if.then2 ], [ %b, %if.end3 ]
  %a.addr.1 = phi i32 [ %inc, %if.then2 ], [ %a, %if.end3 ]
  %call = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %a.addr.1, i32 noundef %b.addr.0)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
  call void @foo(i32 noundef 1, i32 noundef 2)
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
