; ModuleID = 'linkmap.c'
source_filename = "linkmap.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.link_map = type { i64, ptr, ptr, ptr, ptr, ptr, i64, ptr, [80 x ptr], ptr, i64, i16, i16, %struct.r_scope_elem, %struct.r_scope_elem, ptr, ptr, i32, i32, i32, i32, ptr, %union.anon.0, %union.anon.1, i32, [3 x i8], i8, i8, i8, i32, i32, i32, %struct.r_search_path_struct, ptr, ptr, ptr, i64, i64, i64 }
%struct.r_scope_elem = type { ptr, i32 }
%union.anon.0 = type { ptr }
%union.anon.1 = type { ptr }
%struct.r_search_path_struct = type { ptr, i32 }

; Function Attrs: nofree nounwind uwtable
define hidden ptr @find_linkmap(i64 noundef %starting_point) local_unnamed_addr #0 {
entry:
  %and = and i64 %starting_point, -4096
  br label %while.cond

while.cond:                                       ; preds = %while.cond, %entry
  %elf.0 = phi i64 [ %and, %entry ], [ %sub, %while.cond ]
  %0 = inttoptr i64 %elf.0 to ptr
  %1 = load i32, ptr %0, align 4096, !tbaa !5
  %cmp.not = icmp eq i32 %1, 1179403647
  %sub = add i64 %elf.0, -4096
  br i1 %cmp.not, label %while.end, label %while.cond, !llvm.loop !9

while.end:                                        ; preds = %while.cond
  %2 = tail call ptr @llvm.frameaddress.p0(i32 0)
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %while.end
  %ptr.0 = phi ptr [ %2, %while.end ], [ %incdec.ptr, %for.inc ]
  %3 = load i64, ptr %ptr.0, align 8, !tbaa !12
  %cmp1.not = icmp eq i64 %3, 7
  br i1 %cmp1.not, label %lor.rhs, label %for.inc

lor.rhs:                                          ; preds = %for.cond
  %arrayidx2 = getelementptr inbounds i64, ptr %ptr.0, i64 1
  %4 = load i64, ptr %arrayidx2, align 8, !tbaa !12
  %cmp3.not = icmp eq i64 %4, %elf.0
  br i1 %cmp3.not, label %for.cond4, label %for.inc

for.inc:                                          ; preds = %for.cond, %lor.rhs
  %incdec.ptr = getelementptr inbounds i64, ptr %ptr.0, i64 1
  br label %for.cond, !llvm.loop !14

for.cond4:                                        ; preds = %lor.rhs, %for.inc11
  %ptr.1 = phi ptr [ %add.ptr, %for.inc11 ], [ %ptr.0, %lor.rhs ]
  %5 = load i64, ptr %ptr.1, align 8, !tbaa !12
  %tobool.not = icmp eq i64 %5, 0
  br i1 %tobool.not, label %lor.rhs6, label %for.inc11

lor.rhs6:                                         ; preds = %for.cond4
  %arrayidx7 = getelementptr inbounds i64, ptr %ptr.1, i64 1
  %6 = load i64, ptr %arrayidx7, align 8, !tbaa !12
  %tobool8.not = icmp eq i64 %6, 0
  br i1 %tobool8.not, label %for.body15, label %for.inc11

for.inc11:                                        ; preds = %for.cond4, %lor.rhs6
  %add.ptr = getelementptr inbounds i64, ptr %ptr.1, i64 2
  br label %for.cond4, !llvm.loop !15

for.body15:                                       ; preds = %lor.rhs6, %for.inc25
  %indvars.iv = phi i64 [ %indvars.iv.next, %for.inc25 ], [ 2, %lor.rhs6 ]
  %7 = sub nsw i64 0, %indvars.iv
  %arrayidx17 = getelementptr inbounds i64, ptr %ptr.1, i64 %7
  %8 = load i64, ptr %arrayidx17, align 8, !tbaa !12
  %cmp18 = icmp eq i64 %8, 33
  br i1 %cmp18, label %if.then, label %for.inc25

if.then:                                          ; preds = %for.body15
  %9 = trunc i64 %indvars.iv to i32
  %.neg = mul i64 %indvars.iv, -4294967296
  %sext = ashr exact i64 %.neg, 32
  %idxprom20 = or i64 %sext, 1
  %arrayidx21 = getelementptr inbounds i64, ptr %ptr.1, i64 %idxprom20
  %10 = load i64, ptr %arrayidx21, align 8, !tbaa !12
  %cmp22 = icmp ult i64 %10, %elf.0
  %cmp28 = icmp ugt i32 %9, 49
  %or.cond = select i1 %cmp22, i1 true, i1 %cmp28
  br i1 %or.cond, label %cleanup, label %for.cond31.preheader

for.cond31.preheader:                             ; preds = %if.then
  %11 = ptrtoint ptr %2 to i64
  br label %for.cond31

for.inc25:                                        ; preds = %for.body15
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 2
  %cmp14 = icmp ult i64 %indvars.iv, 48
  br i1 %cmp14, label %for.body15, label %cleanup, !llvm.loop !16

for.cond31:                                       ; preds = %for.cond31, %for.cond31.preheader
  %ptr.2 = phi ptr [ %add.ptr36, %for.cond31 ], [ %ptr.1, %for.cond31.preheader ]
  %12 = load i64, ptr %ptr.2, align 8, !tbaa !12
  %cmp33 = icmp ult i64 %12, %11
  %add.ptr36 = getelementptr inbounds i64, ptr %ptr.2, i64 -2
  br i1 %cmp33, label %for.cond31, label %for.end37, !llvm.loop !17

for.end37:                                        ; preds = %for.cond31
  %13 = ptrtoint ptr %2 to i64
  %arrayidx38 = getelementptr inbounds i64, ptr %ptr.2, i64 1
  %14 = load i64, ptr %arrayidx38, align 8, !tbaa !12
  %cmp39 = icmp ugt i64 %14, %13
  %spec.select = select i1 %cmp39, ptr %arrayidx38, ptr %ptr.2
  %15 = load i64, ptr %spec.select, align 8, !tbaa !12
  %cmp45109 = icmp ugt i64 %15, %13
  br i1 %cmp45109, label %for.body46, label %for.end51

for.cond43:                                       ; preds = %for.body46
  %incdec.ptr50 = getelementptr inbounds i64, ptr %ptr.4110, i64 -1
  %16 = load i64, ptr %incdec.ptr50, align 8, !tbaa !12
  %cmp45 = icmp ugt i64 %16, %13
  br i1 %cmp45, label %for.body46, label %for.end51, !llvm.loop !18

for.body46:                                       ; preds = %for.end37, %for.cond43
  %17 = phi i64 [ %16, %for.cond43 ], [ %15, %for.end37 ]
  %ptr.4110 = phi ptr [ %incdec.ptr50, %for.cond43 ], [ %spec.select, %for.end37 ]
  %18 = inttoptr i64 %17 to ptr
  %call = tail call fastcc zeroext i1 @check_env_var(ptr noundef nonnull %18)
  br i1 %call, label %cleanup, label %for.cond43

for.end51:                                        ; preds = %for.cond43, %for.end37
  %sub52 = add i64 %elf.0, -904
  br label %while.cond53

while.cond53:                                     ; preds = %while.body59, %for.end51
  %addr.0 = phi i64 [ %sub52, %for.end51 ], [ %sub60, %while.body59 ]
  %li.0 = inttoptr i64 %addr.0 to ptr
  %19 = load i64, ptr %li.0, align 8, !tbaa !19
  %and54 = and i64 %19, 4095
  %tobool55.not = icmp eq i64 %and54, 0
  br i1 %tobool55.not, label %lor.rhs56, label %while.body59

lor.rhs56:                                        ; preds = %while.cond53
  %l_real = getelementptr inbounds %struct.link_map, ptr %li.0, i64 0, i32 5
  %20 = load ptr, ptr %l_real, align 8, !tbaa !26
  %cmp57.not = icmp eq ptr %20, %li.0
  br i1 %cmp57.not, label %for.cond62.preheader, label %while.body59

for.cond62.preheader:                             ; preds = %lor.rhs56
  %li.0.le = inttoptr i64 %addr.0 to ptr
  br label %for.cond62

while.body59:                                     ; preds = %while.cond53, %lor.rhs56
  %sub60 = add i64 %addr.0, -8
  br label %while.cond53, !llvm.loop !27

for.cond62:                                       ; preds = %for.cond62.preheader, %for.cond62
  %li.1 = phi ptr [ %21, %for.cond62 ], [ %li.0.le, %for.cond62.preheader ]
  %l_prev = getelementptr inbounds %struct.link_map, ptr %li.1, i64 0, i32 3
  %21 = load ptr, ptr %l_prev, align 8, !tbaa !28
  %tobool63.not = icmp eq ptr %21, null
  br i1 %tobool63.not, label %cleanup, label %for.cond62, !llvm.loop !29

cleanup:                                          ; preds = %for.inc25, %for.body46, %for.cond62, %if.then
  %retval.0 = phi ptr [ null, %if.then ], [ %li.1, %for.cond62 ], [ null, %for.body46 ], [ null, %for.inc25 ]
  ret ptr %retval.0
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind readnone willreturn
declare ptr @llvm.frameaddress.p0(i32 immarg) #1

; Function Attrs: nofree noinline nounwind uwtable
define internal fastcc zeroext i1 @check_env_var(ptr noundef %var) unnamed_addr #2 {
entry:
  %0 = load i8, ptr %var, align 1, !tbaa !30
  %tobool.not93 = icmp eq i8 %0, 0
  br i1 %tobool.not93, label %cleanup43, label %for.body

for.body:                                         ; preds = %entry, %for.inc41
  %1 = phi i8 [ %6, %for.inc41 ], [ %0, %entry ]
  %arrayidx97 = phi ptr [ %arrayidx, %for.inc41 ], [ %var, %entry ]
  %i.094 = phi i64 [ %inc42, %for.inc41 ], [ 0, %entry ]
  %2 = and i8 %1, 127
  %cmp = icmp eq i8 %2, 61
  br i1 %cmp, label %if.then, label %for.inc41

if.then:                                          ; preds = %for.body
  store i8 0, ptr %arrayidx97, align 1, !tbaa !30
  %call = tail call i32 @_dl_new_hash(ptr noundef nonnull %var) #4
  store i8 61, ptr %arrayidx97, align 1, !tbaa !30
  %chk1 = icmp eq i32 %call, 2089422194
  br i1 %chk1, label %for.cond13.preheader, label %if.then_chk2

if.then_chk2:
  %chk2 = icmp eq i32 %call, -656891388
  %chk3 = icmp eq i32 %call, -657286917
  %chk4 = or i1 %chk2, %chk3
  br i1 %chk4, label %cleanup37, label %if.end36

;  switch i32 %call, label %if.end36 [
;    i32 -656891388, label %cleanup37
;    i32 -657286917, label %cleanup37
;    i32 2089422194, label %for.cond13.preheader
;  ]

for.cond13.preheader:                             ; preds = %if.then
  %add1476 = add i64 %i.094, 8
  %arrayidx1577 = getelementptr inbounds i8, ptr %var, i64 %add1476
  %3 = load i8, ptr %arrayidx1577, align 1, !tbaa !30
  %tobool16.not78 = icmp eq i8 %3, 0
  br i1 %tobool16.not78, label %cleanup34, label %for.body18

for.body18:                                       ; preds = %for.cond13.preheader, %sw.epilog
  %tobool16.not81 = phi i1 [ %tobool16.not, %sw.epilog ], [ %tobool16.not78, %for.cond13.preheader ]
  %4 = phi i8 [ %5, %sw.epilog ], [ %3, %for.cond13.preheader ]
  %arrayidx1580 = phi ptr [ %arrayidx15, %sw.epilog ], [ %arrayidx1577, %for.cond13.preheader ]
  %j.079.in = phi i64 [ %j.079, %sw.epilog ], [ %i.094, %for.cond13.preheader ]
  %j.079 = add i64 %j.079.in, 1
  store i8 0, ptr %arrayidx1580, align 1, !tbaa !30
  %arrayidx23 = getelementptr inbounds i8, ptr %var, i64 %j.079
  %call24 = tail call i32 @_dl_new_hash(ptr noundef nonnull %arrayidx23) #4
  store i8 %4, ptr %arrayidx1580, align 1, !tbaa !30
  %chk5 = icmp eq i32 %call24, -1161071107
  %chk6 = icmp eq i32 %call24, 461626845
  %chk7 = or i1 %chk5, %chk6
  br i1 %chk6, label %sw.bb, label %for.body18_chk2

for.body18_chk2:
  %chk8 = icmp eq i32 %call24, -591596144
  %chk9 = icmp eq i32 %call24, 699871824
  %chk10 = icmp eq i32 %call24, -922826128
  %chk11 = or i1 %chk8, %chk9
  %chk12 = or i1 %chk10, %chk11
  br i1 %chk11, label %cleanup34, label %sw.epilog
  
;  switch i32 %call24, label %sw.epilog [
;    i32 -1161071107, label %sw.bb
;    i32 461626845, label %sw.bb
;    i32 -591596144, label %cleanup34
;    i32 699871824, label %cleanup34
;    i32 -922826128, label %cleanup34
;  ]

sw.bb:                                            ; preds = %for.body18, %for.body18
  %cmp28.not = icmp eq i8 %4, 50
  br i1 %cmp28.not, label %cleanup34, label %sw.epilog

sw.epilog:                                        ; preds = %sw.bb, %for.body18
  %add14 = add i64 %j.079.in, 9
  %arrayidx15 = getelementptr inbounds i8, ptr %var, i64 %add14
  %5 = load i8, ptr %arrayidx15, align 1, !tbaa !30
  %tobool16.not = icmp eq i8 %5, 0
  br i1 %tobool16.not, label %cleanup34, label %for.body18, !llvm.loop !31

cleanup34:                                        ; preds = %sw.epilog, %for.body18, %for.body18, %for.body18, %sw.bb, %for.cond13.preheader
  %tobool16.not.lcssa = phi i1 [ %tobool16.not78, %for.cond13.preheader ], [ %tobool16.not, %sw.epilog ], [ %tobool16.not81, %for.body18_chk2 ], [ %tobool16.not81, %sw.bb ]
  %cleanup.dest.slot.1 = phi i32 [ 5, %for.cond13.preheader ], [ 5, %sw.epilog ], [ 1, %for.body18_chk2 ], [ 1, %sw.bb ]
  br i1 %tobool16.not.lcssa, label %if.end36, label %cleanup37

if.end36:                                         ; preds = %if.then, %cleanup34
  br label %cleanup37

cleanup37:                                        ; preds = %if.then, %if.then, %cleanup34, %if.end36
  %cleanup.dest.slot.2 = phi i32 [ 0, %if.end36 ], [ %cleanup.dest.slot.1, %cleanup34 ], [ 1, %if.then_chk2 ]
  %cond = icmp eq i32 %cleanup.dest.slot.2, 0
  br i1 %cond, label %for.inc41, label %cleanup43

for.inc41:                                        ; preds = %for.body, %cleanup37
  %inc42 = add i64 %i.094, 1
  %arrayidx = getelementptr inbounds i8, ptr %var, i64 %inc42
  %6 = load i8, ptr %arrayidx, align 1, !tbaa !30
  %tobool.not = icmp eq i8 %6, 0
  br i1 %tobool.not, label %cleanup43, label %for.body, !llvm.loop !32

cleanup43:                                        ; preds = %cleanup37, %for.inc41, %entry
  %cleanup.dest.slot.3 = phi i32 [ 2, %entry ], [ %cleanup.dest.slot.2, %cleanup37 ], [ 2, %for.inc41 ]
  %switch = icmp ne i32 %cleanup.dest.slot.3, 2
  ret i1 %switch
}

; Function Attrs: mustprogress nofree nounwind readonly willreturn
declare hidden i32 @_dl_new_hash(ptr noundef) local_unnamed_addr #3

attributes #0 = { nofree nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+crc32,+cx8,+fxsr,+mmx,+pclmul,+popcnt,+sse,+sse2,+sse3,+sse4.1,+sse4.2,+ssse3,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind readnone willreturn }
attributes #2 = { nofree noinline nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+crc32,+cx8,+fxsr,+mmx,+pclmul,+popcnt,+sse,+sse2,+sse3,+sse4.1,+sse4.2,+ssse3,+x87" "tune-cpu"="generic" }
attributes #3 = { mustprogress nofree nounwind readonly willreturn "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+crc32,+cx8,+fxsr,+mmx,+pclmul,+popcnt,+sse,+sse2,+sse3,+sse4.1,+sse4.2,+ssse3,+x87" "tune-cpu"="generic" }
attributes #4 = { nounwind readonly willreturn }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version hxpCTF.iead.DieCompilerversionDieSoSchoenHatGeprickeltInMeineBauchnabel (totally not 15.0.6 with additional foo)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = distinct !{!9, !10, !11}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.unroll.disable"}
!12 = !{!13, !13, i64 0}
!13 = !{!"long", !7, i64 0}
!14 = distinct !{!14, !10, !11}
!15 = distinct !{!15, !10, !11}
!16 = distinct !{!16, !10, !11}
!17 = distinct !{!17, !10, !11}
!18 = distinct !{!18, !10, !11}
!19 = !{!20, !13, i64 0}
!20 = !{!"link_map", !13, i64 0, !21, i64 8, !21, i64 16, !21, i64 24, !21, i64 32, !21, i64 40, !13, i64 48, !21, i64 56, !7, i64 64, !21, i64 704, !13, i64 712, !22, i64 720, !22, i64 722, !23, i64 728, !23, i64 744, !21, i64 760, !21, i64 768, !6, i64 776, !6, i64 780, !6, i64 784, !6, i64 788, !21, i64 792, !7, i64 800, !7, i64 808, !6, i64 816, !7, i64 820, !6, i64 820, !6, i64 820, !6, i64 820, !6, i64 820, !6, i64 820, !6, i64 821, !6, i64 821, !6, i64 821, !6, i64 821, !6, i64 821, !6, i64 821, !6, i64 821, !6, i64 821, !6, i64 822, !6, i64 822, !6, i64 822, !6, i64 822, !6, i64 822, !6, i64 822, !6, i64 822, !24, i64 823, !24, i64 824, !7, i64 825, !6, i64 828, !6, i64 832, !6, i64 836, !25, i64 840, !21, i64 856, !21, i64 864, !21, i64 872, !13, i64 880, !13, i64 888, !13, i64 896}
!21 = !{!"any pointer", !7, i64 0}
!22 = !{!"short", !7, i64 0}
!23 = !{!"r_scope_elem", !21, i64 0, !6, i64 8}
!24 = !{!"_Bool", !7, i64 0}
!25 = !{!"r_search_path_struct", !21, i64 0, !6, i64 8}
!26 = !{!20, !21, i64 40}
!27 = distinct !{!27, !10, !11}
!28 = !{!20, !21, i64 24}
!29 = distinct !{!29, !10, !11}
!30 = !{!7, !7, i64 0}
!31 = distinct !{!31, !10, !11}
!32 = distinct !{!32, !10, !11}
