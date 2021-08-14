$module_id
$source_filename
$target_datalayout
$target_triple

%S3 = type { %S2 }
%S2 = type { %S1, %str }
%S1 = type { i32, i32 }
%str = type { i64, i8* }

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
define i32 @fun4(%S3 %s) #0 !dbg !12 {
entry:
  %s1 = alloca %S3, align 8
  store %S3 %s, %S3* %s1, align 8
  call void @llvm.dbg.declare(metadata %S3* %s1, metadata !35, metadata !DIExpression()), !dbg !36
  %s2 = load %S3, %S3* %s1, align 8, !dbg !37
  %mber = extractvalue %S3 %s2, 0, !dbg !39
  %mber3 = extractvalue %S2 %mber, 0, !dbg !40
  %mber4 = extractvalue %S1 %mber3, 0, !dbg !41
  %s5 = load %S3, %S3* %s1, align 8, !dbg !42
  %mber6 = extractvalue %S3 %s5, 0, !dbg !43
  %mber7 = extractvalue %S2 %mber6, 0, !dbg !44
  %mber8 = extractvalue %S1 %mber7, 1, !dbg !45
  %add = add i32 %mber4, %mber8, !dbg !46
  ret i32 %add, !dbg !46
}

; Function Attrs: noinline nounwind optnone
define i32 @fun2() #0 !dbg !47 {
entry:
  %call = call i32 @fun3(), !dbg !48
  ret i32 %call, !dbg !48
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

attributes #0 = { noinline nounwind optnone }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }

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
!12 = distinct !DISubprogram(name: "fun4", scope: !1, file: !1, line: 11, type: !13, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !34)
!13 = !DISubroutineType(flags: DIFlagPrototyped, types: !14)
!14 = !{!6, !15}
!15 = !DICompositeType(tag: DW_TAG_structure_type, name: "S3", scope: !1, file: !1, line: 16, elements: !16)
!16 = !{!17}
!17 = !DIDerivedType(tag: DW_TAG_member, name: "s2", scope: !1, file: !1, line: 18, baseType: !18)
!18 = !DICompositeType(tag: DW_TAG_structure_type, name: "S2", scope: !19, file: !19, line: 6, elements: !20)
$filename2
!20 = !{!21, !26}
!21 = !DIDerivedType(tag: DW_TAG_member, name: "s1", scope: !19, file: !19, line: 8, baseType: !22)
!22 = !DICompositeType(tag: DW_TAG_structure_type, name: "S1", scope: !1, file: !1, line: 21, elements: !23)
!23 = !{!24, !25}
!24 = !DIDerivedType(tag: DW_TAG_member, name: "x", scope: !1, file: !1, line: 23, baseType: !6, size: 32, align: 32)
!25 = !DIDerivedType(tag: DW_TAG_member, name: "y", scope: !1, file: !1, line: 24, baseType: !6, size: 32, align: 32, offset: 32)
!26 = !DIDerivedType(tag: DW_TAG_member, name: "string", scope: !19, file: !19, line: 9, baseType: !27, size: 128, align: 32)
!27 = !DICompositeType(tag: DW_TAG_structure_type, name: "str", size: 128, elements: !28)
!28 = !{!29, !31}
!29 = !DIDerivedType(tag: DW_TAG_member, name: "Size", baseType: !30, size: 64, align: 4)
!30 = !DIBasicType(name: "usize", size: 64, encoding: DW_ATE_unsigned)
!31 = !DIDerivedType(tag: DW_TAG_member, name: "Data", baseType: !32, size: 64, align: 4, offset: 64)
!32 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&u8", baseType: !33, size: 64)
!33 = !DIBasicType(name: "u8", size: 8, encoding: DW_ATE_unsigned)
!34 = !{!35}
!35 = !DILocalVariable(name: "s", arg: 1, scope: !12, file: !1, line: 11, type: !15)
!36 = !DILocation(line: 11, scope: !12)
!37 = !DILocation(line: 13, column: 5, scope: !38)
!38 = distinct !DILexicalBlock(scope: !12, file: !1, line: 12, column: 1)
!39 = !DILocation(line: 13, column: 7, scope: !38)
!40 = !DILocation(line: 13, column: 10, scope: !38)
!41 = !DILocation(line: 13, column: 13, scope: !38)
!42 = !DILocation(line: 13, column: 17, scope: !38)
!43 = !DILocation(line: 13, column: 19, scope: !38)
!44 = !DILocation(line: 13, column: 22, scope: !38)
!45 = !DILocation(line: 13, column: 25, scope: !38)
!46 = !DILocation(line: 13, column: 15, scope: !38)
!47 = distinct !DISubprogram(name: "fun2", scope: !19, file: !19, line: 1, type: !4, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!48 = !DILocation(line: 3, column: 5, scope: !49)
!49 = distinct !DILexicalBlock(scope: !47, file: !19, line: 2, column: 1)
