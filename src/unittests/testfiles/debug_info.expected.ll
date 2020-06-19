; ModuleID = 'src/unittests/testfiles/debug_info.wip'
source_filename = "src/unittests/testfiles/debug_info.wip"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone
define i32 @noParams() #0 !dbg !3 {
entry:
  ret i32 0, !dbg !7
}

; Function Attrs: noinline nounwind optnone
define i16 @param(i8 %param1) #0 !dbg !8 {
entry:
  %param11 = alloca i8
  store i8 %param1, i8* %param11
  call void @llvm.dbg.declare(metadata i8* %param11, metadata !14, metadata !DIExpression()), !dbg !15
  %param12 = load i8, i8* %param11, !dbg !16
  %zeroext = zext i8 %param12 to i16, !dbg !16
  ret i16 %zeroext, !dbg !16
}

; Function Attrs: noinline nounwind optnone
define i64 @operators(i32 %x, i64 %y, i8 %z) #0 !dbg !17 {
entry:
  %z3 = alloca i8
  %y2 = alloca i64
  %x1 = alloca i32
  store i32 %x, i32* %x1
  call void @llvm.dbg.declare(metadata i32* %x1, metadata !23, metadata !DIExpression()), !dbg !26
  store i64 %y, i64* %y2
  call void @llvm.dbg.declare(metadata i64* %y2, metadata !24, metadata !DIExpression()), !dbg !26
  store i8 %z, i8* %z3
  call void @llvm.dbg.declare(metadata i8* %z3, metadata !25, metadata !DIExpression()), !dbg !26
  %x4 = load i32, i32* %x1, !dbg !27
  %neg = sub i32 0, %x4, !dbg !28
  %y5 = load i64, i64* %y2, !dbg !29
  %x6 = load i32, i32* %x1, !dbg !30
  %z7 = load i8, i8* %z3, !dbg !31
  %signext = sext i8 %z7 to i32, !dbg !32
  %mul = mul i32 %x6, %signext, !dbg !32
  %signext8 = sext i32 %mul to i64, !dbg !33
  %div = sdiv i64 %y5, %signext8, !dbg !33
  %signext9 = sext i32 %neg to i64, !dbg !34
  %add = add i64 %signext9, %div, !dbg !34
  ret i64 %add, !dbg !34
}

; Function Attrs: noinline nounwind optnone
define i32 @whileLoop(i32 %a, i32 %b) #0 !dbg !35 {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  call void @llvm.dbg.declare(metadata i32* %a1, metadata !39, metadata !DIExpression()), !dbg !41
  store i32 %b, i32* %b2
  call void @llvm.dbg.declare(metadata i32* %b2, metadata !40, metadata !DIExpression()), !dbg !41
  br label %whileCond

whileCond:                                        ; preds = %whileBody, %entry
  %a3 = load i32, i32* %a1, !dbg !42
  %cmpgt = icmp sgt i32 %a3, 0, !dbg !43
  br i1 %cmpgt, label %whileBody, label %whileExit, !dbg !43

whileBody:                                        ; preds = %whileCond
  %load = load i32, i32* %b2, !dbg !44
  %mul = mul i32 %load, 3, !dbg !44
  store i32 %mul, i32* %b2, !dbg !44
  %load4 = load i32, i32* %a1, !dbg !45
  %sub = sub i32 %load4, 1, !dbg !45
  store i32 %sub, i32* %a1, !dbg !45
  br label %whileCond, !dbg !45

whileExit:                                        ; preds = %whileCond
  %b5 = load i32, i32* %b2, !dbg !46
  ret i32 %b5, !dbg !46
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone }
attributes #1 = { nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "WIP Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "debug_info.wip", directory: "src/unittests/testfiles")
!2 = !{}
!3 = distinct !DISubprogram(name: "noParams", scope: !1, file: !1, line: 1, type: !4, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!4 = !DISubroutineType(flags: DIFlagPrototyped, types: !5)
!5 = !{!6}
!6 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!7 = !DILocation(line: 3, column: 5, scope: !3)
!8 = distinct !DISubprogram(name: "param", scope: !1, file: !1, line: 6, type: !9, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !13)
!9 = !DISubroutineType(flags: DIFlagPrototyped, types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "u16", size: 16, encoding: DW_ATE_unsigned)
!12 = !DIBasicType(name: "u8", size: 8, encoding: DW_ATE_unsigned)
!13 = !{!14}
!14 = !DILocalVariable(name: "param1", scope: !8, file: !1, line: 6, type: !12)
!15 = !DILocation(line: 6, scope: !8)
!16 = !DILocation(line: 8, column: 5, scope: !8)
!17 = distinct !DISubprogram(name: "operators", scope: !1, file: !1, line: 11, type: !18, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !22)
!18 = !DISubroutineType(flags: DIFlagPrototyped, types: !19)
!19 = !{!20, !6, !20, !21}
!20 = !DIBasicType(name: "i64", size: 64, encoding: DW_ATE_signed)
!21 = !DIBasicType(name: "i8", size: 8, encoding: DW_ATE_signed)
!22 = !{!23, !24, !25}
!23 = !DILocalVariable(name: "x", scope: !17, file: !1, line: 11, type: !6)
!24 = !DILocalVariable(name: "y", arg: 1, scope: !17, file: !1, line: 11, type: !20)
!25 = !DILocalVariable(name: "z", arg: 2, scope: !17, file: !1, line: 11, type: !21)
!26 = !DILocation(line: 11, scope: !17)
!27 = !DILocation(line: 13, column: 6, scope: !17)
!28 = !DILocation(line: 13, column: 5, scope: !17)
!29 = !DILocation(line: 13, column: 10, scope: !17)
!30 = !DILocation(line: 13, column: 15, scope: !17)
!31 = !DILocation(line: 13, column: 19, scope: !17)
!32 = !DILocation(line: 13, column: 17, scope: !17)
!33 = !DILocation(line: 13, column: 12, scope: !17)
!34 = !DILocation(line: 13, column: 8, scope: !17)
!35 = distinct !DISubprogram(name: "whileLoop", scope: !1, file: !1, line: 16, type: !36, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !38)
!36 = !DISubroutineType(flags: DIFlagPrototyped, types: !37)
!37 = !{!6, !6, !6}
!38 = !{!39, !40}
!39 = !DILocalVariable(name: "a", scope: !35, file: !1, line: 16, type: !6)
!40 = !DILocalVariable(name: "b", arg: 1, scope: !35, file: !1, line: 16, type: !6)
!41 = !DILocation(line: 16, scope: !35)
!42 = !DILocation(line: 18, column: 11, scope: !35)
!43 = !DILocation(line: 18, column: 13, scope: !35)
!44 = !DILocation(line: 20, column: 11, scope: !35)
!45 = !DILocation(line: 21, column: 11, scope: !35)
!46 = !DILocation(line: 24, column: 5, scope: !35)
