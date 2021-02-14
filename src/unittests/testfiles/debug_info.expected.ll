$module_id
$source_filename
$target_datalayout
$target_triple

%str = type { i64, i8* }
%UnitType = type {}
%TestStruct = type { i32, i64, i1 }
%Range32 = type { i32, i32 }
%A = type { i32, %B }
%B = type { i32, %C }
%C = type { i32, %A* }
%"[u16]" = type { i64, i16* }
%"[u32]" = type { i64, i32* }

@strData0 = constant [3 x i8] c"abc"
@strStruct0 = constant %str { i64 3, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @strData0, i32 0, i32 0) }

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
  %s = alloca %str
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
  call void @llvm.dbg.declare(metadata i1* %b, metadata !57, metadata !DIExpression()), !dbg !81
  store i1 true, i1* %b, !dbg !82
  call void @llvm.dbg.declare(metadata i8* %x0, metadata !60, metadata !DIExpression()), !dbg !83
  store i8 0, i8* %x0, !dbg !84
  call void @llvm.dbg.declare(metadata i16* %x1, metadata !61, metadata !DIExpression()), !dbg !85
  store i16 1, i16* %x1, !dbg !86
  call void @llvm.dbg.declare(metadata i32* %x2, metadata !63, metadata !DIExpression()), !dbg !87
  store i32 2, i32* %x2, !dbg !88
  call void @llvm.dbg.declare(metadata i64* %x3, metadata !64, metadata !DIExpression()), !dbg !89
  store i64 3, i64* %x3, !dbg !90
  call void @llvm.dbg.declare(metadata i64* %x4, metadata !65, metadata !DIExpression()), !dbg !91
  store i64 4, i64* %x4, !dbg !92
  call void @llvm.dbg.declare(metadata i8* %x5, metadata !67, metadata !DIExpression()), !dbg !93
  store i8 5, i8* %x5, !dbg !94
  call void @llvm.dbg.declare(metadata i16* %x6, metadata !68, metadata !DIExpression()), !dbg !95
  store i16 6, i16* %x6, !dbg !96
  call void @llvm.dbg.declare(metadata i32* %x7, metadata !69, metadata !DIExpression()), !dbg !97
  store i32 7, i32* %x7, !dbg !98
  call void @llvm.dbg.declare(metadata i64* %x8, metadata !71, metadata !DIExpression()), !dbg !99
  store i64 8, i64* %x8, !dbg !100
  call void @llvm.dbg.declare(metadata i64* %x9, metadata !73, metadata !DIExpression()), !dbg !101
  store i64 9, i64* %x9, !dbg !102
  call void @llvm.dbg.declare(metadata %str* %s, metadata !75, metadata !DIExpression()), !dbg !103
  %load = load %str, %str* @strStruct0, !dbg !103
  store %str %load, %str* %s, !dbg !104
  ret %UnitType zeroinitializer, !dbg !104
}

; Function Attrs: noinline nounwind optnone
define %UnitType @structType(i64 %n) #0 !dbg !105 {
entry:
  %t = alloca %TestStruct
  %n1 = alloca i64
  store i64 %n, i64* %n1
  call void @llvm.dbg.declare(metadata i64* %n1, metadata !109, metadata !DIExpression()), !dbg !117
  call void @llvm.dbg.declare(metadata %TestStruct* %t, metadata !110, metadata !DIExpression()), !dbg !118
  %n2 = load i64, i64* %n1, !dbg !119
  %agg = insertvalue %TestStruct { i32 12, i64 undef, i1 undef }, i64 %n2, 1, !dbg !120
  %agg3 = insertvalue %TestStruct %agg, i1 true, 2, !dbg !121
  store %TestStruct %agg3, %TestStruct* %t, !dbg !122
  ret %UnitType zeroinitializer, !dbg !122
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ranges(i32 %n) #0 !dbg !123 {
entry:
  %r2 = alloca %Range32
  %r1 = alloca %Range32
  %n1 = alloca i32
  store i32 %n, i32* %n1
  call void @llvm.dbg.declare(metadata i32* %n1, metadata !127, metadata !DIExpression()), !dbg !136
  call void @llvm.dbg.declare(metadata %Range32* %r1, metadata !128, metadata !DIExpression()), !dbg !137
  %n2 = load i32, i32* %n1, !dbg !138
  %rng = insertvalue %Range32 { i32 0, i32 undef }, i32 %n2, 1, !dbg !139
  store %Range32 %rng, %Range32* %r1, !dbg !140
  call void @llvm.dbg.declare(metadata %Range32* %r2, metadata !134, metadata !DIExpression()), !dbg !141
  %n3 = load i32, i32* %n1, !dbg !142
  %neg = sub i32 0, %n3, !dbg !143
  %n4 = load i32, i32* %n1, !dbg !144
  %mul = mul i32 %n4, 3, !dbg !145
  %rng5 = insertvalue %Range32 undef, i32 %neg, 0, !dbg !146
  %rng6 = insertvalue %Range32 %rng5, i32 %mul, 1, !dbg !146
  store %Range32 %rng6, %Range32* %r2, !dbg !147
  ret %UnitType zeroinitializer, !dbg !147
}

; Function Attrs: noinline nounwind optnone
define i32 @forLoop(i32 %n) #0 !dbg !148 {
entry:
  %j = alloca i32
  %i = alloca i32
  %a = alloca i32
  %n1 = alloca i32
  store i32 %n, i32* %n1
  call void @llvm.dbg.declare(metadata i32* %n1, metadata !152, metadata !DIExpression()), !dbg !160
  call void @llvm.dbg.declare(metadata i32* %a, metadata !153, metadata !DIExpression()), !dbg !161
  store i32 0, i32* %a, !dbg !162
  %n2 = load i32, i32* %n1, !dbg !163
  %rng = insertvalue %Range32 { i32 0, i32 undef }, i32 %n2, 1, !dbg !164
  call void @llvm.dbg.declare(metadata i32* %i, metadata !155, metadata !DIExpression()), !dbg !165
  %start = extractvalue %Range32 %rng, 0, !dbg !164
  %end = extractvalue %Range32 %rng, 1, !dbg !164
  br label %forCond, !dbg !164

forCond:                                          ; preds = %forIter14, %entry
  %iter = phi i32 [ %start, %entry ], [ %inc15, %forIter14 ], !dbg !165
  %cmp = icmp sle i32 %iter, %end, !dbg !165
  br i1 %cmp, label %forBody, label %forExit16, !dbg !165

forBody:                                          ; preds = %forCond
  store i32 %iter, i32* %i, !dbg !165
  %n3 = load i32, i32* %n1, !dbg !166
  %rng4 = insertvalue %Range32 undef, i32 %n3, 0, !dbg !167
  %rng5 = insertvalue %Range32 %rng4, i32 10, 1, !dbg !167
  call void @llvm.dbg.declare(metadata i32* %j, metadata !157, metadata !DIExpression()), !dbg !168
  %start6 = extractvalue %Range32 %rng5, 0, !dbg !167
  %end7 = extractvalue %Range32 %rng5, 1, !dbg !167
  br label %forCond8, !dbg !167

forCond8:                                         ; preds = %forIter, %forBody
  %iter9 = phi i32 [ %start6, %forBody ], [ %inc, %forIter ], !dbg !168
  %cmp10 = icmp sle i32 %iter9, %end7, !dbg !168
  br i1 %cmp10, label %forBody11, label %forExit, !dbg !168

forBody11:                                        ; preds = %forCond8
  store i32 %iter9, i32* %j, !dbg !168
  %i12 = load i32, i32* %i, !dbg !169
  %j13 = load i32, i32* %j, !dbg !171
  %mul = mul i32 %i12, %j13, !dbg !172
  %load = load i32, i32* %a, !dbg !173
  %add = add i32 %load, %mul, !dbg !173
  store i32 %add, i32* %a, !dbg !173
  br label %forIter, !dbg !173

forIter:                                          ; preds = %forBody11
  %inc = add i32 %iter9, 1, !dbg !173
  br label %forCond8, !dbg !173

forExit:                                          ; preds = %forCond8
  br label %forIter14, !dbg !173

forIter14:                                        ; preds = %forExit
  %inc15 = add i32 %iter, 1, !dbg !173
  br label %forCond, !dbg !173

forExit16:                                        ; preds = %forCond
  %a17 = load i32, i32* %a, !dbg !174
  ret i32 %a17, !dbg !174
}

; Function Attrs: noinline nounwind optnone
define %UnitType @pointerStructTest(%A %a) #0 !dbg !175 {
entry:
  %a1 = alloca %A
  store %A %a, %A* %a1
  call void @llvm.dbg.declare(metadata %A* %a1, metadata !192, metadata !DIExpression()), !dbg !193
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @pointers(i32 %x) #0 !dbg !194 {
entry:
  %ptr = alloca i32*
  %x1 = alloca i32
  store i32 %x, i32* %x1
  call void @llvm.dbg.declare(metadata i32* %x1, metadata !196, metadata !DIExpression()), !dbg !200
  call void @llvm.dbg.declare(metadata i32** %ptr, metadata !197, metadata !DIExpression()), !dbg !201
  store i32* %x1, i32** %ptr, !dbg !202
  %ptr2 = load i32*, i32** %ptr, !dbg !203
  store i32 17, i32* %ptr2, !dbg !204
  ret %UnitType zeroinitializer, !dbg !204
}

; Function Attrs: noinline nounwind optnone
define %UnitType @break_and_continue() #0 !dbg !205 {
entry:
  %j = alloca i8
  %i = alloca i8
  call void @llvm.dbg.declare(metadata i8* %i, metadata !207, metadata !DIExpression()), !dbg !211
  br label %forCond, !dbg !212

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i8 [ 0, %entry ], [ %inc, %forIter ], !dbg !211
  %cmp = icmp sle i8 %iter, 7, !dbg !211
  br i1 %cmp, label %forBody, label %forExit, !dbg !211

forBody:                                          ; preds = %forCond
  store i8 %iter, i8* %i, !dbg !211
  %i1 = load i8, i8* %i, !dbg !213
  %cmpeq = icmp eq i8 %i1, 5, !dbg !215
  br i1 %cmpeq, label %if, label %else, !dbg !215

if:                                               ; preds = %forBody
  br label %forExit, !dbg !216

afterbreak:                                       ; No predecessors!
  br label %merge6, !dbg !216

else:                                             ; preds = %forBody
  %i2 = load i8, i8* %i, !dbg !218
  %cmpeq3 = icmp eq i8 %i2, 2, !dbg !219
  br i1 %cmpeq3, label %if4, label %else5, !dbg !219

if4:                                              ; preds = %else
  br label %forIter, !dbg !220

aftercontinue:                                    ; No predecessors!
  br label %merge, !dbg !220

else5:                                            ; preds = %else
  br label %merge, !dbg !220

merge:                                            ; preds = %else5, %aftercontinue
  %phi = phi %UnitType [ zeroinitializer, %aftercontinue ], [ zeroinitializer, %else5 ], !dbg !220
  br label %merge6, !dbg !220

merge6:                                           ; preds = %merge, %afterbreak
  %phi7 = phi %UnitType [ zeroinitializer, %afterbreak ], [ %phi, %merge ], !dbg !220
  %call = call i32 @noParams(), !dbg !222
  br label %forIter, !dbg !222

forIter:                                          ; preds = %merge6, %if4
  %inc = add i8 %iter, 1, !dbg !222
  br label %forCond, !dbg !222

forExit:                                          ; preds = %if, %forCond
  call void @llvm.dbg.declare(metadata i8* %j, metadata !210, metadata !DIExpression()), !dbg !223
  store i8 1, i8* %j, !dbg !224
  br label %whileCond, !dbg !224

whileCond:                                        ; preds = %merge20, %if11, %forExit
  %j8 = load i8, i8* %j, !dbg !225
  %cmplt = icmp slt i8 %j8, 100, !dbg !226
  br i1 %cmplt, label %whileBody, label %whileExit, !dbg !226

whileBody:                                        ; preds = %whileCond
  %j9 = load i8, i8* %j, !dbg !227
  %rem = srem i8 %j9, 7, !dbg !229
  %cmpeq10 = icmp eq i8 %rem, 0, !dbg !230
  br i1 %cmpeq10, label %if11, label %else13, !dbg !230

if11:                                             ; preds = %whileBody
  br label %whileCond, !dbg !231

aftercontinue12:                                  ; No predecessors!
  br label %merge14, !dbg !231

else13:                                           ; preds = %whileBody
  br label %merge14, !dbg !231

merge14:                                          ; preds = %else13, %aftercontinue12
  %phi15 = phi %UnitType [ zeroinitializer, %aftercontinue12 ], [ zeroinitializer, %else13 ], !dbg !231
  %j16 = load i8, i8* %j, !dbg !233
  %cmpgt = icmp sgt i8 %j16, 89, !dbg !234
  br i1 %cmpgt, label %if17, label %else19, !dbg !234

if17:                                             ; preds = %merge14
  br label %whileExit, !dbg !235

afterbreak18:                                     ; No predecessors!
  br label %merge20, !dbg !235

else19:                                           ; preds = %merge14
  br label %merge20, !dbg !235

merge20:                                          ; preds = %else19, %afterbreak18
  %phi21 = phi %UnitType [ zeroinitializer, %afterbreak18 ], [ zeroinitializer, %else19 ], !dbg !235
  %call22 = call i32 @noParams(), !dbg !237
  %load = load i8, i8* %j, !dbg !238
  %add = add i8 %load, 1, !dbg !238
  store i8 %add, i8* %j, !dbg !238
  br label %whileCond, !dbg !238

whileExit:                                        ; preds = %if17, %whileCond
  ret %UnitType zeroinitializer, !dbg !238
}

; Function Attrs: noinline nounwind optnone
define i32 @ret1(i16 %a, i16 %b) #0 !dbg !239 {
entry:
  %rv = alloca i16
  %b2 = alloca i16
  %a1 = alloca i16
  store i16 %a, i16* %a1
  call void @llvm.dbg.declare(metadata i16* %a1, metadata !243, metadata !DIExpression()), !dbg !247
  store i16 %b, i16* %b2
  call void @llvm.dbg.declare(metadata i16* %b2, metadata !244, metadata !DIExpression()), !dbg !247
  call void @llvm.dbg.declare(metadata i16* %rv, metadata !245, metadata !DIExpression()), !dbg !248
  %a3 = load i16, i16* %a1, !dbg !249
  %b4 = load i16, i16* %b2, !dbg !250
  %add = add i16 %a3, %b4, !dbg !251
  store i16 %add, i16* %rv, !dbg !252
  %rv5 = load i16, i16* %rv, !dbg !253
  %signext = sext i16 %rv5 to i32, !dbg !253
  ret i32 %signext, !dbg !253
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ret2(i32 %a, i32* %p) #0 !dbg !254 {
entry:
  %p2 = alloca i32*
  %a1 = alloca i32
  store i32 %a, i32* %a1
  call void @llvm.dbg.declare(metadata i32* %a1, metadata !258, metadata !DIExpression()), !dbg !260
  store i32* %p, i32** %p2
  call void @llvm.dbg.declare(metadata i32** %p2, metadata !259, metadata !DIExpression()), !dbg !260
  %a3 = load i32, i32* %a1, !dbg !261
  %cmplt = icmp slt i32 %a3, 0, !dbg !263
  br i1 %cmplt, label %if, label %else, !dbg !263

if:                                               ; preds = %entry
  ret %UnitType zeroinitializer, !dbg !264

afterreturn:                                      ; No predecessors!
  br label %merge, !dbg !264

else:                                             ; preds = %entry
  br label %merge, !dbg !264

merge:                                            ; preds = %else, %afterreturn
  %phi = phi %UnitType [ zeroinitializer, %afterreturn ], [ zeroinitializer, %else ], !dbg !264
  %p4 = load i32*, i32** %p2, !dbg !266
  %a5 = load i32, i32* %a1, !dbg !267
  store i32 %a5, i32* %p4, !dbg !268
  ret %UnitType zeroinitializer, !dbg !268
}

; Function Attrs: noinline nounwind optnone
define %UnitType @arrays(%"[u16]" %a) #0 !dbg !269 {
entry:
  %n = alloca i16
  %a1 = alloca %"[u16]"
  store %"[u16]" %a, %"[u16]"* %a1
  call void @llvm.dbg.declare(metadata %"[u16]"* %a1, metadata !277, metadata !DIExpression()), !dbg !280
  call void @llvm.dbg.declare(metadata i16* %n, metadata !278, metadata !DIExpression()), !dbg !281
  %a2 = load %"[u16]", %"[u16]"* %a1, !dbg !282
  %size = extractvalue %"[u16]" %a2, 0, !dbg !283
  %check = icmp uge i64 0, %size, !dbg !283
  br i1 %check, label %failed, label %passed, !dbg !283

failed:                                           ; preds = %entry
  call void @exit(i32 1), !dbg !283
  unreachable, !dbg !283

passed:                                           ; preds = %entry
  %data = extractvalue %"[u16]" %a2, 1, !dbg !283
  %value = getelementptr inbounds i16, i16* %data, i8 0, !dbg !283
  %load = load i16, i16* %value, !dbg !283
  store i16 %load, i16* %n, !dbg !284
  %a3 = load %"[u16]", %"[u16]"* %a1, !dbg !285
  %size4 = extractvalue %"[u16]" %a3, 0, !dbg !286
  %check5 = icmp uge i64 1, %size4, !dbg !286
  br i1 %check5, label %failed6, label %passed7, !dbg !286

failed6:                                          ; preds = %passed
  call void @exit(i32 1), !dbg !286
  unreachable, !dbg !286

passed7:                                          ; preds = %passed
  %data8 = extractvalue %"[u16]" %a3, 1, !dbg !286
  %value9 = getelementptr inbounds i16, i16* %data8, i8 1, !dbg !286
  %n10 = load i16, i16* %n, !dbg !287
  %mul = mul i16 %n10, 2, !dbg !288
  store i16 %mul, i16* %value9, !dbg !289
  ret %UnitType zeroinitializer, !dbg !289
}

; Function Attrs: noinline nounwind optnone
define i64 @forIndex(%"[u32]" %a) #0 !dbg !290 {
entry:
  %i = alloca i64
  %x = alloca i32
  %num = alloca i64
  %a1 = alloca %"[u32]"
  store %"[u32]" %a, %"[u32]"* %a1
  call void @llvm.dbg.declare(metadata %"[u32]"* %a1, metadata !298, metadata !DIExpression()), !dbg !304
  call void @llvm.dbg.declare(metadata i64* %num, metadata !299, metadata !DIExpression()), !dbg !305
  store i64 0, i64* %num, !dbg !306
  %a2 = load %"[u32]", %"[u32]"* %a1, !dbg !307
  call void @llvm.dbg.declare(metadata i32* %x, metadata !301, metadata !DIExpression()), !dbg !308
  call void @llvm.dbg.declare(metadata i64* %i, metadata !303, metadata !DIExpression()), !dbg !309
  %size = extractvalue %"[u32]" %a2, 0, !dbg !307
  %data = extractvalue %"[u32]" %a2, 1, !dbg !307
  br label %forCond, !dbg !307

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i64 [ 0, %entry ], [ %inc, %forIter ], !dbg !308
  %cmp = icmp ult i64 %iter, %size, !dbg !308
  br i1 %cmp, label %forBody, label %forExit, !dbg !308

forBody:                                          ; preds = %forCond
  %value = getelementptr inbounds i32, i32* %data, i64 %iter, !dbg !308
  %load = load i32, i32* %value, !dbg !308
  store i32 %load, i32* %x, !dbg !308
  store i64 %iter, i64* %i, !dbg !309
  %x3 = load i32, i32* %x, !dbg !310
  %i4 = load i64, i64* %i, !dbg !312
  %zeroext = zext i32 %x3 to i64, !dbg !313
  %mul = mul i64 %zeroext, %i4, !dbg !313
  %load5 = load i64, i64* %num, !dbg !314
  %add = add i64 %load5, %mul, !dbg !314
  store i64 %add, i64* %num, !dbg !314
  br label %forIter, !dbg !314

forIter:                                          ; preds = %forBody
  %inc = add i64 %iter, 1, !dbg !314
  br label %forCond, !dbg !314

forExit:                                          ; preds = %forCond
  %num6 = load i64, i64* %num, !dbg !315
  ret i64 %num6, !dbg !315
}

; Function Attrs: noinline nounwind optnone
define %UnitType @casts(i32 %a) #0 !dbg !316 {
entry:
  %x = alloca i8
  %a1 = alloca i32
  store i32 %a, i32* %a1
  call void @llvm.dbg.declare(metadata i32* %a1, metadata !318, metadata !DIExpression()), !dbg !321
  call void @llvm.dbg.declare(metadata i8* %x, metadata !319, metadata !DIExpression()), !dbg !322
  %a2 = load i32, i32* %a1, !dbg !323
  %cast = trunc i32 %a2 to i8, !dbg !324
  store i8 %cast, i8* %x, !dbg !325
  ret %UnitType zeroinitializer, !dbg !325
}

; Function Attrs: nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare void @exit(i32)

attributes #0 = { noinline nounwind optnone }
attributes #1 = { nounwind readnone speculatable willreturn }

!llvm.dbg.cu = !{!0}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "WIP Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
$filename
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
!15 = !DILocalVariable(name: "param1", arg: 1, scope: !9, file: !1, line: 6, type: !13)
!16 = !DILocation(line: 6, scope: !9)
!17 = !DILocation(line: 8, column: 5, scope: !18)
!18 = distinct !DILexicalBlock(scope: !9, file: !1, line: 7, column: 1)
!19 = distinct !DISubprogram(name: "operators", scope: !1, file: !1, line: 11, type: !20, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !24)
!20 = !DISubroutineType(flags: DIFlagPrototyped, types: !21)
!21 = !{!22, !6, !22, !23}
!22 = !DIBasicType(name: "i64", size: 64, encoding: DW_ATE_signed)
!23 = !DIBasicType(name: "i8", size: 8, encoding: DW_ATE_signed)
!24 = !{!25, !26, !27}
!25 = !DILocalVariable(name: "x", arg: 1, scope: !19, file: !1, line: 11, type: !6)
!26 = !DILocalVariable(name: "y", arg: 2, scope: !19, file: !1, line: 11, type: !22)
!27 = !DILocalVariable(name: "z", arg: 3, scope: !19, file: !1, line: 11, type: !23)
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
!42 = !DILocalVariable(name: "a", arg: 1, scope: !38, file: !1, line: 16, type: !6)
!43 = !DILocalVariable(name: "b", arg: 2, scope: !38, file: !1, line: 16, type: !6)
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
!76 = !DICompositeType(tag: DW_TAG_structure_type, name: "str", size: 128, elements: !77)
!77 = !{!78, !79}
!78 = !DIDerivedType(tag: DW_TAG_member, name: "Size", baseType: !74, size: 64, align: 4)
!79 = !DIDerivedType(tag: DW_TAG_member, name: "Data", baseType: !80, size: 64, align: 4, offset: 64)
!80 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&u8", baseType: !13, size: 64)
!81 = !DILocation(line: 29, column: 9, scope: !58)
!82 = !DILocation(line: 29, column: 18, scope: !58)
!83 = !DILocation(line: 30, column: 9, scope: !58)
!84 = !DILocation(line: 30, column: 18, scope: !58)
!85 = !DILocation(line: 31, column: 9, scope: !58)
!86 = !DILocation(line: 31, column: 18, scope: !58)
!87 = !DILocation(line: 32, column: 9, scope: !58)
!88 = !DILocation(line: 32, column: 18, scope: !58)
!89 = !DILocation(line: 33, column: 9, scope: !58)
!90 = !DILocation(line: 33, column: 18, scope: !58)
!91 = !DILocation(line: 34, column: 9, scope: !58)
!92 = !DILocation(line: 34, column: 18, scope: !58)
!93 = !DILocation(line: 35, column: 9, scope: !58)
!94 = !DILocation(line: 35, column: 18, scope: !58)
!95 = !DILocation(line: 36, column: 9, scope: !58)
!96 = !DILocation(line: 36, column: 18, scope: !58)
!97 = !DILocation(line: 37, column: 9, scope: !58)
!98 = !DILocation(line: 37, column: 18, scope: !58)
!99 = !DILocation(line: 38, column: 9, scope: !58)
!100 = !DILocation(line: 38, column: 18, scope: !58)
!101 = !DILocation(line: 39, column: 9, scope: !58)
!102 = !DILocation(line: 39, column: 18, scope: !58)
!103 = !DILocation(line: 40, column: 9, scope: !58)
!104 = !DILocation(line: 40, column: 18, scope: !58)
!105 = distinct !DISubprogram(name: "structType", scope: !1, file: !1, line: 43, type: !106, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !108)
!106 = !DISubroutineType(flags: DIFlagPrototyped, types: !107)
!107 = !{!55, !22}
!108 = !{!109, !110}
!109 = !DILocalVariable(name: "n", arg: 1, scope: !105, file: !1, line: 43, type: !22)
!110 = !DILocalVariable(name: "t", scope: !111, file: !1, line: 45, type: !112)
!111 = distinct !DILexicalBlock(scope: !105, file: !1, line: 44, column: 1)
!112 = !DICompositeType(tag: DW_TAG_structure_type, name: "TestStruct", scope: !1, file: !1, line: 53, elements: !113)
!113 = !{!114, !115, !116}
!114 = !DIDerivedType(tag: DW_TAG_member, name: "num1", scope: !1, file: !1, line: 55, baseType: !6, size: 32, align: 32)
!115 = !DIDerivedType(tag: DW_TAG_member, name: "num2", scope: !1, file: !1, line: 56, baseType: !22, size: 64, align: 32, offset: 32)
!116 = !DIDerivedType(tag: DW_TAG_member, name: "flag", scope: !1, file: !1, line: 57, baseType: !59, size: 8, align: 8, offset: 96)
!117 = !DILocation(line: 43, scope: !105)
!118 = !DILocation(line: 45, column: 9, scope: !111)
!119 = !DILocation(line: 48, column: 15, scope: !111)
!120 = !DILocation(line: 48, column: 9, scope: !111)
!121 = !DILocation(line: 49, column: 9, scope: !111)
!122 = !DILocation(line: 45, column: 11, scope: !111)
!123 = distinct !DISubprogram(name: "ranges", scope: !1, file: !1, line: 60, type: !124, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !126)
!124 = !DISubroutineType(flags: DIFlagPrototyped, types: !125)
!125 = !{!55, !6}
!126 = !{!127, !128, !134}
!127 = !DILocalVariable(name: "n", arg: 1, scope: !123, file: !1, line: 60, type: !6)
!128 = !DILocalVariable(name: "r1", scope: !129, file: !1, line: 62, type: !130)
!129 = distinct !DILexicalBlock(scope: !123, file: !1, line: 61, column: 1)
!130 = !DICompositeType(tag: DW_TAG_structure_type, name: "RangeClosed'i32'", size: 64, elements: !131)
!131 = !{!132, !133}
!132 = !DIDerivedType(tag: DW_TAG_member, name: "Start", baseType: !6, size: 32, align: 32)
!133 = !DIDerivedType(tag: DW_TAG_member, name: "End", baseType: !6, size: 32, align: 32, offset: 32)
!134 = !DILocalVariable(name: "r2", scope: !129, file: !1, line: 63, type: !135)
!135 = !DICompositeType(tag: DW_TAG_structure_type, name: "RangeHalfOpen'i32'", size: 64, elements: !131)
!136 = !DILocation(line: 60, scope: !123)
!137 = !DILocation(line: 62, column: 9, scope: !129)
!138 = !DILocation(line: 62, column: 17, scope: !129)
!139 = !DILocation(line: 62, column: 15, scope: !129)
!140 = !DILocation(line: 62, column: 12, scope: !129)
!141 = !DILocation(line: 63, column: 9, scope: !129)
!142 = !DILocation(line: 63, column: 15, scope: !129)
!143 = !DILocation(line: 63, column: 14, scope: !129)
!144 = !DILocation(line: 63, column: 19, scope: !129)
!145 = !DILocation(line: 63, column: 21, scope: !129)
!146 = !DILocation(line: 63, column: 16, scope: !129)
!147 = !DILocation(line: 63, column: 12, scope: !129)
!148 = distinct !DISubprogram(name: "forLoop", scope: !1, file: !1, line: 66, type: !149, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !151)
!149 = !DISubroutineType(flags: DIFlagPrototyped, types: !150)
!150 = !{!6, !6}
!151 = !{!152, !153, !155, !157}
!152 = !DILocalVariable(name: "n", arg: 1, scope: !148, file: !1, line: 66, type: !6)
!153 = !DILocalVariable(name: "a", scope: !154, file: !1, line: 68, type: !6)
!154 = distinct !DILexicalBlock(scope: !148, file: !1, line: 67, column: 1)
!155 = !DILocalVariable(name: "i", scope: !156, file: !1, line: 69, type: !6)
!156 = distinct !DILexicalBlock(scope: !154, file: !1, line: 69, column: 9)
!157 = !DILocalVariable(name: "j", scope: !158, file: !1, line: 71, type: !6)
!158 = distinct !DILexicalBlock(scope: !159, file: !1, line: 71, column: 13)
!159 = distinct !DILexicalBlock(scope: !156, file: !1, line: 70, column: 5)
!160 = !DILocation(line: 66, scope: !148)
!161 = !DILocation(line: 68, column: 9, scope: !154)
!162 = !DILocation(line: 68, column: 15, scope: !154)
!163 = !DILocation(line: 69, column: 17, scope: !154)
!164 = !DILocation(line: 69, column: 15, scope: !154)
!165 = !DILocation(line: 69, column: 9, scope: !156)
!166 = !DILocation(line: 71, column: 18, scope: !159)
!167 = !DILocation(line: 71, column: 19, scope: !159)
!168 = !DILocation(line: 71, column: 13, scope: !158)
!169 = !DILocation(line: 73, column: 18, scope: !170)
!170 = distinct !DILexicalBlock(scope: !158, file: !1, line: 72, column: 9)
!171 = !DILocation(line: 73, column: 22, scope: !170)
!172 = !DILocation(line: 73, column: 20, scope: !170)
!173 = !DILocation(line: 73, column: 15, scope: !170)
!174 = !DILocation(line: 77, column: 5, scope: !154)
!175 = distinct !DISubprogram(name: "pointerStructTest", scope: !1, file: !1, line: 85, type: !176, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !191)
!176 = !DISubroutineType(flags: DIFlagPrototyped, types: !177)
!177 = !{!55, !178}
!178 = !DICompositeType(tag: DW_TAG_structure_type, name: "A", scope: !1, file: !1, line: 81, elements: !179)
!179 = !{!180, !181}
!180 = !DIDerivedType(tag: DW_TAG_member, name: "n", scope: !1, file: !1, line: 81, baseType: !6, size: 32, align: 32)
!181 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !1, file: !1, line: 81, baseType: !182, offset: 32)
!182 = !DICompositeType(tag: DW_TAG_structure_type, name: "B", scope: !1, file: !1, line: 82, elements: !183)
!183 = !{!184, !185}
!184 = !DIDerivedType(tag: DW_TAG_member, name: "n", scope: !1, file: !1, line: 82, baseType: !6, size: 32, align: 32)
!185 = !DIDerivedType(tag: DW_TAG_member, name: "c", scope: !1, file: !1, line: 82, baseType: !186, offset: 32)
!186 = !DICompositeType(tag: DW_TAG_structure_type, name: "C", scope: !1, file: !1, line: 83, elements: !187)
!187 = !{!188, !189}
!188 = !DIDerivedType(tag: DW_TAG_member, name: "n", scope: !1, file: !1, line: 83, baseType: !6, size: 32, align: 32)
!189 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !1, file: !1, line: 83, baseType: !190, size: 64, align: 32, offset: 32)
!190 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&A", baseType: !178, size: 64)
!191 = !{!192}
!192 = !DILocalVariable(name: "a", arg: 1, scope: !175, file: !1, line: 85, type: !178)
!193 = !DILocation(line: 85, scope: !175)
!194 = distinct !DISubprogram(name: "pointers", scope: !1, file: !1, line: 89, type: !124, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !195)
!195 = !{!196, !197}
!196 = !DILocalVariable(name: "x", arg: 1, scope: !194, file: !1, line: 89, type: !6)
!197 = !DILocalVariable(name: "ptr", scope: !198, file: !1, line: 91, type: !199)
!198 = distinct !DILexicalBlock(scope: !194, file: !1, line: 90, column: 1)
!199 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&i32", baseType: !6, size: 64)
!200 = !DILocation(line: 89, scope: !194)
!201 = !DILocation(line: 91, column: 9, scope: !198)
!202 = !DILocation(line: 91, column: 13, scope: !198)
!203 = !DILocation(line: 92, column: 6, scope: !198)
!204 = !DILocation(line: 92, column: 10, scope: !198)
!205 = distinct !DISubprogram(name: "break_and_continue", scope: !1, file: !1, line: 95, type: !53, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !206)
!206 = !{!207, !210}
!207 = !DILocalVariable(name: "i", scope: !208, file: !1, line: 97, type: !23)
!208 = distinct !DILexicalBlock(scope: !209, file: !1, line: 97, column: 9)
!209 = distinct !DILexicalBlock(scope: !205, file: !1, line: 96, column: 1)
!210 = !DILocalVariable(name: "j", scope: !209, file: !1, line: 111, type: !23)
!211 = !DILocation(line: 97, column: 9, scope: !208)
!212 = !DILocation(line: 97, column: 15, scope: !209)
!213 = !DILocation(line: 99, column: 12, scope: !214)
!214 = distinct !DILexicalBlock(scope: !208, file: !1, line: 98, column: 5)
!215 = !DILocation(line: 99, column: 14, scope: !214)
!216 = !DILocation(line: 101, column: 13, scope: !217)
!217 = distinct !DILexicalBlock(scope: !214, file: !1, line: 100, column: 9)
!218 = !DILocation(line: 103, column: 14, scope: !214)
!219 = !DILocation(line: 103, column: 16, scope: !214)
!220 = !DILocation(line: 105, column: 13, scope: !221)
!221 = distinct !DILexicalBlock(scope: !214, file: !1, line: 104, column: 9)
!222 = !DILocation(line: 108, column: 9, scope: !214)
!223 = !DILocation(line: 111, column: 9, scope: !209)
!224 = !DILocation(line: 111, column: 11, scope: !209)
!225 = !DILocation(line: 112, column: 11, scope: !209)
!226 = !DILocation(line: 112, column: 13, scope: !209)
!227 = !DILocation(line: 114, column: 12, scope: !228)
!228 = distinct !DILexicalBlock(scope: !209, file: !1, line: 113, column: 5)
!229 = !DILocation(line: 114, column: 14, scope: !228)
!230 = !DILocation(line: 114, column: 18, scope: !228)
!231 = !DILocation(line: 116, column: 13, scope: !232)
!232 = distinct !DILexicalBlock(scope: !228, file: !1, line: 115, column: 9)
!233 = !DILocation(line: 119, column: 12, scope: !228)
!234 = !DILocation(line: 119, column: 14, scope: !228)
!235 = !DILocation(line: 121, column: 13, scope: !236)
!236 = distinct !DILexicalBlock(scope: !228, file: !1, line: 120, column: 9)
!237 = !DILocation(line: 124, column: 9, scope: !228)
!238 = !DILocation(line: 126, column: 11, scope: !228)
!239 = distinct !DISubprogram(name: "ret1", scope: !1, file: !1, line: 130, type: !240, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !242)
!240 = !DISubroutineType(flags: DIFlagPrototyped, types: !241)
!241 = !{!6, !62, !62}
!242 = !{!243, !244, !245}
!243 = !DILocalVariable(name: "a", arg: 1, scope: !239, file: !1, line: 130, type: !62)
!244 = !DILocalVariable(name: "b", arg: 2, scope: !239, file: !1, line: 130, type: !62)
!245 = !DILocalVariable(name: "rv", scope: !246, file: !1, line: 132, type: !62)
!246 = distinct !DILexicalBlock(scope: !239, file: !1, line: 131, column: 1)
!247 = !DILocation(line: 130, scope: !239)
!248 = !DILocation(line: 132, column: 9, scope: !246)
!249 = !DILocation(line: 132, column: 14, scope: !246)
!250 = !DILocation(line: 132, column: 18, scope: !246)
!251 = !DILocation(line: 132, column: 16, scope: !246)
!252 = !DILocation(line: 132, column: 12, scope: !246)
!253 = !DILocation(line: 133, column: 12, scope: !246)
!254 = distinct !DISubprogram(name: "ret2", scope: !1, file: !1, line: 136, type: !255, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !257)
!255 = !DISubroutineType(flags: DIFlagPrototyped, types: !256)
!256 = !{!55, !6, !199}
!257 = !{!258, !259}
!258 = !DILocalVariable(name: "a", arg: 1, scope: !254, file: !1, line: 136, type: !6)
!259 = !DILocalVariable(name: "p", arg: 2, scope: !254, file: !1, line: 136, type: !199)
!260 = !DILocation(line: 136, scope: !254)
!261 = !DILocation(line: 138, column: 8, scope: !262)
!262 = distinct !DILexicalBlock(scope: !254, file: !1, line: 137, column: 1)
!263 = !DILocation(line: 138, column: 10, scope: !262)
!264 = !DILocation(line: 140, column: 9, scope: !265)
!265 = distinct !DILexicalBlock(scope: !262, file: !1, line: 139, column: 5)
!266 = !DILocation(line: 143, column: 7, scope: !262)
!267 = !DILocation(line: 143, column: 12, scope: !262)
!268 = !DILocation(line: 143, column: 10, scope: !262)
!269 = distinct !DISubprogram(name: "arrays", scope: !1, file: !1, line: 146, type: !270, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !276)
!270 = !DISubroutineType(flags: DIFlagPrototyped, types: !271)
!271 = !{!55, !272}
!272 = !DICompositeType(tag: DW_TAG_structure_type, name: "[u16]", size: 128, elements: !273)
!273 = !{!78, !274}
!274 = !DIDerivedType(tag: DW_TAG_member, name: "Data", baseType: !275, size: 64, align: 4, offset: 64)
!275 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&u16", baseType: !12, size: 64)
!276 = !{!277, !278}
!277 = !DILocalVariable(name: "a", arg: 1, scope: !269, file: !1, line: 146, type: !272)
!278 = !DILocalVariable(name: "n", scope: !279, file: !1, line: 148, type: !12)
!279 = distinct !DILexicalBlock(scope: !269, file: !1, line: 147, column: 1)
!280 = !DILocation(line: 146, scope: !269)
!281 = !DILocation(line: 148, column: 9, scope: !279)
!282 = !DILocation(line: 148, column: 13, scope: !279)
!283 = !DILocation(line: 148, column: 14, scope: !279)
!284 = !DILocation(line: 148, column: 11, scope: !279)
!285 = !DILocation(line: 149, column: 5, scope: !279)
!286 = !DILocation(line: 149, column: 6, scope: !279)
!287 = !DILocation(line: 149, column: 12, scope: !279)
!288 = !DILocation(line: 149, column: 14, scope: !279)
!289 = !DILocation(line: 149, column: 10, scope: !279)
!290 = distinct !DISubprogram(name: "forIndex", scope: !1, file: !1, line: 152, type: !291, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !297)
!291 = !DISubroutineType(flags: DIFlagPrototyped, types: !292)
!292 = !{!74, !293}
!293 = !DICompositeType(tag: DW_TAG_structure_type, name: "[u32]", size: 128, elements: !294)
!294 = !{!78, !295}
!295 = !DIDerivedType(tag: DW_TAG_member, name: "Data", baseType: !296, size: 64, align: 4, offset: 64)
!296 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&u32", baseType: !70, size: 64)
!297 = !{!298, !299, !301, !303}
!298 = !DILocalVariable(name: "a", arg: 1, scope: !290, file: !1, line: 152, type: !293)
!299 = !DILocalVariable(name: "num", scope: !300, file: !1, line: 154, type: !74)
!300 = distinct !DILexicalBlock(scope: !290, file: !1, line: 153, column: 1)
!301 = !DILocalVariable(name: "x", scope: !302, file: !1, line: 155, type: !70)
!302 = distinct !DILexicalBlock(scope: !300, file: !1, line: 155, column: 9)
!303 = !DILocalVariable(name: "i", scope: !302, file: !1, line: 155, type: !74)
!304 = !DILocation(line: 152, scope: !290)
!305 = !DILocation(line: 154, column: 9, scope: !300)
!306 = !DILocation(line: 154, column: 19, scope: !300)
!307 = !DILocation(line: 155, column: 17, scope: !300)
!308 = !DILocation(line: 155, column: 9, scope: !302)
!309 = !DILocation(line: 155, column: 12, scope: !302)
!310 = !DILocation(line: 157, column: 16, scope: !311)
!311 = distinct !DILexicalBlock(scope: !302, file: !1, line: 156, column: 5)
!312 = !DILocation(line: 157, column: 20, scope: !311)
!313 = !DILocation(line: 157, column: 18, scope: !311)
!314 = !DILocation(line: 157, column: 13, scope: !311)
!315 = !DILocation(line: 160, column: 5, scope: !300)
!316 = distinct !DISubprogram(name: "casts", scope: !1, file: !1, line: 163, type: !124, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !317)
!317 = !{!318, !319}
!318 = !DILocalVariable(name: "a", arg: 1, scope: !316, file: !1, line: 163, type: !6)
!319 = !DILocalVariable(name: "x", scope: !320, file: !1, line: 165, type: !23)
!320 = distinct !DILexicalBlock(scope: !316, file: !1, line: 164, column: 1)
!321 = !DILocation(line: 163, scope: !316)
!322 = !DILocation(line: 165, column: 9, scope: !320)
!323 = !DILocation(line: 165, column: 22, scope: !320)
!324 = !DILocation(line: 165, column: 13, scope: !320)
!325 = !DILocation(line: 165, column: 11, scope: !320)
