$module_id
$source_filename
$target_datalayout
$target_triple

%S3 = type { %S2 }
%S2 = type { %S1, %str }
%S1 = type { i32, i32 }
%str = type { i64, ptr }

; Function Attrs: noinline nounwind optnone
define i32 @fun1() #0 !dbg !2 {
entry:
  %call = call i32 @fun2(), !dbg !6
  ret i32 %call, !dbg !6
}

; Function Attrs: noinline nounwind optnone
define i32 @fun3() #0 !dbg !8 {
entry:
  ret i32 123, !dbg !9
}

; Function Attrs: noinline nounwind optnone
define i32 @fun4(%S3 %s) #0 !dbg !11 {
entry:
  %s1 = alloca %S3, align 8
  store %S3 %s, ptr %s1, align 8
    #dbg_declare(ptr %s1, !34, !DIExpression(), !35)
  %s2 = load %S3, ptr %s1, align 8, !dbg !36
  %mber = extractvalue %S3 %s2, 0, !dbg !38
  %mber3 = extractvalue %S2 %mber, 0, !dbg !39
  %mber4 = extractvalue %S1 %mber3, 0, !dbg !40
  %s5 = load %S3, ptr %s1, align 8, !dbg !41
  %mber6 = extractvalue %S3 %s5, 0, !dbg !42
  %mber7 = extractvalue %S2 %mber6, 0, !dbg !43
  %mber8 = extractvalue %S1 %mber7, 1, !dbg !44
  %add = add i32 %mber4, %mber8, !dbg !45
  ret i32 %add, !dbg !45
}

; Function Attrs: noinline nounwind optnone
define i32 @fun2() #0 !dbg !46 {
entry:
  %call = call i32 @fun3(), !dbg !47
  ret i32 %call, !dbg !47
}

attributes #0 = { noinline nounwind optnone }

!llvm.dbg.cu = !{!0}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "WIP Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug)
$filename1
!2 = distinct !DISubprogram(name: "fun1", scope: !1, file: !1, line: 1, type: !3, spFlags: DISPFlagDefinition, unit: !0)
!3 = !DISubroutineType(flags: DIFlagPrototyped, types: !4)
!4 = !{!5}
!5 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!6 = !DILocation(line: 3, column: 5, scope: !7)
!7 = distinct !DILexicalBlock(scope: !2, file: !1, line: 2, column: 1)
!8 = distinct !DISubprogram(name: "fun3", scope: !1, file: !1, line: 6, type: !3, spFlags: DISPFlagDefinition, unit: !0)
!9 = !DILocation(line: 8, column: 5, scope: !10)
!10 = distinct !DILexicalBlock(scope: !8, file: !1, line: 7, column: 1)
!11 = distinct !DISubprogram(name: "fun4", scope: !1, file: !1, line: 11, type: !12, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !33)
!12 = !DISubroutineType(flags: DIFlagPrototyped, types: !13)
!13 = !{!5, !14}
!14 = !DICompositeType(tag: DW_TAG_structure_type, name: "S3", scope: !1, file: !1, line: 16, elements: !15)
!15 = !{!16}
!16 = !DIDerivedType(tag: DW_TAG_member, name: "s2", scope: !1, file: !1, line: 18, baseType: !17)
!17 = !DICompositeType(tag: DW_TAG_structure_type, name: "S2", scope: !18, file: !18, line: 6, elements: !19)
$filename2
!19 = !{!20, !25}
!20 = !DIDerivedType(tag: DW_TAG_member, name: "s1", scope: !18, file: !18, line: 8, baseType: !21)
!21 = !DICompositeType(tag: DW_TAG_structure_type, name: "S1", scope: !1, file: !1, line: 21, elements: !22)
!22 = !{!23, !24}
!23 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !1, file: !1, line: 23, baseType: !5, size: 32, align: 32)
!24 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !1, file: !1, line: 24, baseType: !5, size: 32, align: 32, offset: 32)
!25 = !DIDerivedType(tag: DW_TAG_member, name: "string", scope: !18, file: !18, line: 9, baseType: !26, size: 128, align: 64)
!26 = !DICompositeType(tag: DW_TAG_structure_type, name: "str", size: 128, elements: !27)
!27 = !{!28, !30}
!28 = !DIDerivedType(tag: DW_TAG_member, name: "Size", baseType: !29, size: 64, align: 4)
!29 = !DIBasicType(name: "usize", size: 64, encoding: DW_ATE_unsigned)
!30 = !DIDerivedType(tag: DW_TAG_member, name: "Data", baseType: !31, size: 64, align: 4, offset: 64)
!31 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&u8", baseType: !32, size: 64)
!32 = !DIBasicType(name: "u8", size: 8, encoding: DW_ATE_unsigned)
!33 = !{!34}
!34 = !DILocalVariable(name: "s", arg: 1, scope: !11, file: !1, line: 11, type: !14)
!35 = !DILocation(line: 11, scope: !11)
!36 = !DILocation(line: 13, column: 5, scope: !37)
!37 = distinct !DILexicalBlock(scope: !11, file: !1, line: 12, column: 1)
!38 = !DILocation(line: 13, column: 7, scope: !37)
!39 = !DILocation(line: 13, column: 10, scope: !37)
!40 = !DILocation(line: 13, column: 13, scope: !37)
!41 = !DILocation(line: 13, column: 17, scope: !37)
!42 = !DILocation(line: 13, column: 19, scope: !37)
!43 = !DILocation(line: 13, column: 22, scope: !37)
!44 = !DILocation(line: 13, column: 25, scope: !37)
!45 = !DILocation(line: 13, column: 15, scope: !37)
!46 = distinct !DISubprogram(name: "fun2", scope: !18, file: !18, line: 1, type: !3, spFlags: DISPFlagDefinition, unit: !0)
!47 = !DILocation(line: 3, column: 5, scope: !48)
!48 = distinct !DILexicalBlock(scope: !46, file: !18, line: 2, column: 1)
