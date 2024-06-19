; ModuleID = 'test/loop_mem2reg.ll'
source_filename = "test/loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @foo(i32 noundef %c, i32 noundef %z) #0 {
entry:
  %add1 = add nsw i32 %c, 3
  %add2 = add nsw i32 %c, 7
  %add11 = add nsw i32 %c, 7
  br label %LOOP

LOOP:                                             ; preds = %if.end8, %entry
  %r.0 = phi i32 [ 0, %entry ], [ %add12, %if.end8 ]
  %n.0 = phi i32 [ 0, %entry ], [ %add10, %if.end8 ]
  %m.0 = phi i32 [ 0, %entry ], [ %add9, %if.end8 ]
  %a.0 = phi i32 [ 9, %entry ], [ %a.1, %if.end8 ]
  %z.addr.0 = phi i32 [ %z, %entry ], [ %add, %if.end8 ]
  %add = add nsw i32 %z.addr.0, 1
  %cmp = icmp slt i32 %add, 5
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %LOOP
  %add3 = add nsw i32 %a.0, 2
  %add4 = add nsw i32 %c, 3
  br label %if.end8

if.else:                                          ; preds = %LOOP
  %sub = sub nsw i32 %a.0, 1
  %add5 = add nsw i32 %c, 4
  %cmp6 = icmp sge i32 %add, 10
  br i1 %cmp6, label %if.then7, label %if.end

if.then7:                                         ; preds = %if.else
  %sub.lcssa = phi i32 [ %sub, %if.else ]
  %add5.lcssa = phi i32 [ %add5, %if.else ]
  %r.0.lcssa = phi i32 [ %r.0, %if.else ]
  %n.0.lcssa = phi i32 [ %n.0, %if.else ]
  %m.0.lcssa = phi i32 [ %m.0, %if.else ]
  %add.lcssa = phi i32 [ %add, %if.else ]
  %add1.lcssa = phi i32 [ %add1, %if.else ]
  %add2.lcssa = phi i32 [ %add2, %if.else ]
  br label %EXIT

if.end:                                           ; preds = %if.else
  br label %if.end8

if.end8:                                          ; preds = %if.end, %if.then
  %h.0 = phi i32 [ %add4, %if.then ], [ %add5, %if.end ]
  %a.1 = phi i32 [ %add3, %if.then ], [ %sub, %if.end ]
  %add9 = add nsw i32 %add1, 7
  %add10 = add nsw i32 %h.0, 2
  %add12 = add nsw i32 %add2, 5
  br label %LOOP

EXIT:                                             ; preds = %if.then7
  %call = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %sub.lcssa, i32 noundef %add5.lcssa, i32 noundef %m.0.lcssa, i32 noundef %n.0.lcssa, i32 noundef %add2.lcssa, i32 noundef %r.0.lcssa, i32 noundef %add1.lcssa, i32 noundef %add.lcssa)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
entry:
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
