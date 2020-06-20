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

; Function Attrs: noinline nounwind optnone
define %UnitType @types() #0 !dbg !47 {
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
  call void @llvm.dbg.declare(metadata i1* %b, metadata !52, metadata !DIExpression()), !dbg !74
  store i1 true, i1* %b, !dbg !75
  call void @llvm.dbg.declare(metadata i8* %x0, metadata !54, metadata !DIExpression()), !dbg !76
  store i8 0, i8* %x0, !dbg !77
  call void @llvm.dbg.declare(metadata i16* %x1, metadata !55, metadata !DIExpression()), !dbg !78
  store i16 1, i16* %x1, !dbg !79
  call void @llvm.dbg.declare(metadata i32* %x2, metadata !57, metadata !DIExpression()), !dbg !80
  store i32 2, i32* %x2, !dbg !81
  call void @llvm.dbg.declare(metadata i64* %x3, metadata !58, metadata !DIExpression()), !dbg !82
  store i64 3, i64* %x3, !dbg !83
  call void @llvm.dbg.declare(metadata i64* %x4, metadata !59, metadata !DIExpression()), !dbg !84
  store i64 4, i64* %x4, !dbg !85
  call void @llvm.dbg.declare(metadata i8* %x5, metadata !61, metadata !DIExpression()), !dbg !86
  store i8 5, i8* %x5, !dbg !87
  call void @llvm.dbg.declare(metadata i16* %x6, metadata !62, metadata !DIExpression()), !dbg !88
  store i16 6, i16* %x6, !dbg !89
  call void @llvm.dbg.declare(metadata i32* %x7, metadata !63, metadata !DIExpression()), !dbg !90
  store i32 7, i32* %x7, !dbg !91
  call void @llvm.dbg.declare(metadata i64* %x8, metadata !65, metadata !DIExpression()), !dbg !92
  store i64 8, i64* %x8, !dbg !93
  call void @llvm.dbg.declare(metadata i64* %x9, metadata !67, metadata !DIExpression()), !dbg !94
  store i64 9, i64* %x9, !dbg !95
  call void @llvm.dbg.declare(metadata %str** %s, metadata !69, metadata !DIExpression()), !dbg !96
  store %str* bitcast ({ i64, [3 x i8] }* @strStruct0 to %str*), %str** %s, !dbg !97
  ret %UnitType zeroinitializer, !dbg !97
}

; Function Attrs: noinline nounwind optnone
define %UnitType @structType(i64 %n) #0 !dbg !98 {
entry:
  %t = alloca %TestStruct
  %n1 = alloca i64
  store i64 %n, i64* %n1
  call void @llvm.dbg.declare(metadata i64* %n1, metadata !102, metadata !DIExpression()), !dbg !109
  call void @llvm.dbg.declare(metadata %TestStruct* %t, metadata !103, metadata !DIExpression()), !dbg !110
  %n2 = load i64, i64* %n1, !dbg !111
  %agg = insertvalue %TestStruct { i32 12, i64 undef, i1 undef }, i64 %n2, 1, !dbg !111
  %agg3 = insertvalue %TestStruct %agg, i1 true, 2, !dbg !112
  store %TestStruct %agg3, %TestStruct* %t, !dbg !113
  ret %UnitType zeroinitializer, !dbg !113
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
!47 = distinct !DISubprogram(name: "types", scope: !1, file: !1, line: 27, type: !48, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !51)
!48 = !DISubroutineType(flags: DIFlagPrototyped, types: !49)
!49 = !{!50}
!50 = !DIBasicType(name: "Unit", encoding: DW_ATE_unsigned)
!51 = !{!52, !54, !55, !57, !58, !59, !61, !62, !63, !65, !67, !69}
!52 = !DILocalVariable(name: "b", scope: !47, file: !1, line: 29, type: !53)
!53 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!54 = !DILocalVariable(name: "x0", scope: !47, file: !1, line: 30, type: !21)
!55 = !DILocalVariable(name: "x1", scope: !47, file: !1, line: 31, type: !56)
!56 = !DIBasicType(name: "i16", size: 16, encoding: DW_ATE_signed)
!57 = !DILocalVariable(name: "x2", scope: !47, file: !1, line: 32, type: !6)
!58 = !DILocalVariable(name: "x3", scope: !47, file: !1, line: 33, type: !20)
!59 = !DILocalVariable(name: "x4", scope: !47, file: !1, line: 34, type: !60)
!60 = !DIBasicType(name: "isize", size: 64, encoding: DW_ATE_signed)
!61 = !DILocalVariable(name: "x5", scope: !47, file: !1, line: 35, type: !12)
!62 = !DILocalVariable(name: "x6", scope: !47, file: !1, line: 36, type: !11)
!63 = !DILocalVariable(name: "x7", scope: !47, file: !1, line: 37, type: !64)
!64 = !DIBasicType(name: "u32", size: 32, encoding: DW_ATE_unsigned)
!65 = !DILocalVariable(name: "x8", scope: !47, file: !1, line: 38, type: !66)
!66 = !DIBasicType(name: "u64", size: 64, encoding: DW_ATE_unsigned)
!67 = !DILocalVariable(name: "x9", scope: !47, file: !1, line: 39, type: !68)
!68 = !DIBasicType(name: "usize", size: 64, encoding: DW_ATE_unsigned)
!69 = !DILocalVariable(name: "s", scope: !47, file: !1, line: 40, type: !70)
!70 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "str", baseType: !71, size: 64)
!71 = !DICompositeType(tag: DW_TAG_structure_type, name: "str_data", size: 64, elements: !72)
!72 = !{!73}
!73 = !DIDerivedType(tag: DW_TAG_member, name: "Size", baseType: !68, size: 64, align: 4)
!74 = !DILocation(line: 29, column: 9, scope: !47)
!75 = !DILocation(line: 29, column: 18, scope: !47)
!76 = !DILocation(line: 30, column: 9, scope: !47)
!77 = !DILocation(line: 30, column: 18, scope: !47)
!78 = !DILocation(line: 31, column: 9, scope: !47)
!79 = !DILocation(line: 31, column: 18, scope: !47)
!80 = !DILocation(line: 32, column: 9, scope: !47)
!81 = !DILocation(line: 32, column: 18, scope: !47)
!82 = !DILocation(line: 33, column: 9, scope: !47)
!83 = !DILocation(line: 33, column: 18, scope: !47)
!84 = !DILocation(line: 34, column: 9, scope: !47)
!85 = !DILocation(line: 34, column: 18, scope: !47)
!86 = !DILocation(line: 35, column: 9, scope: !47)
!87 = !DILocation(line: 35, column: 18, scope: !47)
!88 = !DILocation(line: 36, column: 9, scope: !47)
!89 = !DILocation(line: 36, column: 18, scope: !47)
!90 = !DILocation(line: 37, column: 9, scope: !47)
!91 = !DILocation(line: 37, column: 18, scope: !47)
!92 = !DILocation(line: 38, column: 9, scope: !47)
!93 = !DILocation(line: 38, column: 18, scope: !47)
!94 = !DILocation(line: 39, column: 9, scope: !47)
!95 = !DILocation(line: 39, column: 18, scope: !47)
!96 = !DILocation(line: 40, column: 9, scope: !47)
!97 = !DILocation(line: 40, column: 18, scope: !47)
!98 = distinct !DISubprogram(name: "structType", scope: !1, file: !1, line: 43, type: !99, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !101)
!99 = !DISubroutineType(flags: DIFlagPrototyped, types: !100)
!100 = !{!50, !20}
!101 = !{!102, !103}
!102 = !DILocalVariable(name: "n", scope: !98, file: !1, line: 43, type: !20)
!103 = !DILocalVariable(name: "t", scope: !98, file: !1, line: 45, type: !104)
!104 = !DICompositeType(tag: DW_TAG_structure_type, name: "TestStruct", scope: !1, file: !1, line: 53, elements: !105)
!105 = !{!106, !107, !108}
!106 = !DIDerivedType(tag: DW_TAG_member, name: "num1", scope: !1, file: !1, line: 55, baseType: !6, size: 32, align: 32)
!107 = !DIDerivedType(tag: DW_TAG_member, name: "num2", scope: !1, file: !1, line: 56, baseType: !20, size: 64, align: 32, offset: 32)
!108 = !DIDerivedType(tag: DW_TAG_member, name: "flag", scope: !1, file: !1, line: 57, baseType: !53, size: 8, align: 8, offset: 96)
!109 = !DILocation(line: 43, scope: !98)
!110 = !DILocation(line: 45, column: 9, scope: !98)
!111 = !DILocation(line: 48, column: 15, scope: !98)
!112 = !DILocation(line: 49, column: 15, scope: !98)
!113 = !DILocation(line: 45, column: 11, scope: !98)
