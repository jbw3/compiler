$module_id
$source_filename
$target_datalayout
$target_triple

; Function Attrs: noinline nounwind optnone
define i32 @fun1() #0 !dbg !3 {
entry:
  %call = call i32 @fun2(), !dbg !7
  ret i32 %call, !dbg !7
}

; Function Attrs: noinline nounwind optnone
define i32 @fun3() #0 !dbg !9 {
entry:
  ret i32 123, !dbg !10
}

; Function Attrs: noinline nounwind optnone
define i32 @fun2() #0 !dbg !12 {
entry:
  %call = call i32 @fun3(), !dbg !14
  ret i32 %call, !dbg !14
}

attributes #0 = { noinline nounwind optnone }

!llvm.dbg.cu = !{!0}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "WIP Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
$filename1
!2 = !{}
!3 = distinct !DISubprogram(name: "fun1", scope: !1, file: !1, line: 1, type: !4, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!4 = !DISubroutineType(flags: DIFlagPrototyped, types: !5)
!5 = !{!6}
!6 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!7 = !DILocation(line: 3, column: 5, scope: !8)
!8 = distinct !DILexicalBlock(scope: !3, file: !1, line: 2, column: 1)
!9 = distinct !DISubprogram(name: "fun3", scope: !1, file: !1, line: 6, type: !4, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!10 = !DILocation(line: 8, column: 5, scope: !11)
!11 = distinct !DILexicalBlock(scope: !9, file: !1, line: 7, column: 1)
!12 = distinct !DISubprogram(name: "fun2", scope: !13, file: !13, line: 1, type: !4, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
$filename2
!14 = !DILocation(line: 3, column: 5, scope: !15)
!15 = distinct !DILexicalBlock(scope: !12, file: !13, line: 2, column: 1)
