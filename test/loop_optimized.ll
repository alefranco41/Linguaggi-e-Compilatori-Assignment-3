; ModuleID = 'test/loop_mem2reg.ll'
source_filename = "test/loop.ll"

@.str = private constant [25 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

define void @foo(i32 %0, i32 %1) {
  br label %3

3:                                                ; preds = %15, %2
  %.05 = phi i32 [ 0, %2 ], [ %19, %15 ]
  %.04 = phi i32 [ 0, %2 ], [ %17, %15 ]
  %.03 = phi i32 [ 0, %2 ], [ %16, %15 ]
  %.01 = phi i32 [ 9, %2 ], [ %.1, %15 ]
  %.0 = phi i32 [ %1, %2 ], [ %4, %15 ]
  %4 = add nsw i32 %.0, 1
  %5 = add nsw i32 %0, 3
  %6 = add nsw i32 %0, 7
  %7 = icmp slt i32 %4, 5
  br i1 %7, label %8, label %11

8:                                                ; preds = %3
  %9 = add nsw i32 %.01, 2
  %10 = add nsw i32 %0, 3
  br label %15

11:                                               ; preds = %3
  %12 = sub nsw i32 %.01, 1
  %13 = add nsw i32 %0, 4
  %14 = icmp sge i32 %4, 10
  br i1 %14, label %20, label %15

15:                                               ; preds = %11, %8
  %.02 = phi i32 [ %10, %8 ], [ %13, %11 ]
  %.1 = phi i32 [ %9, %8 ], [ %12, %11 ]
  %16 = add nsw i32 %5, 7
  %17 = add nsw i32 %.02, 2
  %18 = add nsw i32 %0, 7
  %19 = add nsw i32 %6, 5
  br label %3

20:                                               ; preds = %11
  %.lcssa4 = phi i32 [ %12, %11 ]
  %.lcssa3 = phi i32 [ %13, %11 ]
  %.05.lcssa = phi i32 [ %.05, %11 ]
  %.04.lcssa = phi i32 [ %.04, %11 ]
  %.03.lcssa = phi i32 [ %.03, %11 ]
  %.lcssa2 = phi i32 [ %4, %11 ]
  %.lcssa1 = phi i32 [ %5, %11 ]
  %.lcssa = phi i32 [ %6, %11 ]
  %21 = call i32 (ptr, ...) @printf(ptr @.str, i32 %.lcssa4, i32 %.lcssa3, i32 %.03.lcssa, i32 %.04.lcssa, i32 %.lcssa, i32 %.05.lcssa, i32 %.lcssa1, i32 %.lcssa2)
  ret void
}

declare i32 @printf(ptr, ...)

define i32 @main() {
  call void @foo(i32 0, i32 4)
  call void @foo(i32 0, i32 12)
  ret i32 0
}
