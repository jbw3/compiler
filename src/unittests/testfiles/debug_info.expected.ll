; ModuleID = 'src/unittests/testfiles/debug_info.wip'
source_filename = "src/unittests/testfiles/debug_info.wip"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone
define i32 @noArgs() #0 !dbg !3 {
entry:
  ret i32 0, !dbg !7
}

attributes #0 = { noinline nounwind optnone }

!llvm.dbg.cu = !{!0}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "WIP Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "debug_info.wip", directory: "src/unittests/testfiles")
!2 = !{}
!3 = distinct !DISubprogram(name: "noArgs", scope: !1, file: !1, line: 1, type: !4, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!4 = !DISubroutineType(flags: DIFlagPrototyped, types: !5)
!5 = !{!6}
!6 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!7 = !DILocation(line: 3, column: 5, scope: !3)
