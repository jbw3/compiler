$module_id
$source_filename
$target_datalayout
$target_triple

%str = type { i64, ptr }
%UnitType = type {}
%TestStruct = type { i32, i64, i1 }
%"RangeHalfOpen'i32'" = type { i32, i32 }
%"RangeClosed'i32'" = type { i32, i32 }
%A = type { i32, %B }
%B = type { i32, %C }
%C = type { i32, ptr }
%"[]u16" = type { i64, ptr }
%"[]u32" = type { i64, ptr }

@strData0 = constant [3 x i8] c"abc"
@strStruct0 = constant %str { i64 3, ptr @strData0 }

; Function Attrs: noinline nounwind optnone
define i32 @noParams() #0 !dbg !2 {
entry:
  ret i32 0, !dbg !6
}

; Function Attrs: noinline nounwind optnone
define i16 @param(i8 %param1) #0 !dbg !8 {
entry:
  %param11 = alloca i8, align 1
  store i8 %param1, ptr %param11, align 1
    #dbg_declare(ptr %param11, !14, !DIExpression(), !15)
  %param12 = load i8, ptr %param11, align 1, !dbg !16
  %zeroext = zext i8 %param12 to i16, !dbg !16
  ret i16 %zeroext, !dbg !16
}

; Function Attrs: noinline nounwind optnone
define i64 @operators(i32 %x, i64 %y, i8 %z) #0 !dbg !18 {
entry:
  %z3 = alloca i8, align 1
  %y2 = alloca i64, align 8
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
    #dbg_declare(ptr %x1, !24, !DIExpression(), !27)
  store i64 %y, ptr %y2, align 8
    #dbg_declare(ptr %y2, !25, !DIExpression(), !27)
  store i8 %z, ptr %z3, align 1
    #dbg_declare(ptr %z3, !26, !DIExpression(), !27)
  %x4 = load i32, ptr %x1, align 4, !dbg !28
  %neg = sub i32 0, %x4, !dbg !30
  %signext = sext i32 %neg to i64, !dbg !30
  %y5 = load i64, ptr %y2, align 8, !dbg !31
  %x6 = load i32, ptr %x1, align 4, !dbg !32
  %z7 = load i8, ptr %z3, align 1, !dbg !33
  %signext8 = sext i8 %z7 to i32, !dbg !33
  %mul = mul i32 %x6, %signext8, !dbg !34
  %signext9 = sext i32 %mul to i64, !dbg !34
  %div = sdiv i64 %y5, %signext9, !dbg !35
  %add = add i64 %signext, %div, !dbg !36
  ret i64 %add, !dbg !36
}

; Function Attrs: noinline nounwind optnone
define i32 @whileLoop(i32 %a, i32 %b) #0 !dbg !37 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
    #dbg_declare(ptr %a1, !41, !DIExpression(), !43)
  store i32 %b, ptr %b2, align 4
    #dbg_declare(ptr %b2, !42, !DIExpression(), !43)
  br label %whileCond

whileCond:                                        ; preds = %whileBody, %entry
  %a3 = load i32, ptr %a1, align 4, !dbg !44
  %cmpgt = icmp sgt i32 %a3, 0, !dbg !46
  br i1 %cmpgt, label %whileBody, label %whileExit, !dbg !46

whileBody:                                        ; preds = %whileCond
  %load = load i32, ptr %b2, align 4, !dbg !47
  %mul = mul i32 %load, 3, !dbg !47
  store i32 %mul, ptr %b2, align 4, !dbg !47
  %load4 = load i32, ptr %a1, align 4, !dbg !49
  %sub = sub i32 %load4, 1, !dbg !49
  store i32 %sub, ptr %a1, align 4, !dbg !49
  br label %whileCond, !dbg !49

whileExit:                                        ; preds = %whileCond
  %b5 = load i32, ptr %b2, align 4, !dbg !50
  ret i32 %b5, !dbg !50
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types() #0 !dbg !51 {
entry:
  %s = alloca %str, align 8
  %x9 = alloca i64, align 8
  %x8 = alloca i64, align 8
  %x7 = alloca i32, align 4
  %x6 = alloca i16, align 2
  %x5 = alloca i8, align 1
  %x4 = alloca i64, align 8
  %x3 = alloca i64, align 8
  %x2 = alloca i32, align 4
  %x1 = alloca i16, align 2
  %x0 = alloca i8, align 1
  %b = alloca i1, align 1
    #dbg_declare(ptr %b, !56, !DIExpression(), !80)
  store i1 true, ptr %b, align 1, !dbg !81
    #dbg_declare(ptr %x0, !59, !DIExpression(), !82)
  store i8 0, ptr %x0, align 1, !dbg !83
    #dbg_declare(ptr %x1, !60, !DIExpression(), !84)
  store i16 1, ptr %x1, align 2, !dbg !85
    #dbg_declare(ptr %x2, !62, !DIExpression(), !86)
  store i32 2, ptr %x2, align 4, !dbg !87
    #dbg_declare(ptr %x3, !63, !DIExpression(), !88)
  store i64 3, ptr %x3, align 8, !dbg !89
    #dbg_declare(ptr %x4, !64, !DIExpression(), !90)
  store i64 4, ptr %x4, align 8, !dbg !91
    #dbg_declare(ptr %x5, !66, !DIExpression(), !92)
  store i8 5, ptr %x5, align 1, !dbg !93
    #dbg_declare(ptr %x6, !67, !DIExpression(), !94)
  store i16 6, ptr %x6, align 2, !dbg !95
    #dbg_declare(ptr %x7, !68, !DIExpression(), !96)
  store i32 7, ptr %x7, align 4, !dbg !97
    #dbg_declare(ptr %x8, !70, !DIExpression(), !98)
  store i64 8, ptr %x8, align 8, !dbg !99
    #dbg_declare(ptr %x9, !72, !DIExpression(), !100)
  store i64 9, ptr %x9, align 8, !dbg !101
    #dbg_declare(ptr %s, !74, !DIExpression(), !102)
  %load = load %str, ptr @strStruct0, align 8, !dbg !102
  store %str %load, ptr %s, align 8, !dbg !103
  ret %UnitType zeroinitializer, !dbg !103
}

; Function Attrs: noinline nounwind optnone
define %UnitType @structType(i64 %n) #0 !dbg !104 {
entry:
  %t = alloca %TestStruct, align 8
  %n1 = alloca i64, align 8
  store i64 %n, ptr %n1, align 8
    #dbg_declare(ptr %n1, !108, !DIExpression(), !116)
    #dbg_declare(ptr %t, !109, !DIExpression(), !117)
  %n2 = load i64, ptr %n1, align 8, !dbg !118
  %agg = insertvalue %TestStruct { i32 12, i64 undef, i1 undef }, i64 %n2, 1, !dbg !119
  %agg3 = insertvalue %TestStruct %agg, i1 true, 2, !dbg !120
  store %TestStruct %agg3, ptr %t, align 8, !dbg !121
  ret %UnitType zeroinitializer, !dbg !121
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ranges(i32 %n) #0 !dbg !122 {
entry:
  %r2 = alloca %"RangeHalfOpen'i32'", align 8
  %r1 = alloca %"RangeClosed'i32'", align 8
  %n1 = alloca i32, align 4
  store i32 %n, ptr %n1, align 4
    #dbg_declare(ptr %n1, !126, !DIExpression(), !135)
    #dbg_declare(ptr %r1, !127, !DIExpression(), !136)
  %n2 = load i32, ptr %n1, align 4, !dbg !137
  %rng = insertvalue %"RangeClosed'i32'" { i32 0, i32 undef }, i32 %n2, 1, !dbg !138
  store %"RangeClosed'i32'" %rng, ptr %r1, align 4, !dbg !139
    #dbg_declare(ptr %r2, !133, !DIExpression(), !140)
  %n3 = load i32, ptr %n1, align 4, !dbg !141
  %neg = sub i32 0, %n3, !dbg !142
  %n4 = load i32, ptr %n1, align 4, !dbg !143
  %mul = mul i32 %n4, 3, !dbg !144
  %rng5 = insertvalue %"RangeHalfOpen'i32'" undef, i32 %neg, 0, !dbg !145
  %rng6 = insertvalue %"RangeHalfOpen'i32'" %rng5, i32 %mul, 1, !dbg !145
  store %"RangeHalfOpen'i32'" %rng6, ptr %r2, align 4, !dbg !146
  ret %UnitType zeroinitializer, !dbg !146
}

; Function Attrs: noinline nounwind optnone
define i32 @forLoop(i32 %n) #0 !dbg !147 {
entry:
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %n1 = alloca i32, align 4
  store i32 %n, ptr %n1, align 4
    #dbg_declare(ptr %n1, !151, !DIExpression(), !159)
    #dbg_declare(ptr %a, !152, !DIExpression(), !160)
  store i32 0, ptr %a, align 4, !dbg !161
  %n2 = load i32, ptr %n1, align 4, !dbg !162
  %rng = insertvalue %"RangeClosed'i32'" { i32 0, i32 undef }, i32 %n2, 1, !dbg !163
    #dbg_declare(ptr %i, !154, !DIExpression(), !164)
  %start = extractvalue %"RangeClosed'i32'" %rng, 0, !dbg !163
  %end = extractvalue %"RangeClosed'i32'" %rng, 1, !dbg !163
  br label %forCond, !dbg !163

forCond:                                          ; preds = %forIter14, %entry
  %iter = phi i32 [ %start, %entry ], [ %inc15, %forIter14 ], !dbg !164
  %cmp = icmp sle i32 %iter, %end, !dbg !164
  br i1 %cmp, label %forBody, label %forExit16, !dbg !164

forBody:                                          ; preds = %forCond
  store i32 %iter, ptr %i, align 4, !dbg !164
  %n3 = load i32, ptr %n1, align 4, !dbg !165
  %rng4 = insertvalue %"RangeClosed'i32'" undef, i32 %n3, 0, !dbg !166
  %rng5 = insertvalue %"RangeClosed'i32'" %rng4, i32 10, 1, !dbg !166
    #dbg_declare(ptr %j, !156, !DIExpression(), !167)
  %start6 = extractvalue %"RangeClosed'i32'" %rng5, 0, !dbg !166
  %end7 = extractvalue %"RangeClosed'i32'" %rng5, 1, !dbg !166
  br label %forCond8, !dbg !166

forCond8:                                         ; preds = %forIter, %forBody
  %iter9 = phi i32 [ %start6, %forBody ], [ %inc, %forIter ], !dbg !167
  %cmp10 = icmp sle i32 %iter9, %end7, !dbg !167
  br i1 %cmp10, label %forBody11, label %forExit, !dbg !167

forBody11:                                        ; preds = %forCond8
  store i32 %iter9, ptr %j, align 4, !dbg !167
  %i12 = load i32, ptr %i, align 4, !dbg !168
  %j13 = load i32, ptr %j, align 4, !dbg !170
  %mul = mul i32 %i12, %j13, !dbg !171
  %load = load i32, ptr %a, align 4, !dbg !172
  %add = add i32 %load, %mul, !dbg !172
  store i32 %add, ptr %a, align 4, !dbg !172
  br label %forIter, !dbg !172

forIter:                                          ; preds = %forBody11
  %inc = add i32 %iter9, 1, !dbg !172
  br label %forCond8, !dbg !172

forExit:                                          ; preds = %forCond8
  br label %forIter14, !dbg !172

forIter14:                                        ; preds = %forExit
  %inc15 = add i32 %iter, 1, !dbg !172
  br label %forCond, !dbg !172

forExit16:                                        ; preds = %forCond
  %a17 = load i32, ptr %a, align 4, !dbg !173
  ret i32 %a17, !dbg !173
}

; Function Attrs: noinline nounwind optnone
define %UnitType @pointerStructTest(%A %a) #0 !dbg !174 {
entry:
  %a1 = alloca %A, align 8
  store %A %a, ptr %a1, align 8
    #dbg_declare(ptr %a1, !191, !DIExpression(), !192)
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @pointers(i32 %x) #0 !dbg !193 {
entry:
  %ptr = alloca ptr, align 8
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
    #dbg_declare(ptr %x1, !195, !DIExpression(), !199)
    #dbg_declare(ptr %ptr, !196, !DIExpression(), !200)
  store ptr %x1, ptr %ptr, align 8, !dbg !201
  %ptr2 = load ptr, ptr %ptr, align 8, !dbg !202
  store i32 17, ptr %ptr2, align 4, !dbg !203
  ret %UnitType zeroinitializer, !dbg !203
}

; Function Attrs: noinline nounwind optnone
define %UnitType @break_and_continue() #0 !dbg !204 {
entry:
  %j = alloca i8, align 1
  %i = alloca i8, align 1
    #dbg_declare(ptr %i, !206, !DIExpression(), !210)
  br label %forCond, !dbg !211

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i8 [ 0, %entry ], [ %inc, %forIter ], !dbg !210
  %cmp = icmp sle i8 %iter, 7, !dbg !210
  br i1 %cmp, label %forBody, label %forExit, !dbg !210

forBody:                                          ; preds = %forCond
  store i8 %iter, ptr %i, align 1, !dbg !210
  %i1 = load i8, ptr %i, align 1, !dbg !212
  %cmpeq = icmp eq i8 %i1, 5, !dbg !214
  br i1 %cmpeq, label %if, label %else, !dbg !214

if:                                               ; preds = %forBody
  br label %forExit, !dbg !215

afterbreak:                                       ; No predecessors!
  br label %merge6, !dbg !215

else:                                             ; preds = %forBody
  %i2 = load i8, ptr %i, align 1, !dbg !217
  %cmpeq3 = icmp eq i8 %i2, 2, !dbg !218
  br i1 %cmpeq3, label %if4, label %else5, !dbg !218

if4:                                              ; preds = %else
  br label %forIter, !dbg !219

aftercontinue:                                    ; No predecessors!
  br label %merge, !dbg !219

else5:                                            ; preds = %else
  br label %merge, !dbg !219

merge:                                            ; preds = %else5, %aftercontinue
  %phi = phi %UnitType [ zeroinitializer, %aftercontinue ], [ zeroinitializer, %else5 ], !dbg !219
  br label %merge6, !dbg !219

merge6:                                           ; preds = %merge, %afterbreak
  %phi7 = phi %UnitType [ zeroinitializer, %afterbreak ], [ %phi, %merge ], !dbg !219
  %call = call i32 @noParams(), !dbg !221
  br label %forIter, !dbg !221

forIter:                                          ; preds = %merge6, %if4
  %inc = add i8 %iter, 1, !dbg !221
  br label %forCond, !dbg !221

forExit:                                          ; preds = %if, %forCond
    #dbg_declare(ptr %j, !209, !DIExpression(), !222)
  store i8 1, ptr %j, align 1, !dbg !223
  br label %whileCond, !dbg !223

whileCond:                                        ; preds = %merge20, %if11, %forExit
  %j8 = load i8, ptr %j, align 1, !dbg !224
  %cmplt = icmp slt i8 %j8, 100, !dbg !225
  br i1 %cmplt, label %whileBody, label %whileExit, !dbg !225

whileBody:                                        ; preds = %whileCond
  %j9 = load i8, ptr %j, align 1, !dbg !226
  %rem = srem i8 %j9, 7, !dbg !228
  %cmpeq10 = icmp eq i8 %rem, 0, !dbg !229
  br i1 %cmpeq10, label %if11, label %else13, !dbg !229

if11:                                             ; preds = %whileBody
  br label %whileCond, !dbg !230

aftercontinue12:                                  ; No predecessors!
  br label %merge14, !dbg !230

else13:                                           ; preds = %whileBody
  br label %merge14, !dbg !230

merge14:                                          ; preds = %else13, %aftercontinue12
  %phi15 = phi %UnitType [ zeroinitializer, %aftercontinue12 ], [ zeroinitializer, %else13 ], !dbg !230
  %j16 = load i8, ptr %j, align 1, !dbg !232
  %cmpgt = icmp sgt i8 %j16, 89, !dbg !233
  br i1 %cmpgt, label %if17, label %else19, !dbg !233

if17:                                             ; preds = %merge14
  br label %whileExit, !dbg !234

afterbreak18:                                     ; No predecessors!
  br label %merge20, !dbg !234

else19:                                           ; preds = %merge14
  br label %merge20, !dbg !234

merge20:                                          ; preds = %else19, %afterbreak18
  %phi21 = phi %UnitType [ zeroinitializer, %afterbreak18 ], [ zeroinitializer, %else19 ], !dbg !234
  %call22 = call i32 @noParams(), !dbg !236
  %load = load i8, ptr %j, align 1, !dbg !237
  %add = add i8 %load, 1, !dbg !237
  store i8 %add, ptr %j, align 1, !dbg !237
  br label %whileCond, !dbg !237

whileExit:                                        ; preds = %if17, %whileCond
  ret %UnitType zeroinitializer, !dbg !237
}

; Function Attrs: noinline nounwind optnone
define i32 @ret1(i16 %a, i16 %b) #0 !dbg !238 {
entry:
  %rv = alloca i16, align 2
  %b2 = alloca i16, align 2
  %a1 = alloca i16, align 2
  store i16 %a, ptr %a1, align 2
    #dbg_declare(ptr %a1, !242, !DIExpression(), !246)
  store i16 %b, ptr %b2, align 2
    #dbg_declare(ptr %b2, !243, !DIExpression(), !246)
    #dbg_declare(ptr %rv, !244, !DIExpression(), !247)
  %a3 = load i16, ptr %a1, align 2, !dbg !248
  %b4 = load i16, ptr %b2, align 2, !dbg !249
  %add = add i16 %a3, %b4, !dbg !250
  store i16 %add, ptr %rv, align 2, !dbg !251
  %rv5 = load i16, ptr %rv, align 2, !dbg !252
  %signext = sext i16 %rv5 to i32, !dbg !252
  ret i32 %signext, !dbg !252
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ret2(i32 %a, ptr %p) #0 !dbg !253 {
entry:
  %p2 = alloca ptr, align 8
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
    #dbg_declare(ptr %a1, !257, !DIExpression(), !259)
  store ptr %p, ptr %p2, align 8
    #dbg_declare(ptr %p2, !258, !DIExpression(), !259)
  %a3 = load i32, ptr %a1, align 4, !dbg !260
  %cmplt = icmp slt i32 %a3, 0, !dbg !262
  br i1 %cmplt, label %if, label %else, !dbg !262

if:                                               ; preds = %entry
  ret %UnitType zeroinitializer, !dbg !263

afterreturn:                                      ; No predecessors!
  br label %merge, !dbg !263

else:                                             ; preds = %entry
  br label %merge, !dbg !263

merge:                                            ; preds = %else, %afterreturn
  %phi = phi %UnitType [ zeroinitializer, %afterreturn ], [ zeroinitializer, %else ], !dbg !263
  %p4 = load ptr, ptr %p2, align 8, !dbg !265
  %a5 = load i32, ptr %a1, align 4, !dbg !266
  store i32 %a5, ptr %p4, align 4, !dbg !267
  ret %UnitType zeroinitializer, !dbg !267
}

; Function Attrs: noinline nounwind optnone
define %UnitType @arrays(%"[]u16" %a) #0 !dbg !268 {
entry:
  %n = alloca i16, align 2
  %a1 = alloca %"[]u16", align 8
  store %"[]u16" %a, ptr %a1, align 8
    #dbg_declare(ptr %a1, !276, !DIExpression(), !279)
    #dbg_declare(ptr %n, !277, !DIExpression(), !280)
  %a2 = load %"[]u16", ptr %a1, align 8, !dbg !281
  %size = extractvalue %"[]u16" %a2, 0, !dbg !282
  %check = icmp uge i64 0, %size, !dbg !282
  br i1 %check, label %failed, label %passed, !dbg !282

failed:                                           ; preds = %entry
  call void @exit(i32 1), !dbg !282
  unreachable, !dbg !282

passed:                                           ; preds = %entry
  %data = extractvalue %"[]u16" %a2, 1, !dbg !282
  %value = getelementptr inbounds i16, ptr %data, i64 0, !dbg !282
  %load = load i16, ptr %value, align 2, !dbg !282
  store i16 %load, ptr %n, align 2, !dbg !283
  %a3 = load %"[]u16", ptr %a1, align 8, !dbg !284
  %size4 = extractvalue %"[]u16" %a3, 0, !dbg !285
  %check5 = icmp uge i64 1, %size4, !dbg !285
  br i1 %check5, label %failed6, label %passed7, !dbg !285

failed6:                                          ; preds = %passed
  call void @exit(i32 1), !dbg !285
  unreachable, !dbg !285

passed7:                                          ; preds = %passed
  %data8 = extractvalue %"[]u16" %a3, 1, !dbg !285
  %value9 = getelementptr inbounds i16, ptr %data8, i64 1, !dbg !285
  %n10 = load i16, ptr %n, align 2, !dbg !286
  %mul = mul i16 %n10, 2, !dbg !287
  store i16 %mul, ptr %value9, align 2, !dbg !288
  ret %UnitType zeroinitializer, !dbg !288
}

; Function Attrs: noinline nounwind optnone
define i64 @forIndex(%"[]u32" %a) #0 !dbg !289 {
entry:
  %i = alloca i64, align 8
  %x = alloca i32, align 4
  %num = alloca i64, align 8
  %a1 = alloca %"[]u32", align 8
  store %"[]u32" %a, ptr %a1, align 8
    #dbg_declare(ptr %a1, !297, !DIExpression(), !303)
    #dbg_declare(ptr %num, !298, !DIExpression(), !304)
  store i64 0, ptr %num, align 8, !dbg !305
  %a2 = load %"[]u32", ptr %a1, align 8, !dbg !306
    #dbg_declare(ptr %x, !300, !DIExpression(), !307)
    #dbg_declare(ptr %i, !302, !DIExpression(), !308)
  %size = extractvalue %"[]u32" %a2, 0, !dbg !306
  %data = extractvalue %"[]u32" %a2, 1, !dbg !306
  br label %forCond, !dbg !306

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i64 [ 0, %entry ], [ %inc, %forIter ], !dbg !307
  %cmp = icmp ult i64 %iter, %size, !dbg !307
  br i1 %cmp, label %forBody, label %forExit, !dbg !307

forBody:                                          ; preds = %forCond
  %value = getelementptr inbounds i32, ptr %data, i64 %iter, !dbg !307
  %load = load i32, ptr %value, align 4, !dbg !307
  store i32 %load, ptr %x, align 4, !dbg !307
  store i64 %iter, ptr %i, align 8, !dbg !308
  %x3 = load i32, ptr %x, align 4, !dbg !309
  %zeroext = zext i32 %x3 to i64, !dbg !309
  %i4 = load i64, ptr %i, align 8, !dbg !311
  %mul = mul i64 %zeroext, %i4, !dbg !312
  %load5 = load i64, ptr %num, align 8, !dbg !313
  %add = add i64 %load5, %mul, !dbg !313
  store i64 %add, ptr %num, align 8, !dbg !313
  br label %forIter, !dbg !313

forIter:                                          ; preds = %forBody
  %inc = add i64 %iter, 1, !dbg !313
  br label %forCond, !dbg !313

forExit:                                          ; preds = %forCond
  %num6 = load i64, ptr %num, align 8, !dbg !314
  ret i64 %num6, !dbg !314
}

; Function Attrs: noinline nounwind optnone
define %UnitType @casts(i32 %a) #0 !dbg !315 {
entry:
  %x = alloca i8, align 1
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
    #dbg_declare(ptr %a1, !317, !DIExpression(), !320)
    #dbg_declare(ptr %x, !318, !DIExpression(), !321)
  %a2 = load i32, ptr %a1, align 4, !dbg !322
  %cast = trunc i32 %a2 to i8, !dbg !323
  store i8 %cast, ptr %x, align 1, !dbg !324
  ret %UnitType zeroinitializer, !dbg !324
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ftype1() #0 !dbg !325 {
entry:
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i1 @ftype2() #0 !dbg !326 {
entry:
  ret i1 false, !dbg !329
}

; Function Attrs: noinline nounwind optnone
define %UnitType @function_type() #0 !dbg !331 {
entry:
  %f2 = alloca ptr, align 8
  %f1 = alloca ptr, align 8
    #dbg_declare(ptr %f1, !333, !DIExpression(), !340)
  store ptr @ftype1, ptr %f1, align 8, !dbg !341
  %f11 = load ptr, ptr %f1, align 8, !dbg !342
  %call = call %UnitType %f11(), !dbg !342
    #dbg_declare(ptr %f2, !337, !DIExpression(), !343)
  store ptr @ftype2, ptr %f2, align 8, !dbg !344
  %f22 = load ptr, ptr %f2, align 8, !dbg !345
  %call3 = call i1 %f22(), !dbg !345
  ret %UnitType zeroinitializer, !dbg !345
}

; Function Attrs: noinline nounwind optnone
define double @floats(double %a) #0 !dbg !346 {
entry:
  %y = alloca double, align 8
  %x = alloca float, align 4
  %a1 = alloca double, align 8
  store double %a, ptr %a1, align 8
    #dbg_declare(ptr %a1, !351, !DIExpression(), !356)
    #dbg_declare(ptr %x, !352, !DIExpression(), !357)
  store float 0x3F8945B6C0000000, ptr %x, align 4, !dbg !358
    #dbg_declare(ptr %y, !355, !DIExpression(), !359)
  store double 3.141590e+00, ptr %y, align 8, !dbg !360
  %a2 = load double, ptr %a1, align 8, !dbg !361
  %x3 = load float, ptr %x, align 4, !dbg !362
  %fpext = fpext float %x3 to double, !dbg !362
  %y4 = load double, ptr %y, align 8, !dbg !363
  %mul = fmul double %fpext, %y4, !dbg !364
  %add = fadd double %a2, %mul, !dbg !365
  ret double %add, !dbg !365
}

declare void @exit(i32)

attributes #0 = { noinline nounwind optnone }

!llvm.dbg.cu = !{!0}

!0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "WIP Compiler", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug)
$filename
!2 = distinct !DISubprogram(name: "noParams", scope: !1, file: !1, line: 1, type: !3, spFlags: DISPFlagDefinition, unit: !0)
!3 = !DISubroutineType(flags: DIFlagPrototyped, types: !4)
!4 = !{!5}
!5 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
!6 = !DILocation(line: 3, column: 5, scope: !7)
!7 = distinct !DILexicalBlock(scope: !2, file: !1, line: 2, column: 1)
!8 = distinct !DISubprogram(name: "param", scope: !1, file: !1, line: 6, type: !9, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !13)
!9 = !DISubroutineType(flags: DIFlagPrototyped, types: !10)
!10 = !{!11, !12}
!11 = !DIBasicType(name: "u16", size: 16, encoding: DW_ATE_unsigned)
!12 = !DIBasicType(name: "u8", size: 8, encoding: DW_ATE_unsigned)
!13 = !{!14}
!14 = !DILocalVariable(name: "param1", arg: 1, scope: !8, file: !1, line: 6, type: !12)
!15 = !DILocation(line: 6, scope: !8)
!16 = !DILocation(line: 8, column: 5, scope: !17)
!17 = distinct !DILexicalBlock(scope: !8, file: !1, line: 7, column: 1)
!18 = distinct !DISubprogram(name: "operators", scope: !1, file: !1, line: 11, type: !19, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !23)
!19 = !DISubroutineType(flags: DIFlagPrototyped, types: !20)
!20 = !{!21, !5, !21, !22}
!21 = !DIBasicType(name: "i64", size: 64, encoding: DW_ATE_signed)
!22 = !DIBasicType(name: "i8", size: 8, encoding: DW_ATE_signed)
!23 = !{!24, !25, !26}
!24 = !DILocalVariable(name: "x", arg: 1, scope: !18, file: !1, line: 11, type: !5)
!25 = !DILocalVariable(name: "y", arg: 2, scope: !18, file: !1, line: 11, type: !21)
!26 = !DILocalVariable(name: "z", arg: 3, scope: !18, file: !1, line: 11, type: !22)
!27 = !DILocation(line: 11, scope: !18)
!28 = !DILocation(line: 13, column: 6, scope: !29)
!29 = distinct !DILexicalBlock(scope: !18, file: !1, line: 12, column: 1)
!30 = !DILocation(line: 13, column: 5, scope: !29)
!31 = !DILocation(line: 13, column: 10, scope: !29)
!32 = !DILocation(line: 13, column: 15, scope: !29)
!33 = !DILocation(line: 13, column: 19, scope: !29)
!34 = !DILocation(line: 13, column: 17, scope: !29)
!35 = !DILocation(line: 13, column: 12, scope: !29)
!36 = !DILocation(line: 13, column: 8, scope: !29)
!37 = distinct !DISubprogram(name: "whileLoop", scope: !1, file: !1, line: 16, type: !38, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !40)
!38 = !DISubroutineType(flags: DIFlagPrototyped, types: !39)
!39 = !{!5, !5, !5}
!40 = !{!41, !42}
!41 = !DILocalVariable(name: "a", arg: 1, scope: !37, file: !1, line: 16, type: !5)
!42 = !DILocalVariable(name: "b", arg: 2, scope: !37, file: !1, line: 16, type: !5)
!43 = !DILocation(line: 16, scope: !37)
!44 = !DILocation(line: 18, column: 11, scope: !45)
!45 = distinct !DILexicalBlock(scope: !37, file: !1, line: 17, column: 1)
!46 = !DILocation(line: 18, column: 13, scope: !45)
!47 = !DILocation(line: 20, column: 11, scope: !48)
!48 = distinct !DILexicalBlock(scope: !45, file: !1, line: 19, column: 5)
!49 = !DILocation(line: 21, column: 11, scope: !48)
!50 = !DILocation(line: 24, column: 5, scope: !45)
!51 = distinct !DISubprogram(name: "types", scope: !1, file: !1, line: 27, type: !52, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !55)
!52 = !DISubroutineType(flags: DIFlagPrototyped, types: !53)
!53 = !{!54}
!54 = !DIBasicType(name: "Unit", encoding: DW_ATE_unsigned)
!55 = !{!56, !59, !60, !62, !63, !64, !66, !67, !68, !70, !72, !74}
!56 = !DILocalVariable(name: "b", scope: !57, file: !1, line: 29, type: !58)
!57 = distinct !DILexicalBlock(scope: !51, file: !1, line: 28, column: 1)
!58 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!59 = !DILocalVariable(name: "x0", scope: !57, file: !1, line: 30, type: !22)
!60 = !DILocalVariable(name: "x1", scope: !57, file: !1, line: 31, type: !61)
!61 = !DIBasicType(name: "i16", size: 16, encoding: DW_ATE_signed)
!62 = !DILocalVariable(name: "x2", scope: !57, file: !1, line: 32, type: !5)
!63 = !DILocalVariable(name: "x3", scope: !57, file: !1, line: 33, type: !21)
!64 = !DILocalVariable(name: "x4", scope: !57, file: !1, line: 34, type: !65)
!65 = !DIBasicType(name: "isize", size: 64, encoding: DW_ATE_signed)
!66 = !DILocalVariable(name: "x5", scope: !57, file: !1, line: 35, type: !12)
!67 = !DILocalVariable(name: "x6", scope: !57, file: !1, line: 36, type: !11)
!68 = !DILocalVariable(name: "x7", scope: !57, file: !1, line: 37, type: !69)
!69 = !DIBasicType(name: "u32", size: 32, encoding: DW_ATE_unsigned)
!70 = !DILocalVariable(name: "x8", scope: !57, file: !1, line: 38, type: !71)
!71 = !DIBasicType(name: "u64", size: 64, encoding: DW_ATE_unsigned)
!72 = !DILocalVariable(name: "x9", scope: !57, file: !1, line: 39, type: !73)
!73 = !DIBasicType(name: "usize", size: 64, encoding: DW_ATE_unsigned)
!74 = !DILocalVariable(name: "s", scope: !57, file: !1, line: 40, type: !75)
!75 = !DICompositeType(tag: DW_TAG_structure_type, name: "str", size: 128, elements: !76)
!76 = !{!77, !78}
!77 = !DIDerivedType(tag: DW_TAG_member, name: "Size", baseType: !73, size: 64, align: 4)
!78 = !DIDerivedType(tag: DW_TAG_member, name: "Data", baseType: !79, size: 64, align: 4, offset: 64)
!79 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&u8", baseType: !12, size: 64)
!80 = !DILocation(line: 29, column: 9, scope: !57)
!81 = !DILocation(line: 29, column: 18, scope: !57)
!82 = !DILocation(line: 30, column: 9, scope: !57)
!83 = !DILocation(line: 30, column: 18, scope: !57)
!84 = !DILocation(line: 31, column: 9, scope: !57)
!85 = !DILocation(line: 31, column: 18, scope: !57)
!86 = !DILocation(line: 32, column: 9, scope: !57)
!87 = !DILocation(line: 32, column: 18, scope: !57)
!88 = !DILocation(line: 33, column: 9, scope: !57)
!89 = !DILocation(line: 33, column: 18, scope: !57)
!90 = !DILocation(line: 34, column: 9, scope: !57)
!91 = !DILocation(line: 34, column: 18, scope: !57)
!92 = !DILocation(line: 35, column: 9, scope: !57)
!93 = !DILocation(line: 35, column: 18, scope: !57)
!94 = !DILocation(line: 36, column: 9, scope: !57)
!95 = !DILocation(line: 36, column: 18, scope: !57)
!96 = !DILocation(line: 37, column: 9, scope: !57)
!97 = !DILocation(line: 37, column: 18, scope: !57)
!98 = !DILocation(line: 38, column: 9, scope: !57)
!99 = !DILocation(line: 38, column: 18, scope: !57)
!100 = !DILocation(line: 39, column: 9, scope: !57)
!101 = !DILocation(line: 39, column: 18, scope: !57)
!102 = !DILocation(line: 40, column: 9, scope: !57)
!103 = !DILocation(line: 40, column: 18, scope: !57)
!104 = distinct !DISubprogram(name: "structType", scope: !1, file: !1, line: 43, type: !105, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !107)
!105 = !DISubroutineType(flags: DIFlagPrototyped, types: !106)
!106 = !{!54, !21}
!107 = !{!108, !109}
!108 = !DILocalVariable(name: "n", arg: 1, scope: !104, file: !1, line: 43, type: !21)
!109 = !DILocalVariable(name: "t", scope: !110, file: !1, line: 45, type: !111)
!110 = distinct !DILexicalBlock(scope: !104, file: !1, line: 44, column: 1)
!111 = !DICompositeType(tag: DW_TAG_structure_type, name: "TestStruct", scope: !1, file: !1, line: 53, elements: !112)
!112 = !{!113, !114, !115}
!113 = !DIDerivedType(tag: DW_TAG_member, name: "num1", scope: !1, file: !1, line: 55, baseType: !5, size: 32, align: 32)
!114 = !DIDerivedType(tag: DW_TAG_member, name: "num2", scope: !1, file: !1, line: 56, baseType: !21, size: 64, align: 64, offset: 64)
!115 = !DIDerivedType(tag: DW_TAG_member, name: "flag", scope: !1, file: !1, line: 57, baseType: !58, size: 8, align: 8, offset: 128)
!116 = !DILocation(line: 43, scope: !104)
!117 = !DILocation(line: 45, column: 9, scope: !110)
!118 = !DILocation(line: 48, column: 16, scope: !110)
!119 = !DILocation(line: 48, column: 9, scope: !110)
!120 = !DILocation(line: 49, column: 9, scope: !110)
!121 = !DILocation(line: 45, column: 11, scope: !110)
!122 = distinct !DISubprogram(name: "ranges", scope: !1, file: !1, line: 60, type: !123, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !125)
!123 = !DISubroutineType(flags: DIFlagPrototyped, types: !124)
!124 = !{!54, !5}
!125 = !{!126, !127, !133}
!126 = !DILocalVariable(name: "n", arg: 1, scope: !122, file: !1, line: 60, type: !5)
!127 = !DILocalVariable(name: "r1", scope: !128, file: !1, line: 62, type: !129)
!128 = distinct !DILexicalBlock(scope: !122, file: !1, line: 61, column: 1)
!129 = !DICompositeType(tag: DW_TAG_structure_type, name: "RangeClosed'i32'", size: 64, elements: !130)
!130 = !{!131, !132}
!131 = !DIDerivedType(tag: DW_TAG_member, name: "Start", baseType: !5, size: 32, align: 32)
!132 = !DIDerivedType(tag: DW_TAG_member, name: "End", baseType: !5, size: 32, align: 32, offset: 32)
!133 = !DILocalVariable(name: "r2", scope: !128, file: !1, line: 63, type: !134)
!134 = !DICompositeType(tag: DW_TAG_structure_type, name: "RangeHalfOpen'i32'", size: 64, elements: !130)
!135 = !DILocation(line: 60, scope: !122)
!136 = !DILocation(line: 62, column: 9, scope: !128)
!137 = !DILocation(line: 62, column: 17, scope: !128)
!138 = !DILocation(line: 62, column: 15, scope: !128)
!139 = !DILocation(line: 62, column: 12, scope: !128)
!140 = !DILocation(line: 63, column: 9, scope: !128)
!141 = !DILocation(line: 63, column: 15, scope: !128)
!142 = !DILocation(line: 63, column: 14, scope: !128)
!143 = !DILocation(line: 63, column: 19, scope: !128)
!144 = !DILocation(line: 63, column: 21, scope: !128)
!145 = !DILocation(line: 63, column: 16, scope: !128)
!146 = !DILocation(line: 63, column: 12, scope: !128)
!147 = distinct !DISubprogram(name: "forLoop", scope: !1, file: !1, line: 66, type: !148, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !150)
!148 = !DISubroutineType(flags: DIFlagPrototyped, types: !149)
!149 = !{!5, !5}
!150 = !{!151, !152, !154, !156}
!151 = !DILocalVariable(name: "n", arg: 1, scope: !147, file: !1, line: 66, type: !5)
!152 = !DILocalVariable(name: "a", scope: !153, file: !1, line: 68, type: !5)
!153 = distinct !DILexicalBlock(scope: !147, file: !1, line: 67, column: 1)
!154 = !DILocalVariable(name: "i", scope: !155, file: !1, line: 69, type: !5)
!155 = distinct !DILexicalBlock(scope: !153, file: !1, line: 69, column: 9)
!156 = !DILocalVariable(name: "j", scope: !157, file: !1, line: 71, type: !5)
!157 = distinct !DILexicalBlock(scope: !158, file: !1, line: 71, column: 13)
!158 = distinct !DILexicalBlock(scope: !155, file: !1, line: 70, column: 5)
!159 = !DILocation(line: 66, scope: !147)
!160 = !DILocation(line: 68, column: 9, scope: !153)
!161 = !DILocation(line: 68, column: 15, scope: !153)
!162 = !DILocation(line: 69, column: 17, scope: !153)
!163 = !DILocation(line: 69, column: 15, scope: !153)
!164 = !DILocation(line: 69, column: 9, scope: !155)
!165 = !DILocation(line: 71, column: 18, scope: !158)
!166 = !DILocation(line: 71, column: 19, scope: !158)
!167 = !DILocation(line: 71, column: 13, scope: !157)
!168 = !DILocation(line: 73, column: 18, scope: !169)
!169 = distinct !DILexicalBlock(scope: !157, file: !1, line: 72, column: 9)
!170 = !DILocation(line: 73, column: 22, scope: !169)
!171 = !DILocation(line: 73, column: 20, scope: !169)
!172 = !DILocation(line: 73, column: 15, scope: !169)
!173 = !DILocation(line: 77, column: 5, scope: !153)
!174 = distinct !DISubprogram(name: "pointerStructTest", scope: !1, file: !1, line: 85, type: !175, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !190)
!175 = !DISubroutineType(flags: DIFlagPrototyped, types: !176)
!176 = !{!54, !177}
!177 = !DICompositeType(tag: DW_TAG_structure_type, name: "A", scope: !1, file: !1, line: 81, elements: !178)
!178 = !{!179, !180}
!179 = !DIDerivedType(tag: DW_TAG_member, name: "n", scope: !1, file: !1, line: 81, baseType: !5, size: 32, align: 32)
!180 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !1, file: !1, line: 81, baseType: !181, offset: 32)
!181 = !DICompositeType(tag: DW_TAG_structure_type, name: "B", scope: !1, file: !1, line: 82, elements: !182)
!182 = !{!183, !184}
!183 = !DIDerivedType(tag: DW_TAG_member, name: "n", scope: !1, file: !1, line: 82, baseType: !5, size: 32, align: 32)
!184 = !DIDerivedType(tag: DW_TAG_member, name: "c", scope: !1, file: !1, line: 82, baseType: !185, offset: 32)
!185 = !DICompositeType(tag: DW_TAG_structure_type, name: "C", scope: !1, file: !1, line: 83, elements: !186)
!186 = !{!187, !188}
!187 = !DIDerivedType(tag: DW_TAG_member, name: "n", scope: !1, file: !1, line: 83, baseType: !5, size: 32, align: 32)
!188 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !1, file: !1, line: 83, baseType: !189, size: 64, align: 64, offset: 64)
!189 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&A", baseType: !177, size: 64)
!190 = !{!191}
!191 = !DILocalVariable(name: "a", arg: 1, scope: !174, file: !1, line: 85, type: !177)
!192 = !DILocation(line: 85, scope: !174)
!193 = distinct !DISubprogram(name: "pointers", scope: !1, file: !1, line: 89, type: !123, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !194)
!194 = !{!195, !196}
!195 = !DILocalVariable(name: "x", arg: 1, scope: !193, file: !1, line: 89, type: !5)
!196 = !DILocalVariable(name: "ptr", scope: !197, file: !1, line: 91, type: !198)
!197 = distinct !DILexicalBlock(scope: !193, file: !1, line: 90, column: 1)
!198 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&i32", baseType: !5, size: 64)
!199 = !DILocation(line: 89, scope: !193)
!200 = !DILocation(line: 91, column: 9, scope: !197)
!201 = !DILocation(line: 91, column: 13, scope: !197)
!202 = !DILocation(line: 92, column: 6, scope: !197)
!203 = !DILocation(line: 92, column: 10, scope: !197)
!204 = distinct !DISubprogram(name: "break_and_continue", scope: !1, file: !1, line: 95, type: !52, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !205)
!205 = !{!206, !209}
!206 = !DILocalVariable(name: "i", scope: !207, file: !1, line: 97, type: !22)
!207 = distinct !DILexicalBlock(scope: !208, file: !1, line: 97, column: 9)
!208 = distinct !DILexicalBlock(scope: !204, file: !1, line: 96, column: 1)
!209 = !DILocalVariable(name: "j", scope: !208, file: !1, line: 111, type: !22)
!210 = !DILocation(line: 97, column: 9, scope: !207)
!211 = !DILocation(line: 97, column: 18, scope: !208)
!212 = !DILocation(line: 99, column: 12, scope: !213)
!213 = distinct !DILexicalBlock(scope: !207, file: !1, line: 98, column: 5)
!214 = !DILocation(line: 99, column: 14, scope: !213)
!215 = !DILocation(line: 101, column: 13, scope: !216)
!216 = distinct !DILexicalBlock(scope: !213, file: !1, line: 100, column: 9)
!217 = !DILocation(line: 103, column: 14, scope: !213)
!218 = !DILocation(line: 103, column: 16, scope: !213)
!219 = !DILocation(line: 105, column: 13, scope: !220)
!220 = distinct !DILexicalBlock(scope: !213, file: !1, line: 104, column: 9)
!221 = !DILocation(line: 108, column: 9, scope: !213)
!222 = !DILocation(line: 111, column: 9, scope: !208)
!223 = !DILocation(line: 111, column: 14, scope: !208)
!224 = !DILocation(line: 112, column: 11, scope: !208)
!225 = !DILocation(line: 112, column: 13, scope: !208)
!226 = !DILocation(line: 114, column: 12, scope: !227)
!227 = distinct !DILexicalBlock(scope: !208, file: !1, line: 113, column: 5)
!228 = !DILocation(line: 114, column: 14, scope: !227)
!229 = !DILocation(line: 114, column: 18, scope: !227)
!230 = !DILocation(line: 116, column: 13, scope: !231)
!231 = distinct !DILexicalBlock(scope: !227, file: !1, line: 115, column: 9)
!232 = !DILocation(line: 119, column: 12, scope: !227)
!233 = !DILocation(line: 119, column: 14, scope: !227)
!234 = !DILocation(line: 121, column: 13, scope: !235)
!235 = distinct !DILexicalBlock(scope: !227, file: !1, line: 120, column: 9)
!236 = !DILocation(line: 124, column: 9, scope: !227)
!237 = !DILocation(line: 126, column: 11, scope: !227)
!238 = distinct !DISubprogram(name: "ret1", scope: !1, file: !1, line: 130, type: !239, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !241)
!239 = !DISubroutineType(flags: DIFlagPrototyped, types: !240)
!240 = !{!5, !61, !61}
!241 = !{!242, !243, !244}
!242 = !DILocalVariable(name: "a", arg: 1, scope: !238, file: !1, line: 130, type: !61)
!243 = !DILocalVariable(name: "b", arg: 2, scope: !238, file: !1, line: 130, type: !61)
!244 = !DILocalVariable(name: "rv", scope: !245, file: !1, line: 132, type: !61)
!245 = distinct !DILexicalBlock(scope: !238, file: !1, line: 131, column: 1)
!246 = !DILocation(line: 130, scope: !238)
!247 = !DILocation(line: 132, column: 9, scope: !245)
!248 = !DILocation(line: 132, column: 14, scope: !245)
!249 = !DILocation(line: 132, column: 18, scope: !245)
!250 = !DILocation(line: 132, column: 16, scope: !245)
!251 = !DILocation(line: 132, column: 12, scope: !245)
!252 = !DILocation(line: 133, column: 12, scope: !245)
!253 = distinct !DISubprogram(name: "ret2", scope: !1, file: !1, line: 136, type: !254, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !256)
!254 = !DISubroutineType(flags: DIFlagPrototyped, types: !255)
!255 = !{!54, !5, !198}
!256 = !{!257, !258}
!257 = !DILocalVariable(name: "a", arg: 1, scope: !253, file: !1, line: 136, type: !5)
!258 = !DILocalVariable(name: "p", arg: 2, scope: !253, file: !1, line: 136, type: !198)
!259 = !DILocation(line: 136, scope: !253)
!260 = !DILocation(line: 138, column: 8, scope: !261)
!261 = distinct !DILexicalBlock(scope: !253, file: !1, line: 137, column: 1)
!262 = !DILocation(line: 138, column: 10, scope: !261)
!263 = !DILocation(line: 140, column: 9, scope: !264)
!264 = distinct !DILexicalBlock(scope: !261, file: !1, line: 139, column: 5)
!265 = !DILocation(line: 143, column: 6, scope: !261)
!266 = !DILocation(line: 143, column: 10, scope: !261)
!267 = !DILocation(line: 143, column: 8, scope: !261)
!268 = distinct !DISubprogram(name: "arrays", scope: !1, file: !1, line: 146, type: !269, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !275)
!269 = !DISubroutineType(flags: DIFlagPrototyped, types: !270)
!270 = !{!54, !271}
!271 = !DICompositeType(tag: DW_TAG_structure_type, name: "[]u16", size: 128, elements: !272)
!272 = !{!77, !273}
!273 = !DIDerivedType(tag: DW_TAG_member, name: "Data", baseType: !274, size: 64, align: 4, offset: 64)
!274 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&u16", baseType: !11, size: 64)
!275 = !{!276, !277}
!276 = !DILocalVariable(name: "a", arg: 1, scope: !268, file: !1, line: 146, type: !271)
!277 = !DILocalVariable(name: "n", scope: !278, file: !1, line: 148, type: !11)
!278 = distinct !DILexicalBlock(scope: !268, file: !1, line: 147, column: 1)
!279 = !DILocation(line: 146, scope: !268)
!280 = !DILocation(line: 148, column: 9, scope: !278)
!281 = !DILocation(line: 148, column: 13, scope: !278)
!282 = !DILocation(line: 148, column: 14, scope: !278)
!283 = !DILocation(line: 148, column: 11, scope: !278)
!284 = !DILocation(line: 149, column: 5, scope: !278)
!285 = !DILocation(line: 149, column: 6, scope: !278)
!286 = !DILocation(line: 149, column: 12, scope: !278)
!287 = !DILocation(line: 149, column: 14, scope: !278)
!288 = !DILocation(line: 149, column: 10, scope: !278)
!289 = distinct !DISubprogram(name: "forIndex", scope: !1, file: !1, line: 152, type: !290, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !296)
!290 = !DISubroutineType(flags: DIFlagPrototyped, types: !291)
!291 = !{!73, !292}
!292 = !DICompositeType(tag: DW_TAG_structure_type, name: "[]u32", size: 128, elements: !293)
!293 = !{!77, !294}
!294 = !DIDerivedType(tag: DW_TAG_member, name: "Data", baseType: !295, size: 64, align: 4, offset: 64)
!295 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "&u32", baseType: !69, size: 64)
!296 = !{!297, !298, !300, !302}
!297 = !DILocalVariable(name: "a", arg: 1, scope: !289, file: !1, line: 152, type: !292)
!298 = !DILocalVariable(name: "num", scope: !299, file: !1, line: 154, type: !73)
!299 = distinct !DILexicalBlock(scope: !289, file: !1, line: 153, column: 1)
!300 = !DILocalVariable(name: "x", scope: !301, file: !1, line: 155, type: !69)
!301 = distinct !DILexicalBlock(scope: !299, file: !1, line: 155, column: 9)
!302 = !DILocalVariable(name: "i", scope: !301, file: !1, line: 155, type: !73)
!303 = !DILocation(line: 152, scope: !289)
!304 = !DILocation(line: 154, column: 9, scope: !299)
!305 = !DILocation(line: 154, column: 19, scope: !299)
!306 = !DILocation(line: 155, column: 17, scope: !299)
!307 = !DILocation(line: 155, column: 9, scope: !301)
!308 = !DILocation(line: 155, column: 12, scope: !301)
!309 = !DILocation(line: 157, column: 16, scope: !310)
!310 = distinct !DILexicalBlock(scope: !301, file: !1, line: 156, column: 5)
!311 = !DILocation(line: 157, column: 20, scope: !310)
!312 = !DILocation(line: 157, column: 18, scope: !310)
!313 = !DILocation(line: 157, column: 13, scope: !310)
!314 = !DILocation(line: 160, column: 5, scope: !299)
!315 = distinct !DISubprogram(name: "casts", scope: !1, file: !1, line: 163, type: !123, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !316)
!316 = !{!317, !318}
!317 = !DILocalVariable(name: "a", arg: 1, scope: !315, file: !1, line: 163, type: !5)
!318 = !DILocalVariable(name: "x", scope: !319, file: !1, line: 165, type: !22)
!319 = distinct !DILexicalBlock(scope: !315, file: !1, line: 164, column: 1)
!320 = !DILocation(line: 163, scope: !315)
!321 = !DILocation(line: 165, column: 9, scope: !319)
!322 = !DILocation(line: 165, column: 23, scope: !319)
!323 = !DILocation(line: 165, column: 13, scope: !319)
!324 = !DILocation(line: 165, column: 11, scope: !319)
!325 = distinct !DISubprogram(name: "ftype1", scope: !1, file: !1, line: 168, type: !52, spFlags: DISPFlagDefinition, unit: !0)
!326 = distinct !DISubprogram(name: "ftype2", scope: !1, file: !1, line: 169, type: !327, spFlags: DISPFlagDefinition, unit: !0)
!327 = !DISubroutineType(flags: DIFlagPrototyped, types: !328)
!328 = !{!58}
!329 = !DILocation(line: 169, column: 18, scope: !330)
!330 = distinct !DILexicalBlock(scope: !326, file: !1, line: 169, column: 17)
!331 = distinct !DISubprogram(name: "function_type", scope: !1, file: !1, line: 171, type: !52, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !332)
!332 = !{!333, !337}
!333 = !DILocalVariable(name: "f1", scope: !334, file: !1, line: 173, type: !335)
!334 = distinct !DILexicalBlock(scope: !331, file: !1, line: 172, column: 1)
!335 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "fun()", baseType: !336, size: 64)
!336 = !DISubroutineType(types: !53)
!337 = !DILocalVariable(name: "f2", scope: !334, file: !1, line: 175, type: !338)
!338 = !DIDerivedType(tag: DW_TAG_pointer_type, name: "fun() bool", baseType: !339, size: 64)
!339 = !DISubroutineType(types: !328)
!340 = !DILocation(line: 173, column: 9, scope: !334)
!341 = !DILocation(line: 173, column: 12, scope: !334)
!342 = !DILocation(line: 174, column: 5, scope: !334)
!343 = !DILocation(line: 175, column: 9, scope: !334)
!344 = !DILocation(line: 175, column: 12, scope: !334)
!345 = !DILocation(line: 176, column: 5, scope: !334)
!346 = distinct !DISubprogram(name: "floats", scope: !1, file: !1, line: 179, type: !347, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !350)
!347 = !DISubroutineType(flags: DIFlagPrototyped, types: !348)
!348 = !{!349, !349}
!349 = !DIBasicType(name: "f64", size: 64, encoding: DW_ATE_float)
!350 = !{!351, !352, !355}
!351 = !DILocalVariable(name: "a", arg: 1, scope: !346, file: !1, line: 179, type: !349)
!352 = !DILocalVariable(name: "x", scope: !353, file: !1, line: 181, type: !354)
!353 = distinct !DILexicalBlock(scope: !346, file: !1, line: 180, column: 1)
!354 = !DIBasicType(name: "f32", size: 32, encoding: DW_ATE_float)
!355 = !DILocalVariable(name: "y", scope: !353, file: !1, line: 182, type: !349)
!356 = !DILocation(line: 179, scope: !346)
!357 = !DILocation(line: 181, column: 9, scope: !353)
!358 = !DILocation(line: 181, column: 15, scope: !353)
!359 = !DILocation(line: 182, column: 9, scope: !353)
!360 = !DILocation(line: 182, column: 15, scope: !353)
!361 = !DILocation(line: 183, column: 12, scope: !353)
!362 = !DILocation(line: 183, column: 16, scope: !353)
!363 = !DILocation(line: 183, column: 20, scope: !353)
!364 = !DILocation(line: 183, column: 18, scope: !353)
!365 = !DILocation(line: 183, column: 14, scope: !353)
