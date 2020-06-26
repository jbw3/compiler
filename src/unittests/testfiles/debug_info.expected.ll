; ModuleID = 'src/unittests/testfiles/debug_info.wip'
source_filename = "src/unittests/testfiles/debug_info.wip"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%UnitType = type {}
%str = type { i64, [0 x i8] }
%TestStruct = type { i32, i64, i1 }

@strStruct0 = constant { i64, [3 x i8] } { i64 3, [3 x i8] c"abc" }

; Function Attrs: noinline nounwind optnone
define i32 @noParams() #0 !dbg !3 {
entry:
  ret i32 0, !dbg !7
}

; Function Attrs: noinline nounwind optnone
define i16 @param(i8 %param1) #0 !dbg !9 {
entry:
  %param11 = alloca i8
  store i8 %param1, i8* %param11
  call void @llvm.dbg.declare(metadata i8* %param11, metadata !15, metadata !DIExpression()), !dbg !16
  %param12 = load i8, i8* %param11, !dbg !17
  %zeroext = zext i8 %param12 to i16, !dbg !17
  ret i16 %zeroext, !dbg !17
}

; Function Attrs: noinline nounwind optnone
define i64 @operators(i32 %x, i64 %y, i8 %z) #0 !dbg !19 {
entry:
  %z3 = alloca i8
  %y2 = alloca i64
  %x1 = alloca i32
  store i32 %x, i32* %x1
  call void @llvm.dbg.declare(metadata i32* %x1, metadata !25, metadata !DIExpression()), !dbg !28
  store i64 %y, i64* %y2
  call void @llvm.dbg.declare(metadata i64* %y2, metadata !26, metadata !DIExpression()), !dbg !28
  store i8 %z, i8* %z3
  call void @llvm.dbg.declare(metadata i8* %z3, metadata !27, metadata !DIExpression()), !dbg !28
  %x4 = load i32, i32* %x1, !dbg !29
  %neg = sub i32 0, %x4, !dbg !31
  %y5 = load i64, i64* %y2, !dbg !32
  %x6 = load i32, i32* %x1, !dbg !33
  %z7 = load i8, i8* %z3, !dbg !34
  %signext = sext i8 %z7 to i32, !dbg !35
  %mul = mul i32 %x6, %signext, !dbg !35
  %signext8 = sext i32 %mul to i64, !dbg !36
  %div = sdiv i64 %y5, %signext8, !dbg !36
  %signext9 = sext i32 %neg to i64, !dbg !37
  %add = add i64 %signext9, %div, !dbg !37
  ret i64 %add, !dbg !37
}

; Function Attrs: noinline nounwind optnone
define i32 @whileLoop(i32 %a, i32 %b) #0 !dbg !38 {
entry:
  %b2 = alloca i32
  %a1 = alloca i32
  store i32 %a, i32* %a1
  call void @llvm.dbg.declare(metadata i32* %a1, metadata !42, metadata !DIExpression()), !dbg !44
  store i32 %b, i32* %b2
  call void @llvm.dbg.declare(metadata i32* %b2, metadata !43, metadata !DIExpression()), !dbg !44
  br label %whileCond

whileCond:                                        ; preds = %whileBody, %entry
  %a3 = load i32, i32* %a1, !dbg !45
  %cmpgt = icmp sgt i32 %a3, 0, !dbg !47
  br i1 %cmpgt, label %whileBody, label %whileExit, !dbg !47

whileBody:                                        ; preds = %whileCond
  %load = load i32, i32* %b2, !dbg !48
  %mul = mul i32 %load, 3, !dbg !48
  store i32 %mul, i32* %b2, !dbg !48
  %load4 = load i32, i32* %a1, !dbg !50
  %sub = sub i32 %load4, 1, !dbg !50
  store i32 %sub, i32* %a1, !dbg !50
  br label %whileCond, !dbg !50

whileExit:                                        ; preds = %whileCond
  %b5 = load i32, i32* %b2, !dbg !51
  ret i32 %b5, !dbg !51
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types() #0 !dbg !52 {
entry:
  %s = alloca %str*
  %x9 = alloca i64
  %x8 = alloca i64
  %x7 = alloca i32
  %x6 = alloca i16
  %x5 = alloca i8
  %x4 = alloca i64
  %x3 = alloca i64
  %x2 = alloca i32
  %x1 = alloca i16
  %x0 = alloca i8
  %b = alloca i1
  call void @llvm.dbg.declare(metadata i1* %b, metadata !57, metadata !DIExpression()), !dbg !80
  store i1 true, i1* %b, !dbg !81
  call void @llvm.dbg.declare(metadata i8* %x0, metadata !60, metadata !DIExpression()), !dbg !82
  store i8 0, i8* %x0, !dbg !83
  call void @llvm.dbg.declare(metadata i16* %x1, metadata !61, metadata !DIExpression()), !dbg !84
  store i16 1, i16* %x1, !dbg !85
  call void @llvm.dbg.declare(metadata i32* %x2, metadata !63, metadata !DIExpression()), !dbg !86
  store i32 2, i32* %x2, !dbg !87
  call void @llvm.dbg.declare(metadata i64* %x3, metadata !64, metadata !DIExpression()), !dbg !88
  store i64 3, i64* %x3, !dbg !89
  call void @llvm.dbg.declare(metadata i64* %x4, metadata !65, metadata !DIExpression()), !dbg !90
  store i64 4, i64* %x4, !dbg !91
  call void @llvm.dbg.declare(metadata i8* %x5, metadata !67, metadata !DIExpression()), !dbg !92
  store i8 5, i8* %x5, !dbg !93
  call void @llvm.dbg.declare(metadata i16* %x6, metadata !68, metadata !DIExpression()), !dbg !94
  store i16 6, i16* %x6, !dbg !95
  call void @llvm.dbg.declare(metadata i32* %x7, metadata !69, metadata !DIExpression()), !dbg !96
  store i32 7, i32* %x7, !dbg !97
  call void @llvm.dbg.declare(metadata i64* %x8, metadata !71, metadata !DIExpression()), !dbg !98
  store i64 8, i64* %x8, !dbg !99
  call void @llvm.dbg.declare(metadata i64* %x9, metadata !73, metadata !DIExpression()), !dbg !100
  store i64 9, i64* %x9, !dbg !101
  call void @llvm.dbg.declare(metadata %str** %s, metadata !75, metadata !DIExpression()), !dbg !102
  store %str* bitcast ({ i64, [3 x i8] }* @strStruct0 to %str*), %str** %s, !dbg !103
  ret %UnitType zeroinitializer, !dbg !103
}

; Function Attrs: noinline nounwind optnone
define %UnitType @structType(i64 %n) #0 !dbg !104 {
entry:
  %t = alloca %TestStruct
  %n1 = alloca i64
  store i64 %n, i64* %n1
  call void @llvm.dbg.declare(metadata i64* %n1, metadata !108, metadata !DIExpression()), !dbg !116
  call void @llvm.dbg.declare(metadata %TestStruct* %t, metadata !109, metadata !DIExpression()), !dbg !117
  %n2 = load i64, i64* %n1, !dbg !118
  %agg = insertvalue %TestStruct { i32 12, i64 undef, i1 undef }, i64 %n2, 1, !dbg !119
  %agg3 = insertvalue %TestStruct %agg, i1 true, 2, !dbg !120
  store %TestStruct %agg3, %TestStruct* %t, !dbg !121
  ret %UnitType zeroinitializer, !dbg !121
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
!7 = !DILocation(line: 3, column: 5, scope: !8)
!8 = distinct !DILexicalBlock(scope: !3, file: !1, line: 2, column: 1)
!9 = distinct !DISubprogram(name: "param", scope: !1, file: !1, line: 6, type: !10, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!10 = !DISubroutineType(flags: DIFlagPrototyped, types: !11)
!11 = !{!12, !13}
!12 = !DIBasicType(name: "u16", size: 16, encoding: DW_ATE_unsigned)
!13 = !DIBasicType(name: "u8", size: 8, encoding: DW_ATE_unsigned)
!14 = !{!15}
!15 = !DILocalVariable(name: "param1", scope: !9, file: !1, line: 6, type: !13)
!16 = !DILocation(line: 6, scope: !9)
!17 = !DILocation(line: 8, column: 5, scope: !18)
!18 = distinct !DILexicalBlock(scope: !9, file: !1, line: 7, column: 1)
!19 = distinct !DISubprogram(name: "operators", scope: !1, file: !1, line: 11, type: !20, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !24)
!20 = !DISubroutineType(flags: DIFlagPrototyped, types: !21)
!21 = !{!22, !6, !22, !23}
!22 = !DIBasicType(name: "i64", size: 64, encoding: DW_ATE_signed)
!23 = !DIBasicType(name: "i8", size: 8, encoding: DW_ATE_signed)
!24 = !{!25, !26, !27}
!25 = !DILocalVariable(name: "x", scope: !19, file: !1, line: 11, type: !6)
!26 = !DILocalVariable(name: "y", arg: 1, scope: !19, file: !1, line: 11, type: !22)
!27 = !DILocalVariable(name: "z", arg: 2, scope: !19, file: !1, line: 11, type: !23)
!28 = !DILocation(line: 11, scope: !19)
!29 = !DILocation(line: 13, column: 6, scope: !30)
!30 = distinct !DILexicalBlock(scope: !19, file: !1, line: 12, column: 1)
!31 = !DILocation(line: 13, column: 5, scope: !30)
!32 = !DILocation(line: 13, column: 10, scope: !30)
!33 = !DILocation(line: 13, column: 15, scope: !30)
!34 = !DILocation(line: 13, column: 19, scope: !30)
!35 = !DILocation(line: 13, column: 17, scope: !30)
!36 = !DILocation(line: 13, column: 12, scope: !30)
!37 = !DILocation(line: 13, column: 8, scope: !30)
!38 = distinct !DISubprogram(name: "whileLoop", scope: !1, file: !1, line: 16, type: !39, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !41)
!39 = !DISubroutineType(flags: DIFlagPrototyped, types: !40)
!40 = !{!6, !6, !6}
!41 = !{!42, !43}
!42 = !DILocalVariable(name: "a", scope: !38, file: !1, line: 16, type: !6)
!43 = !DILocalVariable(name: "b", arg: 1, scope: !38, file: !1, line: 16, type: !6)
!44 = !DILocation(line: 16, scope: !38)
!45 = !DILocation(line: 18, column: 11, scope: !46)
!46 = distinct !DILexicalBlock(scope: !38, file: !1, line: 17, column: 1)
!47 = !DILocation(line: 18, column: 13, scope: !46)
!48 = !DILocation(line: 20, column: 11, scope: !49)
!49 = distinct !DILexicalBlock(scope: !46, file: !1, line: 19, column: 5)
!50 = !DILocation(line: 21, column: 11, scope: !49)
!51 = !DILocation(line: 24, column: 5, scope: !46)
!52 = distinct !DISubprogram(name: "types", scope: !1, file: !1, line: 27, type: !53, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !56)
!53 = !DISubroutineType(flags: DIFlagPrototyped, types: !54)
!54 = !{!55}
!55 = !DIBasicType(name: "Unit", encoding: DW_ATE_unsigned)
!56 = !{!57, !60, !61, !63, !64, !65, !67, !68, !69, !71, !73, !75}
!57 = !DILocalVariable(name: "b", scope: !58, file: !1, line: 29, type: !59)
!58 = distinct !DILexicalBlock(scope: !52, file: !1, line: 28, column: 1)
!59 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!60 = !DILocalVariable(name: "x0", scope: !58, file: !1, line: 30, type: !23)
!61 = !DILocalVariable(name: "x1", scope: !58, file: !1, line: 31, type: !62)
!62 = !DIBasicType(name: "i16", size: 16, encoding: DW_ATE_signed)
!63 = !DILocalVariable(name: "x2", scope: !58, file: !1, line: 32, type: !6)
!64 = !DILocalVariable(name: "x3", scope: !58, file: !1, line: 33, type: !22)
!65 = !DILocalVariable(name: "x4", scope: !58, file: !1, line: 34, type: !66)
!66 = !DIBasicType(name: "isize", size: 64, encoding: DW_ATE_signed)
!67 = !DILocalVariable(name: "x5", scope: !58, file: !1, line: 35, type: !13)
!68 = !DILocalVariable(name: "x6", scope: !58, file: !1, line: 36, type: !12)
!69 = !DILocalVariable(name: "x7", scope: !58, file: !1, line: 37, type: !70)
!70 = !DIBasicType(name: "u32", size: 32, encoding: DW_ATE_unsigned)
!71 = !DILocalVariable(name: "x8", scope: !58, file: !1, line: 38, type: !72)
!72 = !DIBasicType(name: "u64", size: 64, encoding: DW_ATE_unsigned)
!73 = !DILocalVariable(name: "x9", scope: !58, file: !1, line: 39, type: !74)
!74 = !DIBasicType(name: "usize", size: 64, encoding: DW_ATE_unsigned)
!75 = !DILocalVariable(name: "s", scope: !58, file: !1, line: 40, type: !76)
!76 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "str", baseType: !77, size: 64)
!77 = !DICompositeType(tag: DW_TAG_structure_type, name: "str_data", size: 64, elements: !78)
!78 = !{!79}
!79 = !DIDerivedType(tag: DW_TAG_member, name: "Size", baseType: !74, size: 64, align: 4)
!80 = !DILocation(line: 29, column: 9, scope: !58)
!81 = !DILocation(line: 29, column: 18, scope: !58)
!82 = !DILocation(line: 30, column: 9, scope: !58)
!83 = !DILocation(line: 30, column: 18, scope: !58)
!84 = !DILocation(line: 31, column: 9, scope: !58)
!85 = !DILocation(line: 31, column: 18, scope: !58)
!86 = !DILocation(line: 32, column: 9, scope: !58)
!87 = !DILocation(line: 32, column: 18, scope: !58)
!88 = !DILocation(line: 33, column: 9, scope: !58)
!89 = !DILocation(line: 33, column: 18, scope: !58)
!90 = !DILocation(line: 34, column: 9, scope: !58)
!91 = !DILocation(line: 34, column: 18, scope: !58)
!92 = !DILocation(line: 35, column: 9, scope: !58)
!93 = !DILocation(line: 35, column: 18, scope: !58)
!94 = !DILocation(line: 36, column: 9, scope: !58)
!95 = !DILocation(line: 36, column: 18, scope: !58)
!96 = !DILocation(line: 37, column: 9, scope: !58)
!97 = !DILocation(line: 37, column: 18, scope: !58)
!98 = !DILocation(line: 38, column: 9, scope: !58)
!99 = !DILocation(line: 38, column: 18, scope: !58)
!100 = !DILocation(line: 39, column: 9, scope: !58)
!101 = !DILocation(line: 39, column: 18, scope: !58)
!102 = !DILocation(line: 40, column: 9, scope: !58)
!103 = !DILocation(line: 40, column: 18, scope: !58)
!104 = distinct !DISubprogram(name: "structType", scope: !1, file: !1, line: 43, type: !105, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !107)
!105 = !DISubroutineType(flags: DIFlagPrototyped, types: !106)
!106 = !{!55, !22}
!107 = !{!108, !109}
!108 = !DILocalVariable(name: "n", scope: !104, file: !1, line: 43, type: !22)
!109 = !DILocalVariable(name: "t", scope: !110, file: !1, line: 45, type: !111)
!110 = distinct !DILexicalBlock(scope: !104, file: !1, line: 44, column: 1)
!111 = !DICompositeType(tag: DW_TAG_structure_type, name: "TestStruct", scope: !1, file: !1, line: 53, elements: !112)
!112 = !{!113, !114, !115}
!113 = !DIDerivedType(tag: DW_TAG_member, name: "num1", scope: !1, file: !1, line: 55, baseType: !6, size: 32, align: 32)
!114 = !DIDerivedType(tag: DW_TAG_member, name: "num2", scope: !1, file: !1, line: 56, baseType: !22, size: 64, align: 32, offset: 32)
!115 = !DIDerivedType(tag: DW_TAG_member, name: "flag", scope: !1, file: !1, line: 57, baseType: !59, size: 8, align: 8, offset: 96)
!116 = !DILocation(line: 43, scope: !104)
!117 = !DILocation(line: 45, column: 9, scope: !110)
!118 = !DILocation(line: 48, column: 15, scope: !110)
!119 = !DILocation(line: 48, column: 9, scope: !110)
!120 = !DILocation(line: 49, column: 9, scope: !110)
!121 = !DILocation(line: 45, column: 11, scope: !110)