; ModuleID = 'dbtranslator/Ssss.cpp'
source_filename = "dbtranslator/Ssss.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.SegmentManager = type { ptr, i32, i32 }

; Function Attrs: mustprogress nofree norecurse nosync nounwind memory(read, inaccessiblemem: write) uwtable
define dso_local zeroext i8 @read8(ptr nocapture noundef readonly %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %4 = load i32, ptr %3, align 8, !tbaa !5
  %5 = zext i32 %4 to i64
  %6 = icmp ne i32 %4, 0
  tail call void @llvm.assume(i1 %6)
  %7 = load ptr, ptr %0, align 8, !tbaa !12
  br label %8

8:                                                ; preds = %25, %2
  %9 = phi i64 [ 0, %2 ], [ %26, %25 ]
  %10 = getelementptr inbounds nuw %struct.SegmentManager, ptr %7, i64 %9
  %11 = getelementptr inbounds nuw i8, ptr %10, i64 12
  %12 = load i32, ptr %11, align 4, !tbaa !13
  %13 = icmp ugt i32 %12, %1
  br i1 %13, label %25, label %14

14:                                               ; preds = %8
  %15 = getelementptr inbounds nuw i8, ptr %10, i64 8
  %16 = load i32, ptr %15, align 8, !tbaa !16
  %17 = add i32 %16, %12
  %18 = icmp ult i32 %1, %17
  br i1 %18, label %19, label %25

19:                                               ; preds = %14
  %20 = load ptr, ptr %10, align 8, !tbaa !17
  %21 = sub i32 %1, %12
  %22 = zext i32 %21 to i64
  %23 = getelementptr inbounds nuw i8, ptr %20, i64 %22
  %24 = load i8, ptr %23, align 1, !tbaa !18
  ret i8 %24

25:                                               ; preds = %14, %8
  %26 = add nuw nsw i64 %9, 1
  %27 = icmp ne i64 %26, %5
  tail call void @llvm.assume(i1 %27)
  br label %8
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind memory(read, inaccessiblemem: write) uwtable
define dso_local zeroext i16 @read16(ptr nocapture noundef readonly %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %4 = load i32, ptr %3, align 8, !tbaa !5
  %5 = zext i32 %4 to i64
  %6 = icmp ne i32 %4, 0
  tail call void @llvm.assume(i1 %6)
  %7 = load ptr, ptr %0, align 8, !tbaa !12
  br label %8

8:                                                ; preds = %25, %2
  %9 = phi i64 [ 0, %2 ], [ %26, %25 ]
  %10 = getelementptr inbounds nuw %struct.SegmentManager, ptr %7, i64 %9
  %11 = getelementptr inbounds nuw i8, ptr %10, i64 12
  %12 = load i32, ptr %11, align 4, !tbaa !13
  %13 = icmp ugt i32 %12, %1
  br i1 %13, label %25, label %14

14:                                               ; preds = %8
  %15 = getelementptr inbounds nuw i8, ptr %10, i64 8
  %16 = load i32, ptr %15, align 8, !tbaa !16
  %17 = add i32 %16, %12
  %18 = icmp ult i32 %1, %17
  br i1 %18, label %19, label %25

19:                                               ; preds = %14
  %20 = load ptr, ptr %10, align 8, !tbaa !17
  %21 = sub i32 %1, %12
  %22 = zext i32 %21 to i64
  %23 = getelementptr inbounds nuw i8, ptr %20, i64 %22
  %24 = load i16, ptr %23, align 2, !tbaa !19
  ret i16 %24

25:                                               ; preds = %14, %8
  %26 = add nuw nsw i64 %9, 1
  %27 = icmp ne i64 %26, %5
  tail call void @llvm.assume(i1 %27)
  br label %8
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind memory(read, inaccessiblemem: write) uwtable
define dso_local i32 @read32(ptr nocapture noundef readonly %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %4 = load i32, ptr %3, align 8, !tbaa !5
  %5 = zext i32 %4 to i64
  %6 = icmp ne i32 %4, 0
  tail call void @llvm.assume(i1 %6)
  %7 = load ptr, ptr %0, align 8, !tbaa !12
  br label %8

8:                                                ; preds = %25, %2
  %9 = phi i64 [ 0, %2 ], [ %26, %25 ]
  %10 = getelementptr inbounds nuw %struct.SegmentManager, ptr %7, i64 %9
  %11 = getelementptr inbounds nuw i8, ptr %10, i64 12
  %12 = load i32, ptr %11, align 4, !tbaa !13
  %13 = icmp ugt i32 %12, %1
  br i1 %13, label %25, label %14

14:                                               ; preds = %8
  %15 = getelementptr inbounds nuw i8, ptr %10, i64 8
  %16 = load i32, ptr %15, align 8, !tbaa !16
  %17 = add i32 %16, %12
  %18 = icmp ult i32 %1, %17
  br i1 %18, label %19, label %25

19:                                               ; preds = %14
  %20 = load ptr, ptr %10, align 8, !tbaa !17
  %21 = sub i32 %1, %12
  %22 = zext i32 %21 to i64
  %23 = getelementptr inbounds nuw i8, ptr %20, i64 %22
  %24 = load i32, ptr %23, align 4, !tbaa !21
  ret i32 %24

25:                                               ; preds = %14, %8
  %26 = add nuw nsw i64 %9, 1
  %27 = icmp ne i64 %26, %5
  tail call void @llvm.assume(i1 %27)
  br label %8
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: write) uwtable
define dso_local void @write8(ptr nocapture noundef readonly %0, i32 noundef %1, i8 noundef zeroext %2) local_unnamed_addr #1 {
  %4 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %5 = load i32, ptr %4, align 8, !tbaa !5
  %6 = zext i32 %5 to i64
  %7 = icmp ne i32 %5, 0
  tail call void @llvm.assume(i1 %7)
  %8 = load ptr, ptr %0, align 8, !tbaa !12
  br label %9

9:                                                ; preds = %25, %3
  %10 = phi i64 [ 0, %3 ], [ %26, %25 ]
  %11 = getelementptr inbounds nuw %struct.SegmentManager, ptr %8, i64 %10
  %12 = getelementptr inbounds nuw i8, ptr %11, i64 12
  %13 = load i32, ptr %12, align 4, !tbaa !13
  %14 = icmp ugt i32 %13, %1
  br i1 %14, label %25, label %15

15:                                               ; preds = %9
  %16 = getelementptr inbounds nuw i8, ptr %11, i64 8
  %17 = load i32, ptr %16, align 8, !tbaa !16
  %18 = add i32 %17, %13
  %19 = icmp ult i32 %1, %18
  br i1 %19, label %20, label %25

20:                                               ; preds = %15
  %21 = load ptr, ptr %11, align 8, !tbaa !17
  %22 = sub i32 %1, %13
  %23 = zext i32 %22 to i64
  %24 = getelementptr inbounds nuw i8, ptr %21, i64 %23
  store i8 %2, ptr %24, align 1, !tbaa !18
  ret void

25:                                               ; preds = %15, %9
  %26 = add nuw nsw i64 %10, 1
  %27 = icmp ne i64 %26, %6
  tail call void @llvm.assume(i1 %27)
  br label %9
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: write) uwtable
define dso_local void @write16(ptr nocapture noundef readonly %0, i32 noundef %1, i16 noundef zeroext %2) local_unnamed_addr #1 {
  %4 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %5 = load i32, ptr %4, align 8, !tbaa !5
  %6 = zext i32 %5 to i64
  %7 = icmp ne i32 %5, 0
  tail call void @llvm.assume(i1 %7)
  %8 = load ptr, ptr %0, align 8, !tbaa !12
  br label %9

9:                                                ; preds = %25, %3
  %10 = phi i64 [ 0, %3 ], [ %26, %25 ]
  %11 = getelementptr inbounds nuw %struct.SegmentManager, ptr %8, i64 %10
  %12 = getelementptr inbounds nuw i8, ptr %11, i64 12
  %13 = load i32, ptr %12, align 4, !tbaa !13
  %14 = icmp ugt i32 %13, %1
  br i1 %14, label %25, label %15

15:                                               ; preds = %9
  %16 = getelementptr inbounds nuw i8, ptr %11, i64 8
  %17 = load i32, ptr %16, align 8, !tbaa !16
  %18 = add i32 %17, %13
  %19 = icmp ult i32 %1, %18
  br i1 %19, label %20, label %25

20:                                               ; preds = %15
  %21 = load ptr, ptr %11, align 8, !tbaa !17
  %22 = sub i32 %1, %13
  %23 = zext i32 %22 to i64
  %24 = getelementptr inbounds nuw i8, ptr %21, i64 %23
  store i16 %2, ptr %24, align 2, !tbaa !19
  ret void

25:                                               ; preds = %15, %9
  %26 = add nuw nsw i64 %10, 1
  %27 = icmp ne i64 %26, %6
  tail call void @llvm.assume(i1 %27)
  br label %9
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: write) uwtable
define dso_local void @write32(ptr nocapture noundef readonly %0, i32 noundef %1, i32 noundef %2) local_unnamed_addr #1 {
  %4 = getelementptr inbounds nuw i8, ptr %0, i64 8
  %5 = load i32, ptr %4, align 8, !tbaa !5
  %6 = zext i32 %5 to i64
  %7 = icmp ne i32 %5, 0
  tail call void @llvm.assume(i1 %7)
  %8 = load ptr, ptr %0, align 8, !tbaa !12
  br label %9

9:                                                ; preds = %25, %3
  %10 = phi i64 [ 0, %3 ], [ %26, %25 ]
  %11 = getelementptr inbounds nuw %struct.SegmentManager, ptr %8, i64 %10
  %12 = getelementptr inbounds nuw i8, ptr %11, i64 12
  %13 = load i32, ptr %12, align 4, !tbaa !13
  %14 = icmp ugt i32 %13, %1
  br i1 %14, label %25, label %15

15:                                               ; preds = %9
  %16 = getelementptr inbounds nuw i8, ptr %11, i64 8
  %17 = load i32, ptr %16, align 8, !tbaa !16
  %18 = add i32 %17, %13
  %19 = icmp ult i32 %1, %18
  br i1 %19, label %20, label %25

20:                                               ; preds = %15
  %21 = load ptr, ptr %11, align 8, !tbaa !17
  %22 = sub i32 %1, %13
  %23 = zext i32 %22 to i64
  %24 = getelementptr inbounds nuw i8, ptr %21, i64 %23
  store i32 %2, ptr %24, align 4, !tbaa !21
  ret void

25:                                               ; preds = %15, %9
  %26 = add nuw nsw i64 %10, 1
  %27 = icmp ne i64 %26, %6
  tail call void @llvm.assume(i1 %27)
  br label %9
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: write)
declare void @llvm.assume(i1 noundef) #2

attributes #0 = { mustprogress nofree norecurse nosync nounwind memory(read, inaccessiblemem: write) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: write) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: write) }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 20.1.3 (++20250415115000+9420327ad768-1~exp1~20250415235030.102)"}
!5 = !{!6, !11, i64 8}
!6 = !{!"_ZTS13MemoryManager", !7, i64 0, !11, i64 8}
!7 = !{!"p1 _ZTS14SegmentManager", !8, i64 0}
!8 = !{!"any pointer", !9, i64 0}
!9 = !{!"omnipotent char", !10, i64 0}
!10 = !{!"Simple C++ TBAA"}
!11 = !{!"int", !9, i64 0}
!12 = !{!6, !7, i64 0}
!13 = !{!14, !11, i64 12}
!14 = !{!"_ZTS14SegmentManager", !15, i64 0, !11, i64 8, !11, i64 12}
!15 = !{!"p1 omnipotent char", !8, i64 0}
!16 = !{!14, !11, i64 8}
!17 = !{!14, !15, i64 0}
!18 = !{!9, !9, i64 0}
!19 = !{!20, !20, i64 0}
!20 = !{!"short", !9, i64 0}
!21 = !{!11, !11, i64 0}
