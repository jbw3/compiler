; ModuleID = 'src/unittests/testfiles/multi_file1.wip'
source_filename = "src/unittests/testfiles/multi_file1.wip"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone
define i32 @fun1() #0 !dbg !5 {
entry:
  %call = call i32 @fun2(), !dbg !9
  ret i32 %call, !dbg !9
}

; Function Attrs: noinline nounwind optnone
define i32 @fun3() #0 !dbg !11 {
entry:
  ret i32 123, !dbg !12
}

; Function Attrs: noinline nounwind optnone
define i32 @fun2() #0 !dbg !14 {
entry:
  %call = call i32 @fun3(), !dbg !15
  ret i32 %call, !dbg !15
}

attributes #0 = { noinline nounwind optnone }

!llvm.dbg.cu = !{!0, !2}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "WIP Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug)
!1 = !DIFile(filename: "src/unittests/testfiles/multi_file1.wip", directory: "/home/jwilkes/dev/compiler")
!2 = distinct !DICompileUnit(language: DW_LANG_C, file: !3, producer: "WIP Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4)
!3 = !DIFile(filename: "src/unittests/testfiles/multi_file2.wip", directory: "/home/jwilkes/dev/compiler")
!4 = !{}
!5 = distinct !DISubprogram(name: "fun1", scope: !1, file: !1, line: 1, type: !6, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !4)
!6 = !DISubroutineType(flags: DIFlagPrototyped, types: !7)
!7 = !{!8}
!8 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!9 = !DILocation(line: 3, column: 5, scope: !10)
!10 = distinct !DILexicalBlock(scope: !5, file: !1, line: 2, column: 1)
!11 = distinct !DISubprogram(name: "fun3", scope: !1, file: !1, line: 6, type: !6, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !4)
!12 = !DILocation(line: 8, column: 5, scope: !13)
!13 = distinct !DILexicalBlock(scope: !11, file: !1, line: 7, column: 1)
!14 = distinct !DISubprogram(name: "fun2", scope: !3, file: !3, line: 1, type: !6, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!15 = !DILocation(line: 3, column: 5, scope: !16)
!16 = distinct !DILexicalBlock(scope: !14, file: !3, line: 2, column: 1)
