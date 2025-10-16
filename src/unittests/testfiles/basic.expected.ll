$module_id
$source_filename
$target_datalayout
$target_triple

%str = type { i64, ptr }
%UnitType = type {}
%"RangeClosed'i32'" = type { i32, i32 }
%"RangeHalfOpen'u32'" = type { i32, i32 }
%"[]i32" = type { i64, ptr }
%"[]i8" = type { i64, ptr }
%"[]i16" = type { i64, ptr }
%"RangeClosed'i8'" = type { i8, i8 }
%Test2 = type { %Test1, i32 }
%Test1 = type { i32, i1, %str }
%EmptyType = type {}
%SubscriptTest1 = type { %SubscriptTest2 }
%SubscriptTest2 = type { %str }
%ArrayTest = type { %"[]i32" }
%"[][]i32" = type { i64, ptr }
%"[]u8" = type { i64, ptr }
%"RangeClosed'u32'" = type { i32, i32 }
%"RangeClosed'i16'" = type { i16, i16 }
%"<struct29>" = type { i32, i16 }
%TypesTest = type {}
%"[]TypesTest" = type { i64, ptr }

@strData0 = constant [0 x i8] zeroinitializer
@strStruct0 = constant %str { i64 0, ptr @strData0 }
@strData1 = constant [15 x i8] c"Is this a test?"
@strStruct1 = constant %str { i64 15, ptr @strData1 }
@strData2 = constant [6 x i8] c"\09\0D\0A\\\22'"
@strStruct2 = constant %str { i64 6, ptr @strData2 }
@strData3 = constant [5 x i8] c"caf\C3\A9"
@strStruct3 = constant %str { i64 5, ptr @strData3 }
@strData4 = constant [3 x i8] c"JBW"
@strStruct4 = constant %str { i64 3, ptr @strData4 }
@strData5 = constant [11 x i8] c"\0A0\CF\80\E2\82\BF\F0\9F\98\80"
@strStruct5 = constant %str { i64 11, ptr @strData5 }
@strData6 = constant [3 x i8] c"abc"
@strStruct6 = constant %str { i64 3, ptr @strData6 }
@strData7 = constant [4 x i8] c"cdef"
@strStruct7 = constant %str { i64 4, ptr @strData7 }
@strData8 = constant [3 x i8] c"cde"
@strStruct8 = constant %str { i64 3, ptr @strData8 }
@strData9 = constant [2 x i8] c"ij"
@strStruct9 = constant %str { i64 2, ptr @strData9 }

declare %UnitType @extern1()

declare i64 @extern2(i32, i1)

; Function Attrs: noinline nounwind optnone
define i32 @noArgs() #0 {
entry:
  ret i32 42
}

; Function Attrs: noinline nounwind optnone
define i32 @oneArg(i32 %x) #0 {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  %x2 = load i32, ptr %x1, align 4
  %add = add i32 %x2, 2
  ret i32 %add
}

; Function Attrs: noinline nounwind optnone
define i32 @integerLiterals(i32 %x) #0 {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  %x2 = load i32, ptr %x1, align 4
  %add = add i32 5, %x2
  %add3 = add i32 %add, 248
  %x4 = load i32, ptr %x1, align 4
  %add5 = add i32 %add3, %x4
  %add6 = add i32 %add5, 123
  %x7 = load i32, ptr %x1, align 4
  %add8 = add i32 %add6, %x7
  %add9 = add i32 %add8, 10846099
  ret i32 %add9
}

; Function Attrs: noinline nounwind optnone
define i32 @twoArgs(i32 %arg1, i32 %arg2) #0 {
entry:
  %arg22 = alloca i32, align 4
  %arg11 = alloca i32, align 4
  store i32 %arg1, ptr %arg11, align 4
  store i32 %arg2, ptr %arg22, align 4
  %arg13 = load i32, ptr %arg11, align 4
  %arg24 = load i32, ptr %arg22, align 4
  %sub = sub i32 %arg13, %arg24
  ret i32 %sub
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder1(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  store i32 %d, ptr %d4, align 4
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %c7 = load i32, ptr %c3, align 4
  %mul = mul i32 %b6, %c7
  %add = add i32 %a5, %mul
  %d8 = load i32, ptr %d4, align 4
  %sub = sub i32 %add, %d8
  ret i32 %sub
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder2(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  store i32 %d, ptr %d4, align 4
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %c7 = load i32, ptr %c3, align 4
  %mul = mul i32 %b6, %c7
  %add = add i32 %a5, %mul
  %d8 = load i32, ptr %d4, align 4
  %a9 = load i32, ptr %a1, align 4
  %div = sdiv i32 %d8, %a9
  %sub = sub i32 %add, %div
  %b10 = load i32, ptr %b2, align 4
  %rem = srem i32 %b10, 42
  %add11 = add i32 %sub, %rem
  ret i32 %add11
}

; Function Attrs: noinline nounwind optnone
define i1 @opOrder3(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  store i32 %d, ptr %d4, align 4
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %c7 = load i32, ptr %c3, align 4
  %d8 = load i32, ptr %d4, align 4
  %mul = mul i32 %c7, %d8
  %add = add i32 %b6, %mul
  %cmpeq = icmp eq i32 %a5, %add
  ret i1 %cmpeq
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder4(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e, i32 %f) #0 {
entry:
  %f6 = alloca i32, align 4
  %e5 = alloca i32, align 4
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  store i32 %d, ptr %d4, align 4
  store i32 %e, ptr %e5, align 4
  store i32 %f, ptr %f6, align 4
  %a7 = load i32, ptr %a1, align 4
  %b8 = load i32, ptr %b2, align 4
  %c9 = load i32, ptr %c3, align 4
  %d10 = load i32, ptr %d4, align 4
  %e11 = load i32, ptr %e5, align 4
  %shl = shl i32 %d10, %e11
  %f12 = load i32, ptr %f6, align 4
  %lshr = lshr i32 %shl, %f12
  %a13 = load i32, ptr %a1, align 4
  %ashr = ashr i32 %lshr, %a13
  %bitand = and i32 %c9, %ashr
  %bitxor = xor i32 %b8, %bitand
  %bitor = or i32 %a7, %bitxor
  ret i32 %bitor
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder5(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  store i32 %d, ptr %d4, align 4
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %add = add i32 %a5, %b6
  %neg = sub i32 0, %add
  %a7 = load i32, ptr %a1, align 4
  %b8 = load i32, ptr %b2, align 4
  %c9 = load i32, ptr %c3, align 4
  %d10 = load i32, ptr %d4, align 4
  %call = call i32 @opOrder6(i32 %a7, i32 %b8, i32 %c9, i32 %d10)
  %add11 = add i32 %neg, %call
  %d12 = load i32, ptr %d4, align 4
  %mul = mul i32 %add11, %d12
  ret i32 %mul
}

; Function Attrs: noinline nounwind optnone
define i32 @opOrder6(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  store i32 %d, ptr %d4, align 4
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %bitor = or i32 %a5, %b6
  %c7 = load i32, ptr %c3, align 4
  %d8 = load i32, ptr %d4, align 4
  %bitand = and i32 %c7, %d8
  %bitxor = xor i32 %bitor, %bitand
  ret i32 %bitxor
}

; Function Attrs: noinline nounwind optnone
define i32 @negatives(i32 %a, i32 %b, i32 %c) #0 {
entry:
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  %a4 = load i32, ptr %a1, align 4
  %neg = sub i32 0, %a4
  %b5 = load i32, ptr %b2, align 4
  %neg6 = sub i32 0, %b5
  %c7 = load i32, ptr %c3, align 4
  %neg8 = sub i32 0, %c7
  %add = add i32 2, %neg8
  %call = call i32 @oneArg(i32 %add)
  %neg9 = sub i32 0, %call
  %mul = mul i32 %neg6, %neg9
  %sub = sub i32 %neg, %mul
  ret i32 %sub
}

; Function Attrs: noinline nounwind optnone
define i32 @not(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %not = xor i32 %a3, -1
  %b4 = load i32, ptr %b2, align 4
  %not5 = xor i32 %b4, -1
  %bitand = and i32 %not, %not5
  %call = call i32 @noArgs()
  %not6 = xor i32 %call, -1
  %bitor = or i32 %bitand, %not6
  ret i32 %bitor
}

; Function Attrs: noinline nounwind optnone
define i1 @equal(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %cmpeq = icmp eq i32 %a3, %b4
  ret i1 %cmpeq
}

; Function Attrs: noinline nounwind optnone
define i1 @notEqual(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %cmpne = icmp ne i32 %a3, %b4
  ret i1 %cmpne
}

; Function Attrs: noinline nounwind optnone
define i1 @lessThan(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %cmplt = icmp slt i32 %a3, %b4
  ret i1 %cmplt
}

; Function Attrs: noinline nounwind optnone
define i1 @lessThanOrEqual(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %cmple = icmp sle i32 %a3, %b4
  ret i1 %cmple
}

; Function Attrs: noinline nounwind optnone
define i1 @greaterThan(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %cmpgt = icmp sgt i32 %a3, %b4
  ret i1 %cmpgt
}

; Function Attrs: noinline nounwind optnone
define i1 @greaterThanOrEqual(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %cmpge = icmp sge i32 %a3, %b4
  ret i1 %cmpge
}

; Function Attrs: noinline nounwind optnone
define i1 @logicalAnd2(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %cmpne = icmp ne i32 %x4, 0
  br i1 %cmpne, label %andtrue, label %andmerge

andtrue:                                          ; preds = %entry
  %y5 = load i32, ptr %y2, align 4
  %z6 = load i32, ptr %z3, align 4
  %add = add i32 %z6, 5
  %cmplt = icmp slt i32 %y5, %add
  br label %andmerge

andmerge:                                         ; preds = %andtrue, %entry
  %andphi = phi i1 [ %cmplt, %andtrue ], [ false, %entry ]
  ret i1 %andphi
}

; Function Attrs: noinline nounwind optnone
define i1 @logicalAnd3(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %andtrue, label %andmerge

andtrue:                                          ; preds = %entry
  %y5 = load i32, ptr %y2, align 4
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %andmerge

andmerge:                                         ; preds = %andtrue, %entry
  %andphi = phi i1 [ %cmpeq6, %andtrue ], [ false, %entry ]
  br i1 %andphi, label %andtrue7, label %andmerge10

andtrue7:                                         ; preds = %andmerge
  %z8 = load i32, ptr %z3, align 4
  %cmpeq9 = icmp eq i32 %z8, 2
  br label %andmerge10

andmerge10:                                       ; preds = %andtrue7, %andmerge
  %andphi11 = phi i1 [ %cmpeq9, %andtrue7 ], [ false, %andmerge ]
  ret i1 %andphi11
}

; Function Attrs: noinline nounwind optnone
define i1 @logicalOr2(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %z4 = load i32, ptr %z3, align 4
  %cmpeq = icmp eq i32 %z4, 0
  br i1 %cmpeq, label %ormerge, label %orfalse

orfalse:                                          ; preds = %entry
  %x5 = load i32, ptr %x1, align 4
  %y6 = load i32, ptr %y2, align 4
  %z7 = load i32, ptr %z3, align 4
  %div = sdiv i32 %y6, %z7
  %add = add i32 %x5, %div
  %cmplt = icmp slt i32 %add, 100
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %entry
  %orphi = phi i1 [ %cmplt, %orfalse ], [ true, %entry ]
  ret i1 %orphi
}

; Function Attrs: noinline nounwind optnone
define i1 @logicalOr3(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %ormerge, label %orfalse

orfalse:                                          ; preds = %entry
  %y5 = load i32, ptr %y2, align 4
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %entry
  %orphi = phi i1 [ %cmpeq6, %orfalse ], [ true, %entry ]
  br i1 %orphi, label %ormerge10, label %orfalse7

orfalse7:                                         ; preds = %ormerge
  %z8 = load i32, ptr %z3, align 4
  %cmpeq9 = icmp eq i32 %z8, 2
  br label %ormerge10

ormerge10:                                        ; preds = %orfalse7, %ormerge
  %orphi11 = phi i1 [ %cmpeq9, %orfalse7 ], [ true, %ormerge ]
  ret i1 %orphi11
}

; Function Attrs: noinline nounwind optnone
define %UnitType @assignOps(i8 %a, i16 %b, i32 %c) #0 {
entry:
  %c3 = alloca i32, align 4
  %b2 = alloca i16, align 2
  %a1 = alloca i8, align 1
  store i8 %a, ptr %a1, align 1
  store i16 %b, ptr %b2, align 2
  store i32 %c, ptr %c3, align 4
  %a4 = load i8, ptr %a1, align 1
  %signext = sext i8 %a4 to i32
  %load = load i32, ptr %c3, align 4
  %add = add i32 %load, %signext
  store i32 %add, ptr %c3, align 4
  %a5 = load i8, ptr %a1, align 1
  %signext6 = sext i8 %a5 to i16
  %b7 = load i16, ptr %b2, align 2
  %add8 = add i16 %signext6, %b7
  %signext9 = sext i16 %add8 to i32
  %load10 = load i32, ptr %c3, align 4
  %sub = sub i32 %load10, %signext9
  store i32 %sub, ptr %c3, align 4
  %a11 = load i8, ptr %a1, align 1
  %signext12 = sext i8 %a11 to i32
  %load13 = load i32, ptr %c3, align 4
  %mul = mul i32 %load13, %signext12
  store i32 %mul, ptr %c3, align 4
  %a14 = load i8, ptr %a1, align 1
  %signext15 = sext i8 %a14 to i16
  %b16 = load i16, ptr %b2, align 2
  %add17 = add i16 %signext15, %b16
  %signext18 = sext i16 %add17 to i32
  %load19 = load i32, ptr %c3, align 4
  %div = sdiv i32 %load19, %signext18
  store i32 %div, ptr %c3, align 4
  %a20 = load i8, ptr %a1, align 1
  %signext21 = sext i8 %a20 to i32
  %load22 = load i32, ptr %c3, align 4
  %rem = srem i32 %load22, %signext21
  store i32 %rem, ptr %c3, align 4
  %a23 = load i8, ptr %a1, align 1
  %signext24 = sext i8 %a23 to i16
  %b25 = load i16, ptr %b2, align 2
  %add26 = add i16 %signext24, %b25
  %cast = sext i16 %add26 to i32
  %load27 = load i32, ptr %c3, align 4
  %shl = shl i32 %load27, %cast
  store i32 %shl, ptr %c3, align 4
  %load28 = load i32, ptr %c3, align 4
  %lshr = lshr i32 %load28, 2
  store i32 %lshr, ptr %c3, align 4
  %load29 = load i32, ptr %c3, align 4
  %ashr = ashr i32 %load29, 9
  store i32 %ashr, ptr %c3, align 4
  %a30 = load i8, ptr %a1, align 1
  %signext31 = sext i8 %a30 to i16
  %b32 = load i16, ptr %b2, align 2
  %add33 = add i16 %signext31, %b32
  %signext34 = sext i16 %add33 to i32
  %load35 = load i32, ptr %c3, align 4
  %bitand = and i32 %load35, %signext34
  store i32 %bitand, ptr %c3, align 4
  %a36 = load i8, ptr %a1, align 1
  %signext37 = sext i8 %a36 to i32
  %load38 = load i32, ptr %c3, align 4
  %bitxor = xor i32 %load38, %signext37
  store i32 %bitxor, ptr %c3, align 4
  %a39 = load i8, ptr %a1, align 1
  %signext40 = sext i8 %a39 to i16
  %b41 = load i16, ptr %b2, align 2
  %add42 = add i16 %signext40, %b41
  %signext43 = sext i16 %add42 to i32
  %load44 = load i32, ptr %c3, align 4
  %bitor = or i32 %load44, %signext43
  store i32 %bitor, ptr %c3, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i32 @nestedCall(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %call = call i32 @twoArgs(i32 0, i32 %x4)
  %y5 = load i32, ptr %y2, align 4
  %add = add i32 %y5, 42
  %z6 = load i32, ptr %z3, align 4
  %sub = sub i32 %z6, 57
  %call7 = call i32 @twoArgs(i32 %add, i32 %sub)
  %add8 = add i32 %call, %call7
  ret i32 %add8
}

; Function Attrs: noinline nounwind optnone
define i1 @call_function_defined_later(i1 %a, i1 %b) #0 {
entry:
  %b2 = alloca i1, align 1
  %a1 = alloca i1, align 1
  store i1 %a, ptr %a1, align 1
  store i1 %b, ptr %b2, align 1
  %a3 = load i1, ptr %a1, align 1
  %b4 = load i1, ptr %b2, align 1
  %call = call i1 @types_bool(i1 %a3, i1 %b4)
  ret i1 %call
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types_unit(i64 %number) #0 {
entry:
  %number1 = alloca i64, align 8
  store i64 %number, ptr %number1, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types_unit2() #0 {
entry:
  %call = call %UnitType @types_unit(i64 123)
  ret %UnitType %call
}

; Function Attrs: noinline nounwind optnone
define i1 @types_bool(i1 %a, i1 %b) #0 {
entry:
  %b2 = alloca i1, align 1
  %a1 = alloca i1, align 1
  store i1 %a, ptr %a1, align 1
  store i1 %b, ptr %b2, align 1
  %a3 = load i1, ptr %a1, align 1
  %b4 = load i1, ptr %b2, align 1
  %bitor = or i1 %a3, %b4
  ret i1 %bitor
}

; Function Attrs: noinline nounwind optnone
define i8 @types_i8(i8 %n1, i8 %n2) #0 {
entry:
  %n22 = alloca i8, align 1
  %n11 = alloca i8, align 1
  store i8 %n1, ptr %n11, align 1
  store i8 %n2, ptr %n22, align 1
  %n13 = load i8, ptr %n11, align 1
  %n24 = load i8, ptr %n22, align 1
  %add = add i8 %n13, %n24
  ret i8 %add
}

; Function Attrs: noinline nounwind optnone
define i16 @types_i16(i16 %n1, i16 %n2) #0 {
entry:
  %n22 = alloca i16, align 2
  %n11 = alloca i16, align 2
  store i16 %n1, ptr %n11, align 2
  store i16 %n2, ptr %n22, align 2
  %n13 = load i16, ptr %n11, align 2
  %n24 = load i16, ptr %n22, align 2
  %sub = sub i16 %n13, %n24
  ret i16 %sub
}

; Function Attrs: noinline nounwind optnone
define i32 @types_i32(i32 %n1, i32 %n2) #0 {
entry:
  %n22 = alloca i32, align 4
  %n11 = alloca i32, align 4
  store i32 %n1, ptr %n11, align 4
  store i32 %n2, ptr %n22, align 4
  %n13 = load i32, ptr %n11, align 4
  %n24 = load i32, ptr %n22, align 4
  %mul = mul i32 %n13, %n24
  ret i32 %mul
}

; Function Attrs: noinline nounwind optnone
define i64 @types_i64(i64 %n1, i64 %n2) #0 {
entry:
  %n22 = alloca i64, align 8
  %n11 = alloca i64, align 8
  store i64 %n1, ptr %n11, align 8
  store i64 %n2, ptr %n22, align 8
  %n13 = load i64, ptr %n11, align 8
  %n24 = load i64, ptr %n22, align 8
  %div = sdiv i64 %n13, %n24
  ret i64 %div
}

; Function Attrs: noinline nounwind optnone
define i64 @types_isize(i64 %n1, i64 %n2) #0 {
entry:
  %n22 = alloca i64, align 8
  %n11 = alloca i64, align 8
  store i64 %n1, ptr %n11, align 8
  store i64 %n2, ptr %n22, align 8
  %n13 = load i64, ptr %n11, align 8
  %n24 = load i64, ptr %n22, align 8
  %rem = srem i64 %n13, %n24
  ret i64 %rem
}

; Function Attrs: noinline nounwind optnone
define i8 @types_u8(i8 %n1, i8 %n2) #0 {
entry:
  %n22 = alloca i8, align 1
  %n11 = alloca i8, align 1
  store i8 %n1, ptr %n11, align 1
  store i8 %n2, ptr %n22, align 1
  %n13 = load i8, ptr %n11, align 1
  %n24 = load i8, ptr %n22, align 1
  %rem = urem i8 %n13, %n24
  ret i8 %rem
}

; Function Attrs: noinline nounwind optnone
define i16 @types_u16(i16 %n1, i16 %n2) #0 {
entry:
  %n22 = alloca i16, align 2
  %n11 = alloca i16, align 2
  store i16 %n1, ptr %n11, align 2
  store i16 %n2, ptr %n22, align 2
  %n13 = load i16, ptr %n11, align 2
  %n24 = load i16, ptr %n22, align 2
  %div = udiv i16 %n13, %n24
  ret i16 %div
}

; Function Attrs: noinline nounwind optnone
define i32 @types_u32(i32 %n1, i32 %n2) #0 {
entry:
  %n22 = alloca i32, align 4
  %n11 = alloca i32, align 4
  store i32 %n1, ptr %n11, align 4
  store i32 %n2, ptr %n22, align 4
  %n13 = load i32, ptr %n11, align 4
  %n24 = load i32, ptr %n22, align 4
  %mul = mul i32 %n13, %n24
  ret i32 %mul
}

; Function Attrs: noinline nounwind optnone
define i64 @types_u64(i64 %n1, i64 %n2) #0 {
entry:
  %n22 = alloca i64, align 8
  %n11 = alloca i64, align 8
  store i64 %n1, ptr %n11, align 8
  store i64 %n2, ptr %n22, align 8
  %n13 = load i64, ptr %n11, align 8
  %n24 = load i64, ptr %n22, align 8
  %sub = sub i64 %n13, %n24
  ret i64 %sub
}

; Function Attrs: noinline nounwind optnone
define i64 @types_usize(i64 %n1, i64 %n2) #0 {
entry:
  %n22 = alloca i64, align 8
  %n11 = alloca i64, align 8
  store i64 %n1, ptr %n11, align 8
  store i64 %n2, ptr %n22, align 8
  %n13 = load i64, ptr %n11, align 8
  %n24 = load i64, ptr %n22, align 8
  %add = add i64 %n13, %n24
  ret i64 %add
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types_str() #0 {
entry:
  %dup2 = alloca %str, align 8
  %dup1 = alloca %str, align 8
  %s6 = alloca %str, align 8
  %s5 = alloca %str, align 8
  %s4 = alloca %str, align 8
  %s3 = alloca %str, align 8
  %s2 = alloca %str, align 8
  %s1 = alloca %str, align 8
  %load = load %str, ptr @strStruct0, align 8
  store %str %load, ptr %s1, align 8
  %load1 = load %str, ptr @strStruct1, align 8
  store %str %load1, ptr %s2, align 8
  %load2 = load %str, ptr @strStruct2, align 8
  store %str %load2, ptr %s3, align 8
  %load3 = load %str, ptr @strStruct3, align 8
  store %str %load3, ptr %s4, align 8
  %load4 = load %str, ptr @strStruct4, align 8
  store %str %load4, ptr %s5, align 8
  %load5 = load %str, ptr @strStruct5, align 8
  store %str %load5, ptr %s6, align 8
  %load6 = load %str, ptr @strStruct6, align 8
  store %str %load6, ptr %dup1, align 8
  %load7 = load %str, ptr @strStruct6, align 8
  store %str %load7, ptr %dup2, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types_range(i1 %b) #0 {
entry:
  %r1 = alloca %"RangeClosed'i32'", align 8
  %hr = alloca %"RangeHalfOpen'u32'", align 8
  %cr = alloca %"RangeClosed'i32'", align 8
  %y2 = alloca i32, align 4
  %y1 = alloca i32, align 4
  %x2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  %b1 = alloca i1, align 1
  store i1 %b, ptr %b1, align 1
  store i32 10, ptr %x1, align 4
  store i32 13, ptr %x2, align 4
  store i32 25, ptr %y1, align 4
  store i32 39, ptr %y2, align 4
  %x12 = load i32, ptr %x1, align 4
  %rng = insertvalue %"RangeClosed'i32'" { i32 0, i32 undef }, i32 %x12, 1
  store %"RangeClosed'i32'" %rng, ptr %cr, align 4
  %x13 = load i32, ptr %x1, align 4
  %x24 = load i32, ptr %x2, align 4
  %rng5 = insertvalue %"RangeClosed'i32'" undef, i32 %x13, 0
  %rng6 = insertvalue %"RangeClosed'i32'" %rng5, i32 %x24, 1
  store %"RangeClosed'i32'" %rng6, ptr %cr, align 4
  store %"RangeClosed'i32'" { i32 0, i32 1 }, ptr %cr, align 4
  %x27 = load i32, ptr %x2, align 4
  %neg = sub i32 0, %x27
  %x18 = load i32, ptr %x1, align 4
  %x29 = load i32, ptr %x2, align 4
  %add = add i32 %x18, %x29
  %rng10 = insertvalue %"RangeClosed'i32'" undef, i32 %neg, 0
  %rng11 = insertvalue %"RangeClosed'i32'" %rng10, i32 %add, 1
  store %"RangeClosed'i32'" %rng11, ptr %cr, align 4
  %y112 = load i32, ptr %y1, align 4
  %rng13 = insertvalue %"RangeHalfOpen'u32'" { i32 0, i32 undef }, i32 %y112, 1
  store %"RangeHalfOpen'u32'" %rng13, ptr %hr, align 4
  %y114 = load i32, ptr %y1, align 4
  %y215 = load i32, ptr %y2, align 4
  %rng16 = insertvalue %"RangeHalfOpen'u32'" undef, i32 %y114, 0
  %rng17 = insertvalue %"RangeHalfOpen'u32'" %rng16, i32 %y215, 1
  store %"RangeHalfOpen'u32'" %rng17, ptr %hr, align 4
  store %"RangeHalfOpen'u32'" { i32 0, i32 1 }, ptr %hr, align 4
  %x118 = load i32, ptr %x1, align 4
  %rng19 = insertvalue %"RangeClosed'i32'" { i32 0, i32 undef }, i32 %x118, 1
  store %"RangeClosed'i32'" %rng19, ptr %r1, align 4
  %b20 = load i1, ptr %b1, align 1
  br i1 %b20, label %if, label %else

if:                                               ; preds = %entry
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %"RangeClosed'i32'" [ { i32 0, i32 3 }, %if ], [ { i32 100, i32 200 }, %else ]
  store %"RangeClosed'i32'" %phi, ptr %r1, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i64 @str_member1() #0 {
entry:
  %size = alloca i64, align 8
  %s = alloca %str, align 8
  %load = load %str, ptr @strStruct6, align 8
  store %str %load, ptr %s, align 8
  %s1 = load %str, ptr %s, align 8
  %mber = extractvalue %str %s1, 0
  store i64 %mber, ptr %size, align 8
  %size2 = load i64, ptr %size, align 8
  ret i64 %size2
}

; Function Attrs: noinline nounwind optnone
define i64 @str_member2(i32 %x) #0 {
entry:
  %size = alloca i64, align 8
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  %x2 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x2, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %load = load %str, ptr @strStruct6, align 8
  br label %merge

else:                                             ; preds = %entry
  %load3 = load %str, ptr @strStruct3, align 8
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %str [ %load, %if ], [ %load3, %else ]
  %mber = extractvalue %str %phi, 0
  store i64 %mber, ptr %size, align 8
  %size4 = load i64, ptr %size, align 8
  ret i64 %size4
}

; Function Attrs: noinline nounwind optnone
define i8 @types_add_literal(i8 %num) #0 {
entry:
  %num1 = alloca i8, align 1
  store i8 %num, ptr %num1, align 1
  %num2 = load i8, ptr %num1, align 1
  %add = add i8 %num2, 1
  ret i8 %add
}

; Function Attrs: noinline nounwind optnone
define i64 @sign_extend_bin_op(i8 %a, i16 %b, i64 %c, i32 %d) #0 {
entry:
  %d4 = alloca i32, align 4
  %c3 = alloca i64, align 8
  %b2 = alloca i16, align 2
  %a1 = alloca i8, align 1
  store i8 %a, ptr %a1, align 1
  store i16 %b, ptr %b2, align 2
  store i64 %c, ptr %c3, align 8
  store i32 %d, ptr %d4, align 4
  %a5 = load i8, ptr %a1, align 1
  %signext = sext i8 %a5 to i16
  %b6 = load i16, ptr %b2, align 2
  %mul = mul i16 %signext, %b6
  %signext7 = sext i16 %mul to i64
  %c8 = load i64, ptr %c3, align 8
  %div = sdiv i64 %signext7, %c8
  %d9 = load i32, ptr %d4, align 4
  %signext10 = sext i32 %d9 to i64
  %rem = srem i64 %div, %signext10
  ret i64 %rem
}

; Function Attrs: noinline nounwind optnone
define i64 @sign_extend_ret(i32 %param) #0 {
entry:
  %param1 = alloca i32, align 4
  store i32 %param, ptr %param1, align 4
  %param2 = load i32, ptr %param1, align 4
  %add = add i32 %param2, 5
  %signext = sext i32 %add to i64
  ret i64 %signext
}

; Function Attrs: noinline nounwind optnone
define i64 @sign_extend_call(i8 %param) #0 {
entry:
  %param1 = alloca i8, align 1
  store i8 %param, ptr %param1, align 1
  %param2 = load i8, ptr %param1, align 1
  %signext = sext i8 %param2 to i32
  %call = call i64 @sign_extend_ret(i32 %signext)
  ret i64 %call
}

; Function Attrs: noinline nounwind optnone
define i32 @basicBranch(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %add = add i32 %x4, 1
  %y5 = load i32, ptr %y2, align 4
  %div = sdiv i32 %y5, 2
  %cmpeq = icmp eq i32 %add, %div
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %z6 = load i32, ptr %z3, align 4
  %mul = mul i32 %z6, 10
  br label %merge

else:                                             ; preds = %entry
  %z7 = load i32, ptr %z3, align 4
  %neg = sub i32 0, %z7
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %mul, %if ], [ %neg, %else ]
  ret i32 %phi
}

; Function Attrs: noinline nounwind optnone
define i64 @signExtendBranch(i1 %b, i16 %x, i16 %y) #0 {
entry:
  %y3 = alloca i16, align 2
  %x2 = alloca i16, align 2
  %b1 = alloca i1, align 1
  store i1 %b, ptr %b1, align 1
  store i16 %x, ptr %x2, align 2
  store i16 %y, ptr %y3, align 2
  %b4 = load i1, ptr %b1, align 1
  br i1 %b4, label %if, label %else

if:                                               ; preds = %entry
  %x5 = load i16, ptr %x2, align 2
  %y6 = load i16, ptr %y3, align 2
  %call = call i16 @types_i16(i16 %x5, i16 %y6)
  %signext = sext i16 %call to i32
  br label %merge

else:                                             ; preds = %entry
  %x7 = load i16, ptr %x2, align 2
  %signext8 = sext i16 %x7 to i32
  %y9 = load i16, ptr %y3, align 2
  %signext10 = sext i16 %y9 to i32
  %call11 = call i32 @types_i32(i32 %signext8, i32 %signext10)
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %signext, %if ], [ %call11, %else ]
  %signext12 = sext i32 %phi to i64
  ret i64 %signext12
}

; Function Attrs: noinline nounwind optnone
define i32 @nestedBranches(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, ptr %y2, align 4
  %z6 = load i32, ptr %z3, align 4
  %add = add i32 %y5, %z6
  br label %merge15

else:                                             ; preds = %entry
  %x7 = load i32, ptr %x1, align 4
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, ptr %y2, align 4
  %z11 = load i32, ptr %z3, align 4
  %sub = sub i32 %y10, %z11
  br label %merge

else12:                                           ; preds = %else
  %y13 = load i32, ptr %y2, align 4
  %z14 = load i32, ptr %z3, align 4
  %mul = mul i32 %y13, %z14
  br label %merge

merge:                                            ; preds = %else12, %if9
  %phi = phi i32 [ %sub, %if9 ], [ %mul, %else12 ]
  br label %merge15

merge15:                                          ; preds = %merge, %if
  %phi16 = phi i32 [ %add, %if ], [ %phi, %merge ]
  ret i32 %phi16
}

; Function Attrs: noinline nounwind optnone
define i32 @elseIfBranches(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, ptr %y2, align 4
  %z6 = load i32, ptr %z3, align 4
  %add = add i32 %y5, %z6
  br label %merge21

else:                                             ; preds = %entry
  %x7 = load i32, ptr %x1, align 4
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, ptr %y2, align 4
  %z11 = load i32, ptr %z3, align 4
  %sub = sub i32 %y10, %z11
  br label %merge19

else12:                                           ; preds = %else
  %x13 = load i32, ptr %x1, align 4
  %cmpeq14 = icmp eq i32 %x13, 2
  br i1 %cmpeq14, label %if15, label %else18

if15:                                             ; preds = %else12
  %y16 = load i32, ptr %y2, align 4
  %z17 = load i32, ptr %z3, align 4
  %mul = mul i32 %y16, %z17
  br label %merge

else18:                                           ; preds = %else12
  br label %merge

merge:                                            ; preds = %else18, %if15
  %phi = phi i32 [ %mul, %if15 ], [ 42, %else18 ]
  br label %merge19

merge19:                                          ; preds = %merge, %if9
  %phi20 = phi i32 [ %sub, %if9 ], [ %phi, %merge ]
  br label %merge21

merge21:                                          ; preds = %merge19, %if
  %phi22 = phi i32 [ %add, %if ], [ %phi20, %merge19 ]
  ret i32 %phi22
}

; Function Attrs: noinline nounwind optnone
define i32 @noElseBranches(i32 %x, i32 %y) #0 {
entry:
  %z = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 0, ptr %z, align 4
  %x3 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x3, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %x4 = load i32, ptr %x1, align 4
  %y5 = load i32, ptr %y2, align 4
  %add = add i32 %x4, %y5
  store i32 %add, ptr %z, align 4
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %UnitType [ zeroinitializer, %if ], [ zeroinitializer, %else ]
  %y6 = load i32, ptr %y2, align 4
  %cmplt = icmp slt i32 %y6, -5
  br i1 %cmplt, label %if7, label %else8

if7:                                              ; preds = %merge
  %load = load i32, ptr %z, align 4
  %mul = mul i32 %load, 2
  store i32 %mul, ptr %z, align 4
  br label %merge15

else8:                                            ; preds = %merge
  %y9 = load i32, ptr %y2, align 4
  %cmpgt = icmp sgt i32 %y9, 5
  br i1 %cmpgt, label %if10, label %else12

if10:                                             ; preds = %else8
  %load11 = load i32, ptr %z, align 4
  %div = sdiv i32 %load11, 2
  store i32 %div, ptr %z, align 4
  br label %merge13

else12:                                           ; preds = %else8
  br label %merge13

merge13:                                          ; preds = %else12, %if10
  %phi14 = phi %UnitType [ zeroinitializer, %if10 ], [ zeroinitializer, %else12 ]
  br label %merge15

merge15:                                          ; preds = %merge13, %if7
  %phi16 = phi %UnitType [ zeroinitializer, %if7 ], [ %phi14, %merge13 ]
  %z17 = load i32, ptr %z, align 4
  ret i32 %z17
}

; Function Attrs: noinline nounwind optnone
define i32 @elseIfBranchesNesting(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, ptr %y2, align 4
  %z6 = load i32, ptr %z3, align 4
  %add = add i32 %y5, %z6
  br label %merge43

else:                                             ; preds = %entry
  %x7 = load i32, ptr %x1, align 4
  %cmpeq8 = icmp eq i32 %x7, 1
  br i1 %cmpeq8, label %if9, label %else12

if9:                                              ; preds = %else
  %y10 = load i32, ptr %y2, align 4
  %z11 = load i32, ptr %z3, align 4
  %sub = sub i32 %y10, %z11
  br label %merge41

else12:                                           ; preds = %else
  %x13 = load i32, ptr %x1, align 4
  %cmpeq14 = icmp eq i32 %x13, 2
  br i1 %cmpeq14, label %if15, label %else18

if15:                                             ; preds = %else12
  %y16 = load i32, ptr %y2, align 4
  %z17 = load i32, ptr %z3, align 4
  %mul = mul i32 %y16, %z17
  br label %merge39

else18:                                           ; preds = %else12
  %x19 = load i32, ptr %x1, align 4
  %cmpeq20 = icmp eq i32 %x19, 3
  br i1 %cmpeq20, label %if21, label %else28

if21:                                             ; preds = %else18
  %z22 = load i32, ptr %z3, align 4
  %cmpeq23 = icmp eq i32 %z22, 0
  br i1 %cmpeq23, label %if24, label %else25

if24:                                             ; preds = %if21
  br label %merge

else25:                                           ; preds = %if21
  %y26 = load i32, ptr %y2, align 4
  %z27 = load i32, ptr %z3, align 4
  %div = sdiv i32 %y26, %z27
  br label %merge

merge:                                            ; preds = %else25, %if24
  %phi = phi i32 [ 0, %if24 ], [ %div, %else25 ]
  br label %merge37

else28:                                           ; preds = %else18
  %z29 = load i32, ptr %z3, align 4
  %cmpeq30 = icmp eq i32 %z29, 0
  br i1 %cmpeq30, label %if31, label %else32

if31:                                             ; preds = %else28
  br label %merge35

else32:                                           ; preds = %else28
  %y33 = load i32, ptr %y2, align 4
  %z34 = load i32, ptr %z3, align 4
  %rem = srem i32 %y33, %z34
  br label %merge35

merge35:                                          ; preds = %else32, %if31
  %phi36 = phi i32 [ 0, %if31 ], [ %rem, %else32 ]
  br label %merge37

merge37:                                          ; preds = %merge35, %merge
  %phi38 = phi i32 [ %phi, %merge ], [ %phi36, %merge35 ]
  br label %merge39

merge39:                                          ; preds = %merge37, %if15
  %phi40 = phi i32 [ %mul, %if15 ], [ %phi38, %merge37 ]
  br label %merge41

merge41:                                          ; preds = %merge39, %if9
  %phi42 = phi i32 [ %sub, %if9 ], [ %phi40, %merge39 ]
  br label %merge43

merge43:                                          ; preds = %merge41, %if
  %phi44 = phi i32 [ %add, %if ], [ %phi42, %merge41 ]
  ret i32 %phi44
}

; Function Attrs: noinline nounwind optnone
define i32 @branchInExpression(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %num = alloca i32, align 4
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %entry
  %y5 = load i32, ptr %y2, align 4
  br label %merge

else:                                             ; preds = %entry
  %z6 = load i32, ptr %z3, align 4
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %y5, %if ], [ %z6, %else ]
  %add = add i32 %phi, 58
  store i32 %add, ptr %num, align 4
  %num7 = load i32, ptr %num, align 4
  ret i32 %num7
}

; Function Attrs: noinline nounwind optnone
define i32 @branchWithLogicalOperators(i32 %x, i32 %y, i32 %z) #0 {
entry:
  %z3 = alloca i32, align 4
  %y2 = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 %y, ptr %y2, align 4
  store i32 %z, ptr %z3, align 4
  %x4 = load i32, ptr %x1, align 4
  %cmpeq = icmp eq i32 %x4, 0
  br i1 %cmpeq, label %andtrue, label %andmerge

andtrue:                                          ; preds = %entry
  %y5 = load i32, ptr %y2, align 4
  %cmpeq6 = icmp eq i32 %y5, 1
  br label %andmerge

andmerge:                                         ; preds = %andtrue, %entry
  %andphi = phi i1 [ %cmpeq6, %andtrue ], [ false, %entry ]
  br i1 %andphi, label %ormerge, label %orfalse

orfalse:                                          ; preds = %andmerge
  %z7 = load i32, ptr %z3, align 4
  %cmpeq8 = icmp eq i32 %z7, 1
  br label %ormerge

ormerge:                                          ; preds = %orfalse, %andmerge
  %orphi = phi i1 [ %cmpeq8, %orfalse ], [ true, %andmerge ]
  br i1 %orphi, label %if, label %else

if:                                               ; preds = %ormerge
  br label %merge

else:                                             ; preds = %ormerge
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ 100, %if ], [ 200, %else ]
  ret i32 %phi
}

; Function Attrs: noinline nounwind optnone
define i32 @assign(i32 %a, i32 %b, i32 %c) #0 {
entry:
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  %b4 = load i32, ptr %b2, align 4
  %c5 = load i32, ptr %c3, align 4
  %mul = mul i32 %b4, %c5
  store i32 %mul, ptr %a1, align 4
  %a6 = load i32, ptr %a1, align 4
  %b7 = load i32, ptr %b2, align 4
  %mul8 = mul i32 3, %b7
  %add = add i32 %a6, %mul8
  %c9 = load i32, ptr %c3, align 4
  %add10 = add i32 %add, %c9
  store i32 %add10, ptr %b2, align 4
  %c11 = load i32, ptr %c3, align 4
  %add12 = add i32 %c11, 42
  store i32 %add12, ptr %c3, align 4
  %c13 = load i32, ptr %c3, align 4
  ret i32 %c13
}

; Function Attrs: noinline nounwind optnone
define i64 @assignWithBranch(i32 %a, i32 %b, i64 %c) #0 {
entry:
  %c3 = alloca i64, align 8
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i64 %c, ptr %c3, align 8
  %b4 = load i32, ptr %b2, align 4
  %cmple = icmp sle i32 %b4, 0
  br i1 %cmple, label %if, label %else

if:                                               ; preds = %entry
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %mul = mul i32 %a5, %b6
  %signext = sext i32 %mul to i64
  %c7 = load i64, ptr %c3, align 8
  %add = add i64 %signext, %c7
  br label %merge

else:                                             ; preds = %entry
  %a8 = load i32, ptr %a1, align 4
  %b9 = load i32, ptr %b2, align 4
  %div = sdiv i32 %a8, %b9
  %signext10 = sext i32 %div to i64
  %c11 = load i64, ptr %c3, align 8
  %sub = sub i64 %signext10, %c11
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i64 [ %add, %if ], [ %sub, %else ]
  store i64 %phi, ptr %c3, align 8
  %c12 = load i64, ptr %c3, align 8
  %mul13 = mul i64 %c12, 2
  store i64 %mul13, ptr %c3, align 8
  %c14 = load i64, ptr %c3, align 8
  ret i64 %c14
}

; Function Attrs: noinline nounwind optnone
define i32 @assignInBranch(i32 %a, i32 %b, i32 %c) #0 {
entry:
  %c3 = alloca i32, align 4
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  store i32 %c, ptr %c3, align 4
  %a4 = load i32, ptr %a1, align 4
  %cmple = icmp sle i32 %a4, 0
  br i1 %cmple, label %if, label %else

if:                                               ; preds = %entry
  store i32 10, ptr %b2, align 4
  %c5 = load i32, ptr %c3, align 4
  %add = add i32 %c5, 1
  br label %merge

else:                                             ; preds = %entry
  store i32 10000, ptr %b2, align 4
  %c6 = load i32, ptr %c3, align 4
  %add7 = add i32 %c6, 2
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %add, %if ], [ %add7, %else ]
  store i32 %phi, ptr %c3, align 4
  %b8 = load i32, ptr %b2, align 4
  %c9 = load i32, ptr %c3, align 4
  %mul = mul i32 %b8, %c9
  store i32 %mul, ptr %c3, align 4
  %c10 = load i32, ptr %c3, align 4
  ret i32 %c10
}

; Function Attrs: noinline nounwind optnone
define i64 @createVars(i32 %num) #0 {
entry:
  %temp2 = alloca i64, align 8
  %temp1 = alloca i32, align 4
  %rv = alloca i64, align 8
  %num1 = alloca i32, align 4
  store i32 %num, ptr %num1, align 4
  %num2 = load i32, ptr %num1, align 4
  %cmplt = icmp slt i32 %num2, 0
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  %num3 = load i32, ptr %num1, align 4
  %mul = mul i32 %num3, 10
  store i32 %mul, ptr %temp1, align 4
  %temp14 = load i32, ptr %temp1, align 4
  %signext = sext i32 %temp14 to i64
  br label %merge

else:                                             ; preds = %entry
  %num5 = load i32, ptr %num1, align 4
  %div = sdiv i32 %num5, 10
  %signext6 = sext i32 %div to i64
  store i64 %signext6, ptr %temp2, align 8
  %temp27 = load i64, ptr %temp2, align 8
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i64 [ %signext, %if ], [ %temp27, %else ]
  store i64 %phi, ptr %rv, align 8
  %rv8 = load i64, ptr %rv, align 8
  ret i64 %rv8
}

; Function Attrs: noinline nounwind optnone
define i32 @inferTypes(i32 %a, i32 %b) #0 {
entry:
  %quotient = alloca i32, align 4
  %bIsZero = alloca i1, align 1
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %b3 = load i32, ptr %b2, align 4
  %cmpeq = icmp eq i32 %b3, 0
  store i1 %cmpeq, ptr %bIsZero, align 1
  %bIsZero4 = load i1, ptr %bIsZero, align 1
  br i1 %bIsZero4, label %if, label %else

if:                                               ; preds = %entry
  br label %merge

else:                                             ; preds = %entry
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %div = sdiv i32 %a5, %b6
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ 0, %if ], [ %div, %else ]
  store i32 %phi, ptr %quotient, align 4
  %quotient7 = load i32, ptr %quotient, align 4
  ret i32 %quotient7
}

; Function Attrs: noinline nounwind optnone
define i32 @whileLoop(i32 %num) #0 {
entry:
  %rv = alloca i32, align 4
  %i = alloca i32, align 4
  %num1 = alloca i32, align 4
  store i32 %num, ptr %num1, align 4
  store i32 0, ptr %i, align 4
  store i32 1, ptr %rv, align 4
  br label %whileCond

whileCond:                                        ; preds = %whileBody, %entry
  %i2 = load i32, ptr %i, align 4
  %num3 = load i32, ptr %num1, align 4
  %cmplt = icmp slt i32 %i2, %num3
  br i1 %cmplt, label %whileBody, label %whileExit

whileBody:                                        ; preds = %whileCond
  %rv4 = load i32, ptr %rv, align 4
  %mul = mul i32 %rv4, 2
  store i32 %mul, ptr %rv, align 4
  %i5 = load i32, ptr %i, align 4
  %add = add i32 %i5, 1
  store i32 %add, ptr %i, align 4
  br label %whileCond

whileExit:                                        ; preds = %whileCond
  %rv6 = load i32, ptr %rv, align 4
  ret i32 %rv6
}

; Function Attrs: noinline nounwind optnone
define i32 @nestedLoop(i32 %num) #0 {
entry:
  %j = alloca i32, align 4
  %rv = alloca i32, align 4
  %i = alloca i32, align 4
  %num1 = alloca i32, align 4
  store i32 %num, ptr %num1, align 4
  store i32 0, ptr %i, align 4
  store i32 1, ptr %rv, align 4
  br label %whileCond

whileCond:                                        ; preds = %whileExit, %entry
  %i2 = load i32, ptr %i, align 4
  %num3 = load i32, ptr %num1, align 4
  %cmplt = icmp slt i32 %i2, %num3
  br i1 %cmplt, label %whileBody, label %whileExit18

whileBody:                                        ; preds = %whileCond
  %i4 = load i32, ptr %i, align 4
  %rem = srem i32 %i4, 2
  %cmpeq = icmp eq i32 %rem, 0
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %whileBody
  %rv5 = load i32, ptr %rv, align 4
  %mul = mul i32 %rv5, 2
  br label %merge

else:                                             ; preds = %whileBody
  %rv6 = load i32, ptr %rv, align 4
  %mul7 = mul i32 %rv6, 3
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i32 [ %mul, %if ], [ %mul7, %else ]
  store i32 %phi, ptr %rv, align 4
  store i32 0, ptr %j, align 4
  br label %whileCond8

whileCond8:                                       ; preds = %whileBody12, %merge
  %j9 = load i32, ptr %j, align 4
  %i10 = load i32, ptr %i, align 4
  %cmplt11 = icmp slt i32 %j9, %i10
  br i1 %cmplt11, label %whileBody12, label %whileExit

whileBody12:                                      ; preds = %whileCond8
  %rv13 = load i32, ptr %rv, align 4
  %add = add i32 %rv13, 1
  store i32 %add, ptr %rv, align 4
  %j14 = load i32, ptr %j, align 4
  %add15 = add i32 %j14, 1
  store i32 %add15, ptr %j, align 4
  br label %whileCond8

whileExit:                                        ; preds = %whileCond8
  %i16 = load i32, ptr %i, align 4
  %add17 = add i32 %i16, 1
  store i32 %add17, ptr %i, align 4
  br label %whileCond

whileExit18:                                      ; preds = %whileCond
  %rv19 = load i32, ptr %rv, align 4
  ret i32 %rv19
}

; Function Attrs: noinline nounwind optnone
define i64 @whileComplexCondition(%str %s, i8 %c) #0 {
entry:
  %idx = alloca i64, align 8
  %c2 = alloca i8, align 1
  %s1 = alloca %str, align 8
  store %str %s, ptr %s1, align 8
  store i8 %c, ptr %c2, align 1
  store i64 0, ptr %idx, align 8
  br label %whileCond

whileCond:                                        ; preds = %whileBody, %entry
  %idx3 = load i64, ptr %idx, align 8
  %s4 = load %str, ptr %s1, align 8
  %mber = extractvalue %str %s4, 0
  %cmplt = icmp ult i64 %idx3, %mber
  br i1 %cmplt, label %andtrue, label %andmerge

andtrue:                                          ; preds = %whileCond
  %s5 = load %str, ptr %s1, align 8
  %idx6 = load i64, ptr %idx, align 8
  %size = extractvalue %str %s5, 0
  %check = icmp uge i64 %idx6, %size
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %andtrue
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %andtrue
  %data = extractvalue %str %s5, 1
  %value = getelementptr inbounds i8, ptr %data, i64 %idx6
  %load = load i8, ptr %value, align 1
  %c7 = load i8, ptr %c2, align 1
  %cmpne = icmp ne i8 %load, %c7
  br label %andmerge

andmerge:                                         ; preds = %passed, %whileCond
  %andphi = phi i1 [ %cmpne, %passed ], [ false, %whileCond ]
  br i1 %andphi, label %whileBody, label %whileExit

whileBody:                                        ; preds = %andmerge
  %load8 = load i64, ptr %idx, align 8
  %add = add i64 %load8, 1
  store i64 %add, ptr %idx, align 8
  br label %whileCond

whileExit:                                        ; preds = %andmerge
  %idx9 = load i64, ptr %idx, align 8
  ret i64 %idx9
}

; Function Attrs: noinline nounwind optnone
define i32 @forLoop(i32 %start, i32 %end) #0 {
entry:
  %i = alloca i32, align 4
  %num = alloca i32, align 4
  %end2 = alloca i32, align 4
  %start1 = alloca i32, align 4
  store i32 %start, ptr %start1, align 4
  store i32 %end, ptr %end2, align 4
  store i32 1, ptr %num, align 4
  %start3 = load i32, ptr %start1, align 4
  %end4 = load i32, ptr %end2, align 4
  %rng = insertvalue %"RangeClosed'i32'" undef, i32 %start3, 0
  %rng5 = insertvalue %"RangeClosed'i32'" %rng, i32 %end4, 1
  %start6 = extractvalue %"RangeClosed'i32'" %rng5, 0
  %end7 = extractvalue %"RangeClosed'i32'" %rng5, 1
  br label %forCond

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i32 [ %start6, %entry ], [ %inc, %forIter ]
  %cmp = icmp sle i32 %iter, %end7
  br i1 %cmp, label %forBody, label %forExit

forBody:                                          ; preds = %forCond
  store i32 %iter, ptr %i, align 4
  %i8 = load i32, ptr %i, align 4
  %load = load i32, ptr %num, align 4
  %mul = mul i32 %load, %i8
  store i32 %mul, ptr %num, align 4
  br label %forIter

forIter:                                          ; preds = %forBody
  %inc = add i32 %iter, 1
  br label %forCond

forExit:                                          ; preds = %forCond
  %num9 = load i32, ptr %num, align 4
  ret i32 %num9
}

; Function Attrs: noinline nounwind optnone
define i32 @forLoopArray(%"[]i32" %a) #0 {
entry:
  %x28 = alloca i32, align 4
  %x11 = alloca i16, align 2
  %array6 = alloca [3 x i16], align 2
  %x = alloca i8, align 1
  %array = alloca [3 x i8], align 1
  %sum = alloca i32, align 4
  %a1 = alloca %"[]i32", align 8
  store %"[]i32" %a, ptr %a1, align 8
  store i32 0, ptr %sum, align 4
  %ptr = getelementptr inbounds [3 x i8], ptr %array, i64 0, i64 0
  store i8 1, ptr %ptr, align 1
  %ptr2 = getelementptr inbounds [3 x i8], ptr %array, i64 0, i64 1
  store i8 2, ptr %ptr2, align 1
  %ptr3 = getelementptr inbounds [3 x i8], ptr %array, i64 0, i64 2
  store i8 3, ptr %ptr3, align 1
  %agg = insertvalue %"[]i8" { i64 3, ptr undef }, ptr %array, 1
  %size = extractvalue %"[]i8" %agg, 0
  %data = extractvalue %"[]i8" %agg, 1
  br label %forCond

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i64 [ 0, %entry ], [ %inc, %forIter ]
  %cmp = icmp ult i64 %iter, %size
  br i1 %cmp, label %forBody, label %forExit

forBody:                                          ; preds = %forCond
  %value = getelementptr inbounds i8, ptr %data, i64 %iter
  %load = load i8, ptr %value, align 1
  store i8 %load, ptr %x, align 1
  %x4 = load i8, ptr %x, align 1
  %signext = sext i8 %x4 to i32
  %load5 = load i32, ptr %sum, align 4
  %add = add i32 %load5, %signext
  store i32 %add, ptr %sum, align 4
  br label %forIter

forIter:                                          ; preds = %forBody
  %inc = add i64 %iter, 1
  br label %forCond

forExit:                                          ; preds = %forCond
  %ptr7 = getelementptr inbounds [3 x i16], ptr %array6, i64 0, i64 0
  store i16 10, ptr %ptr7, align 2
  %ptr8 = getelementptr inbounds [3 x i16], ptr %array6, i64 0, i64 1
  store i16 20, ptr %ptr8, align 2
  %ptr9 = getelementptr inbounds [3 x i16], ptr %array6, i64 0, i64 2
  store i16 30, ptr %ptr9, align 2
  %agg10 = insertvalue %"[]i16" { i64 3, ptr undef }, ptr %array6, 1
  %size12 = extractvalue %"[]i16" %agg10, 0
  %data13 = extractvalue %"[]i16" %agg10, 1
  br label %forCond14

forCond14:                                        ; preds = %forIter24, %forExit
  %iter15 = phi i64 [ 0, %forExit ], [ %inc25, %forIter24 ]
  %cmp16 = icmp ult i64 %iter15, %size12
  br i1 %cmp16, label %forBody17, label %forExit26

forBody17:                                        ; preds = %forCond14
  %value18 = getelementptr inbounds i16, ptr %data13, i64 %iter15
  %load19 = load i16, ptr %value18, align 2
  store i16 %load19, ptr %x11, align 2
  %x20 = load i16, ptr %x11, align 2
  %signext21 = sext i16 %x20 to i32
  %load22 = load i32, ptr %sum, align 4
  %add23 = add i32 %load22, %signext21
  store i32 %add23, ptr %sum, align 4
  br label %forIter24

forIter24:                                        ; preds = %forBody17
  %inc25 = add i64 %iter15, 1
  br label %forCond14

forExit26:                                        ; preds = %forCond14
  %a27 = load %"[]i32", ptr %a1, align 8
  %size29 = extractvalue %"[]i32" %a27, 0
  %data30 = extractvalue %"[]i32" %a27, 1
  br label %forCond31

forCond31:                                        ; preds = %forIter40, %forExit26
  %iter32 = phi i64 [ 0, %forExit26 ], [ %inc41, %forIter40 ]
  %cmp33 = icmp ult i64 %iter32, %size29
  br i1 %cmp33, label %forBody34, label %forExit42

forBody34:                                        ; preds = %forCond31
  %value35 = getelementptr inbounds i32, ptr %data30, i64 %iter32
  %load36 = load i32, ptr %value35, align 4
  store i32 %load36, ptr %x28, align 4
  %x37 = load i32, ptr %x28, align 4
  %load38 = load i32, ptr %sum, align 4
  %add39 = add i32 %load38, %x37
  store i32 %add39, ptr %sum, align 4
  br label %forIter40

forIter40:                                        ; preds = %forBody34
  %inc41 = add i64 %iter32, 1
  br label %forCond31

forExit42:                                        ; preds = %forCond31
  %sum43 = load i32, ptr %sum, align 4
  ret i32 %sum43
}

; Function Attrs: noinline nounwind optnone
define i32 @nestedForLoop() #0 {
entry:
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, ptr %x, align 4
  br label %forCond

forCond:                                          ; preds = %forIter7, %entry
  %iter = phi i32 [ 0, %entry ], [ %inc8, %forIter7 ]
  %cmp = icmp ult i32 %iter, 5
  br i1 %cmp, label %forBody, label %forExit9

forBody:                                          ; preds = %forCond
  store i32 %iter, ptr %i, align 4
  br label %forCond1

forCond1:                                         ; preds = %forIter, %forBody
  %iter2 = phi i32 [ 1, %forBody ], [ %inc, %forIter ]
  %cmp3 = icmp ule i32 %iter2, 10
  br i1 %cmp3, label %forBody4, label %forExit

forBody4:                                         ; preds = %forCond1
  store i32 %iter2, ptr %j, align 4
  %i5 = load i32, ptr %i, align 4
  %j6 = load i32, ptr %j, align 4
  %mul = mul i32 %i5, %j6
  %load = load i32, ptr %x, align 4
  %add = add i32 %load, %mul
  store i32 %add, ptr %x, align 4
  br label %forIter

forIter:                                          ; preds = %forBody4
  %inc = add i32 %iter2, 1
  br label %forCond1

forExit:                                          ; preds = %forCond1
  br label %forIter7

forIter7:                                         ; preds = %forExit
  %inc8 = add i32 %iter, 1
  br label %forCond

forExit9:                                         ; preds = %forCond
  %x10 = load i32, ptr %x, align 4
  ret i32 %x10
}

; Function Attrs: noinline nounwind optnone
define i64 @forLoopIndex(%"[]i32" %array) #0 {
entry:
  %i10 = alloca i64, align 8
  %x9 = alloca i32, align 4
  %num = alloca i64, align 8
  %i = alloca i64, align 8
  %x = alloca i32, align 4
  %array1 = alloca %"[]i32", align 8
  store %"[]i32" %array, ptr %array1, align 8
  %array2 = load %"[]i32", ptr %array1, align 8
  %size = extractvalue %"[]i32" %array2, 0
  %data = extractvalue %"[]i32" %array2, 1
  br label %forCond

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i64 [ 0, %entry ], [ %inc, %forIter ]
  %cmp = icmp ult i64 %iter, %size
  br i1 %cmp, label %forBody, label %forExit

forBody:                                          ; preds = %forCond
  %value = getelementptr inbounds i32, ptr %data, i64 %iter
  %load = load i32, ptr %value, align 4
  store i32 %load, ptr %x, align 4
  store i64 %iter, ptr %i, align 8
  %array3 = load %"[]i32", ptr %array1, align 8
  %i4 = load i64, ptr %i, align 8
  %size5 = extractvalue %"[]i32" %array3, 0
  %check = icmp uge i64 %i4, %size5
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %forBody
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %forBody
  %data6 = extractvalue %"[]i32" %array3, 1
  %value7 = getelementptr inbounds i32, ptr %data6, i64 %i4
  %x8 = load i32, ptr %x, align 4
  %mul = mul i32 %x8, 2
  store i32 %mul, ptr %value7, align 4
  br label %forIter

forIter:                                          ; preds = %passed
  %inc = add i64 %iter, 1
  br label %forCond

forExit:                                          ; preds = %forCond
  store i64 0, ptr %num, align 8
  br label %forCond11

forCond11:                                        ; preds = %forIter19, %forExit
  %iter12 = phi i32 [ 0, %forExit ], [ %inc20, %forIter19 ]
  %idx = phi i64 [ 0, %forExit ], [ %inc21, %forIter19 ]
  %cmp13 = icmp ult i32 %iter12, 10
  br i1 %cmp13, label %forBody14, label %forExit22

forBody14:                                        ; preds = %forCond11
  store i32 %iter12, ptr %x9, align 4
  store i64 %idx, ptr %i10, align 8
  %x15 = load i32, ptr %x9, align 4
  %zeroext = zext i32 %x15 to i64
  %i16 = load i64, ptr %i10, align 8
  %mul17 = mul i64 %zeroext, %i16
  %load18 = load i64, ptr %num, align 8
  %add = add i64 %load18, %mul17
  store i64 %add, ptr %num, align 8
  br label %forIter19

forIter19:                                        ; preds = %forBody14
  %inc20 = add i32 %iter12, 1
  %inc21 = add i64 %idx, 1
  br label %forCond11

forExit22:                                        ; preds = %forCond11
  %num23 = load i64, ptr %num, align 8
  ret i64 %num23
}

; Function Attrs: noinline nounwind optnone
define %UnitType @forLoopImplicitCast() #0 {
entry:
  %i = alloca i32, align 4
  %x = alloca i8, align 1
  store i8 10, ptr %x, align 1
  %x1 = load i8, ptr %x, align 1
  %rng = insertvalue %"RangeClosed'i8'" { i8 0, i8 undef }, i8 %x1, 1
  %start = extractvalue %"RangeClosed'i8'" %rng, 0
  %signext = sext i8 %start to i32
  %end = extractvalue %"RangeClosed'i8'" %rng, 1
  %signext2 = sext i8 %end to i32
  br label %forCond

forCond:                                          ; preds = %forIter, %entry
  %iter = phi i32 [ %signext, %entry ], [ %inc, %forIter ]
  %cmp = icmp sle i32 %iter, %signext2
  br i1 %cmp, label %forBody, label %forExit

forBody:                                          ; preds = %forCond
  store i32 %iter, ptr %i, align 4
  br label %forIter

forIter:                                          ; preds = %forBody
  %inc = add i32 %iter, 1
  br label %forCond

forExit:                                          ; preds = %forCond
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i32 @blockExpression(i32 %param) #0 {
entry:
  %x = alloca i32, align 4
  %param1 = alloca i32, align 4
  store i32 %param, ptr %param1, align 4
  %call = call %UnitType @types_unit(i64 100)
  %param2 = load i32, ptr %param1, align 4
  %cmpgt = icmp sgt i32 %param2, 0
  %param3 = load i32, ptr %param1, align 4
  %rem = srem i32 %param3, 12
  %cmpeq = icmp eq i32 %rem, 5
  %call4 = call i1 @types_bool(i1 %cmpgt, i1 %cmpeq)
  %param5 = load i32, ptr %param1, align 4
  %param6 = load i32, ptr %param1, align 4
  %call7 = call i32 @types_i32(i32 %param5, i32 %param6)
  store i32 %call7, ptr %x, align 4
  %x8 = load i32, ptr %x, align 4
  ret i32 %x8
}

; Function Attrs: noinline nounwind optnone
define %UnitType @nestedBlockExpression() #0 {
entry:
  %x = alloca i32, align 4
  store i32 12, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  %signext = sext i32 %x1 to i64
  %call = call %UnitType @types_unit(i64 %signext)
  %call2 = call %UnitType @types_unit2()
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i64 @externTest() #0 {
entry:
  %call = call %UnitType @extern1()
  %call1 = call i64 @extern2(i32 5, i1 true)
  ret i64 %call1
}

; Function Attrs: noinline nounwind optnone
define i32 @scopes(i32 %x) #0 {
entry:
  %y4 = alloca i16, align 2
  %y = alloca i32, align 4
  %rv = alloca i32, align 4
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i32 0, ptr %rv, align 4
  %x2 = load i32, ptr %x1, align 4
  %cmplt = icmp slt i32 %x2, 0
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  store i32 5, ptr %y, align 4
  %y3 = load i32, ptr %y, align 4
  store i32 %y3, ptr %rv, align 4
  br label %merge

else:                                             ; preds = %entry
  store i16 12, ptr %y4, align 2
  %y5 = load i16, ptr %y4, align 2
  %signext = sext i16 %y5 to i32
  store i32 %signext, ptr %rv, align 4
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi %UnitType [ zeroinitializer, %if ], [ zeroinitializer, %else ]
  %rv6 = load i32, ptr %rv, align 4
  ret i32 %rv6
}

; Function Attrs: noinline nounwind optnone
define %UnitType @initTypes() #0 {
entry:
  %test2 = alloca %Test2, align 8
  %test1 = alloca %Test1, align 8
  %empty = alloca %EmptyType, align 8
  store %EmptyType undef, ptr %empty, align 1
  %load = load %str, ptr @strStruct3, align 8
  %agg = insertvalue %Test1 undef, %str %load, 2
  %call = call i16 @types_i16(i16 1, i16 2)
  %signext = sext i16 %call to i32
  %agg1 = insertvalue %Test1 %agg, i32 %signext, 0
  %agg2 = insertvalue %Test1 %agg1, i1 true, 1
  store %Test1 %agg2, ptr %test1, align 8
  %load3 = load %str, ptr @strStruct6, align 8
  %agg4 = insertvalue %Test1 { i32 1, i1 false, %str undef }, %str %load3, 2
  %agg5 = insertvalue %Test2 undef, %Test1 %agg4, 0
  %agg6 = insertvalue %Test2 %agg5, i32 12, 1
  store %Test2 %agg6, ptr %test2, align 8
  %mber = getelementptr inbounds %Test1, ptr %test1, i64 0, i32 1
  store i1 false, ptr %mber, align 1
  %mber7 = getelementptr inbounds %Test2, ptr %test2, i64 0, i32 0
  %mber8 = getelementptr inbounds %Test1, ptr %mber7, i64 0, i32 0
  %load9 = load i32, ptr %mber8, align 4
  %add = add i32 %load9, 7
  store i32 %add, ptr %mber8, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @sign_zero_extension(i1 %b, i8 %x8, i8 %y8, i32 %x32) #0 {
entry:
  %r9 = alloca %"RangeClosed'i32'", align 8
  %r8 = alloca i16, align 2
  %r7 = alloca i16, align 2
  %r6 = alloca i32, align 4
  %r5 = alloca i32, align 4
  %r4 = alloca i32, align 4
  %r3 = alloca i32, align 4
  %r2 = alloca i32, align 4
  %r1 = alloca i32, align 4
  %x324 = alloca i32, align 4
  %y83 = alloca i8, align 1
  %x82 = alloca i8, align 1
  %b1 = alloca i1, align 1
  store i1 %b, ptr %b1, align 1
  store i8 %x8, ptr %x82, align 1
  store i8 %y8, ptr %y83, align 1
  store i32 %x32, ptr %x324, align 4
  %b5 = load i1, ptr %b1, align 1
  br i1 %b5, label %if, label %else

if:                                               ; preds = %entry
  %x86 = load i8, ptr %x82, align 1
  %signext = sext i8 %x86 to i16
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i16 [ %signext, %if ], [ 200, %else ]
  %signext7 = sext i16 %phi to i32
  store i32 %signext7, ptr %r1, align 4
  %b8 = load i1, ptr %b1, align 1
  br i1 %b8, label %if9, label %else11

if9:                                              ; preds = %merge
  %y810 = load i8, ptr %y83, align 1
  br label %merge12

else11:                                           ; preds = %merge
  br label %merge12

merge12:                                          ; preds = %else11, %if9
  %phi13 = phi i8 [ %y810, %if9 ], [ -56, %else11 ]
  %zeroext = zext i8 %phi13 to i32
  store i32 %zeroext, ptr %r2, align 4
  %x814 = load i8, ptr %x82, align 1
  %add = add i8 %x814, 20
  %signext15 = sext i8 %add to i32
  store i32 %signext15, ptr %r3, align 4
  %y816 = load i8, ptr %y83, align 1
  %add17 = add i8 %y816, 20
  %zeroext18 = zext i8 %add17 to i32
  store i32 %zeroext18, ptr %r4, align 4
  %x819 = load i8, ptr %x82, align 1
  %signext20 = sext i8 %x819 to i16
  %add21 = add i16 %signext20, 200
  %signext22 = sext i16 %add21 to i32
  store i32 %signext22, ptr %r5, align 4
  %y823 = load i8, ptr %y83, align 1
  %add24 = add i8 %y823, -56
  %zeroext25 = zext i8 %add24 to i32
  store i32 %zeroext25, ptr %r6, align 4
  store i16 201, ptr %r7, align 2
  store i16 201, ptr %r8, align 2
  %x3226 = load i32, ptr %x324, align 4
  %x3227 = load i32, ptr %x324, align 4
  %rng = insertvalue %"RangeClosed'i32'" undef, i32 %x3226, 0
  %rng28 = insertvalue %"RangeClosed'i32'" %rng, i32 %x3227, 1
  store %"RangeClosed'i32'" %rng28, ptr %r9, align 4
  %b29 = load i1, ptr %b1, align 1
  br i1 %b29, label %if30, label %else31

if30:                                             ; preds = %merge12
  br label %merge32

else31:                                           ; preds = %merge12
  br label %merge32

merge32:                                          ; preds = %else31, %if30
  %phi33 = phi %"RangeClosed'i32'" [ { i32 0, i32 1 }, %if30 ], [ { i32 1, i32 2 }, %else31 ]
  store %"RangeClosed'i32'" %phi33, ptr %r9, align 4
  %b34 = load i1, ptr %b1, align 1
  br i1 %b34, label %if35, label %else36

if35:                                             ; preds = %merge32
  br label %merge37

else36:                                           ; preds = %merge32
  br label %merge37

merge37:                                          ; preds = %else36, %if35
  %phi38 = phi %"RangeClosed'i32'" [ { i32 0, i32 1 }, %if35 ], [ { i32 1, i32 1000 }, %else36 ]
  store %"RangeClosed'i32'" %phi38, ptr %r9, align 4
  %b39 = load i1, ptr %b1, align 1
  br i1 %b39, label %if40, label %else41

if40:                                             ; preds = %merge37
  br label %merge46

else41:                                           ; preds = %merge37
  %x3242 = load i32, ptr %x324, align 4
  %x3243 = load i32, ptr %x324, align 4
  %rng44 = insertvalue %"RangeClosed'i32'" undef, i32 %x3242, 0
  %rng45 = insertvalue %"RangeClosed'i32'" %rng44, i32 %x3243, 1
  br label %merge46

merge46:                                          ; preds = %else41, %if40
  %phi47 = phi %"RangeClosed'i32'" [ { i32 0, i32 1 }, %if40 ], [ %rng45, %else41 ]
  store %"RangeClosed'i32'" %phi47, ptr %r9, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @sign_operator(i8 %x8, i8 %y8) #0 {
entry:
  %r2 = alloca i32, align 4
  %r1 = alloca i32, align 4
  %y82 = alloca i8, align 1
  %x81 = alloca i8, align 1
  store i8 %x8, ptr %x81, align 1
  store i8 %y8, ptr %y82, align 1
  %x83 = load i8, ptr %x81, align 1
  %signext = sext i8 %x83 to i16
  %div = sdiv i16 200, %signext
  %signext4 = sext i16 %div to i32
  store i32 %signext4, ptr %r1, align 4
  %y85 = load i8, ptr %y82, align 1
  %div6 = udiv i8 -56, %y85
  %zeroext = zext i8 %div6 to i32
  store i32 %zeroext, ptr %r2, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @unary_operator(i1 %b, i8 %x, i8 %y) #0 {
entry:
  %r16 = alloca i32, align 4
  %r15 = alloca i32, align 4
  %r14 = alloca i32, align 4
  %r13 = alloca i32, align 4
  %r12 = alloca i32, align 4
  %r11 = alloca i32, align 4
  %r10 = alloca i32, align 4
  %r9 = alloca i32, align 4
  %r8 = alloca i32, align 4
  %r7 = alloca i32, align 4
  %r6 = alloca i32, align 4
  %r5 = alloca i32, align 4
  %r4 = alloca i32, align 4
  %r3 = alloca i32, align 4
  %r2 = alloca i32, align 4
  %r1 = alloca i32, align 4
  %y3 = alloca i8, align 1
  %x2 = alloca i8, align 1
  %b1 = alloca i1, align 1
  store i1 %b, ptr %b1, align 1
  store i8 %x, ptr %x2, align 1
  store i8 %y, ptr %y3, align 1
  %b4 = load i1, ptr %b1, align 1
  br i1 %b4, label %if, label %else

if:                                               ; preds = %entry
  %x5 = load i8, ptr %x2, align 1
  %signext = sext i8 %x5 to i16
  br label %merge

else:                                             ; preds = %entry
  br label %merge

merge:                                            ; preds = %else, %if
  %phi = phi i16 [ %signext, %if ], [ -200, %else ]
  %signext6 = sext i16 %phi to i32
  store i32 %signext6, ptr %r1, align 4
  %x7 = load i8, ptr %x2, align 1
  %signext8 = sext i8 %x7 to i32
  %add = add i32 %signext8, -200
  store i32 %add, ptr %r2, align 4
  store i32 -20, ptr %r3, align 4
  store i32 20, ptr %r4, align 4
  store i32 -200, ptr %r5, align 4
  store i32 200, ptr %r6, align 4
  store i32 -21, ptr %r7, align 4
  store i32 20, ptr %r8, align 4
  store i32 -21, ptr %r9, align 4
  store i32 20, ptr %r10, align 4
  store i32 -201, ptr %r11, align 4
  store i32 200, ptr %r12, align 4
  store i32 -201, ptr %r13, align 4
  store i32 200, ptr %r14, align 4
  %x9 = load i8, ptr %x2, align 1
  %signext10 = sext i8 %x9 to i16
  %add11 = add i16 %signext10, 200
  %signext12 = sext i16 %add11 to i32
  store i32 %signext12, ptr %r15, align 4
  %y13 = load i8, ptr %y3, align 1
  %add14 = add i8 %y13, -56
  %zeroext = zext i8 %add14 to i32
  store i32 %zeroext, ptr %r16, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define ptr @pointers(ptr %p1) #0 {
entry:
  %b = alloca i1, align 1
  %bPtr2 = alloca ptr, align 8
  %bPtr1 = alloca ptr, align 8
  %test = alloca %Test1, align 8
  %xPtr = alloca ptr, align 8
  %x = alloca i8, align 1
  %isEqual = alloca i1, align 1
  %p2 = alloca ptr, align 8
  %p11 = alloca ptr, align 8
  store ptr %p1, ptr %p11, align 8
  %p12 = load ptr, ptr %p11, align 8
  store ptr %p12, ptr %p2, align 8
  %p13 = load ptr, ptr %p11, align 8
  %p24 = load ptr, ptr %p2, align 8
  %cmpeq = icmp eq ptr %p13, %p24
  store i1 %cmpeq, ptr %isEqual, align 1
  store i8 2, ptr %x, align 1
  store ptr %x, ptr %xPtr, align 8
  %xPtr5 = load ptr, ptr %xPtr, align 8
  %load = load i8, ptr %xPtr5, align 1
  %add = add i8 %load, 5
  store i8 %add, ptr %xPtr5, align 1
  %xPtr6 = load ptr, ptr %xPtr, align 8
  %load7 = load i8, ptr %xPtr6, align 1
  %signext = sext i8 %load7 to i32
  %agg = insertvalue %Test1 undef, i32 %signext, 0
  %agg8 = insertvalue %Test1 %agg, i1 true, 1
  %load9 = load %str, ptr @strStruct6, align 8
  %agg10 = insertvalue %Test1 %agg8, %str %load9, 2
  store %Test1 %agg10, ptr %test, align 8
  %mber = getelementptr inbounds %Test1, ptr %test, i64 0, i32 1
  store ptr %mber, ptr %bPtr1, align 8
  store ptr %bPtr1, ptr %bPtr2, align 8
  %bPtr211 = load ptr, ptr %bPtr2, align 8
  %load12 = load ptr, ptr %bPtr211, align 8
  %load13 = load i1, ptr %load12, align 1
  store i1 %load13, ptr %b, align 1
  %p114 = load ptr, ptr %p11, align 8
  ret ptr %p114
}

; Function Attrs: noinline nounwind optnone
define %UnitType @subscript(%str %s, %SubscriptTest1 %t) #0 {
entry:
  %y = alloca i8, align 1
  %x = alloca i8, align 1
  %t2 = alloca %SubscriptTest1, align 8
  %s1 = alloca %str, align 8
  store %str %s, ptr %s1, align 8
  store %SubscriptTest1 %t, ptr %t2, align 8
  %s3 = load %str, ptr %s1, align 8
  %size = extractvalue %str %s3, 0
  %check = icmp uge i64 0, %size
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %entry
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %entry
  %data = extractvalue %str %s3, 1
  %value = getelementptr inbounds i8, ptr %data, i64 0
  %load = load i8, ptr %value, align 1
  store i8 %load, ptr %x, align 1
  %t4 = load %SubscriptTest1, ptr %t2, align 8
  %mber = extractvalue %SubscriptTest1 %t4, 0
  %mber5 = extractvalue %SubscriptTest2 %mber, 0
  %size6 = extractvalue %str %mber5, 0
  %check7 = icmp uge i64 5, %size6
  br i1 %check7, label %failed8, label %passed9

failed8:                                          ; preds = %passed
  call void @exit(i32 1)
  unreachable

passed9:                                          ; preds = %passed
  %data10 = extractvalue %str %mber5, 1
  %value11 = getelementptr inbounds i8, ptr %data10, i64 5
  %load12 = load i8, ptr %value11, align 1
  store i8 %load12, ptr %y, align 1
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @break_and_continue() #0 {
entry:
  %k = alloca i32, align 4
  %j = alloca i8, align 1
  %i = alloca i8, align 1
  br label %forCond

forCond:                                          ; preds = %forIter12, %entry
  %iter = phi i8 [ 0, %entry ], [ %inc13, %forIter12 ]
  %cmp = icmp sle i8 %iter, 10
  br i1 %cmp, label %forBody, label %forExit14

forBody:                                          ; preds = %forCond
  store i8 %iter, ptr %i, align 1
  br label %forCond1

forCond1:                                         ; preds = %forIter, %forBody
  %iter2 = phi i8 [ 0, %forBody ], [ %inc, %forIter ]
  %cmp3 = icmp sle i8 %iter2, 7
  br i1 %cmp3, label %forBody4, label %forExit

forBody4:                                         ; preds = %forCond1
  store i8 %iter2, ptr %j, align 1
  %j5 = load i8, ptr %j, align 1
  %cmpeq = icmp eq i8 %j5, 4
  br i1 %cmpeq, label %if, label %else

if:                                               ; preds = %forBody4
  br label %forIter

aftercontinue:                                    ; No predecessors!
  br label %merge

else:                                             ; preds = %forBody4
  br label %merge

merge:                                            ; preds = %else, %aftercontinue
  %phi = phi %UnitType [ zeroinitializer, %aftercontinue ], [ zeroinitializer, %else ]
  %call = call i32 @noArgs()
  br label %forIter

forIter:                                          ; preds = %merge, %if
  %inc = add i8 %iter2, 1
  br label %forCond1

forExit:                                          ; preds = %forCond1
  %i6 = load i8, ptr %i, align 1
  %cmpeq7 = icmp eq i8 %i6, 8
  br i1 %cmpeq7, label %if8, label %else9

if8:                                              ; preds = %forExit
  br label %forExit14

afterbreak:                                       ; No predecessors!
  br label %merge10

else9:                                            ; preds = %forExit
  br label %merge10

merge10:                                          ; preds = %else9, %afterbreak
  %phi11 = phi %UnitType [ zeroinitializer, %afterbreak ], [ zeroinitializer, %else9 ]
  br label %forIter12

forIter12:                                        ; preds = %merge10
  %inc13 = add i8 %iter, 1
  br label %forCond

forExit14:                                        ; preds = %if8, %forCond
  store i32 0, ptr %k, align 4
  br label %whileCond

whileCond:                                        ; preds = %merge28, %if18, %forExit14
  %k15 = load i32, ptr %k, align 4
  %cmplt = icmp slt i32 %k15, 17
  br i1 %cmplt, label %whileBody, label %whileExit

whileBody:                                        ; preds = %whileCond
  %k16 = load i32, ptr %k, align 4
  %cmpeq17 = icmp eq i32 %k16, 3
  br i1 %cmpeq17, label %if18, label %else20

if18:                                             ; preds = %whileBody
  br label %whileCond

aftercontinue19:                                  ; No predecessors!
  br label %merge28

else20:                                           ; preds = %whileBody
  %k21 = load i32, ptr %k, align 4
  %cmpeq22 = icmp eq i32 %k21, 10
  br i1 %cmpeq22, label %if23, label %else25

if23:                                             ; preds = %else20
  br label %whileExit

afterbreak24:                                     ; No predecessors!
  br label %merge26

else25:                                           ; preds = %else20
  br label %merge26

merge26:                                          ; preds = %else25, %afterbreak24
  %phi27 = phi %UnitType [ zeroinitializer, %afterbreak24 ], [ zeroinitializer, %else25 ]
  br label %merge28

merge28:                                          ; preds = %merge26, %aftercontinue19
  %phi29 = phi %UnitType [ zeroinitializer, %aftercontinue19 ], [ %phi27, %merge26 ]
  %call30 = call i32 @noArgs()
  %load = load i32, ptr %k, align 4
  %add = add i32 %load, 1
  store i32 %add, ptr %k, align 4
  br label %whileCond

whileExit:                                        ; preds = %if23, %whileCond
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i32 @ret1(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %cmplt = icmp slt i32 %a3, %b4
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  ret i32 -1

afterreturn:                                      ; No predecessors!
  br label %merge10

else:                                             ; preds = %entry
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %cmpgt = icmp sgt i32 %a5, %b6
  br i1 %cmpgt, label %if7, label %else9

if7:                                              ; preds = %else
  ret i32 1

afterreturn8:                                     ; No predecessors!
  br label %merge

else9:                                            ; preds = %else
  br label %merge

merge:                                            ; preds = %else9, %afterreturn8
  %phi = phi %UnitType [ zeroinitializer, %afterreturn8 ], [ zeroinitializer, %else9 ]
  br label %merge10

merge10:                                          ; preds = %merge, %afterreturn
  %phi11 = phi %UnitType [ zeroinitializer, %afterreturn ], [ %phi, %merge ]
  ret i32 0
}

; Function Attrs: noinline nounwind optnone
define i32 @ret2(i32 %a, i32 %b) #0 {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %cmplt = icmp slt i32 %a3, %b4
  br i1 %cmplt, label %if, label %else

if:                                               ; preds = %entry
  ret i32 -1

afterreturn:                                      ; No predecessors!
  br label %merge10

else:                                             ; preds = %entry
  %a5 = load i32, ptr %a1, align 4
  %b6 = load i32, ptr %b2, align 4
  %cmpgt = icmp sgt i32 %a5, %b6
  br i1 %cmpgt, label %if7, label %else9

if7:                                              ; preds = %else
  ret i32 1

afterreturn8:                                     ; No predecessors!
  br label %merge

else9:                                            ; preds = %else
  br label %merge

merge:                                            ; preds = %else9, %afterreturn8
  %phi = phi %UnitType [ zeroinitializer, %afterreturn8 ], [ zeroinitializer, %else9 ]
  br label %merge10

merge10:                                          ; preds = %merge, %afterreturn
  %phi11 = phi %UnitType [ zeroinitializer, %afterreturn ], [ %phi, %merge ]
  ret i32 0
}

; Function Attrs: noinline nounwind optnone
define %UnitType @arrays1(%"[]i32" %a1) #0 {
entry:
  %n = alloca i32, align 4
  %a11 = alloca %"[]i32", align 8
  store %"[]i32" %a1, ptr %a11, align 8
  %a12 = load %"[]i32", ptr %a11, align 8
  %size = extractvalue %"[]i32" %a12, 0
  %check = icmp uge i64 0, %size
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %entry
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %entry
  %data = extractvalue %"[]i32" %a12, 1
  %value = getelementptr inbounds i32, ptr %data, i64 0
  %load = load i32, ptr %value, align 4
  store i32 %load, ptr %n, align 4
  %a13 = load %"[]i32", ptr %a11, align 8
  %size4 = extractvalue %"[]i32" %a13, 0
  %check5 = icmp uge i64 1, %size4
  br i1 %check5, label %failed6, label %passed7

failed6:                                          ; preds = %passed
  call void @exit(i32 1)
  unreachable

passed7:                                          ; preds = %passed
  %data8 = extractvalue %"[]i32" %a13, 1
  %value9 = getelementptr inbounds i32, ptr %data8, i64 1
  store i32 17, ptr %value9, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @arrays2() #0 {
entry:
  %array64 = alloca [5 x i8], align 1
  %r1 = alloca i8, align 1
  %array52 = alloca [12 x i32], align 4
  %array42 = alloca [3 x i32], align 4
  %s = alloca %ArrayTest, align 8
  %array33 = alloca [12 x i32], align 4
  %array24 = alloca [4 x i32], align 4
  %a5 = alloca %"[]i32", align 8
  %array15 = alloca [3 x i32], align 4
  %a4 = alloca %"[]i32", align 8
  %array5 = alloca [3 x i16], align 2
  %a3 = alloca %"[]i16", align 8
  %array3 = alloca [0 x i32], align 4
  %a2 = alloca %"[]i32", align 8
  %array = alloca [10 x i32], align 4
  %a1 = alloca %"[]i32", align 8
  %x = alloca i32, align 4
  store i32 12, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  %startPtr = getelementptr inbounds [10 x i32], ptr %array, i64 0, i64 0
  %endPtr = getelementptr inbounds [10 x i32], ptr %array, i64 0, i64 10
  br label %fillBody

fillBody:                                         ; preds = %fillBody, %entry
  %phi = phi ptr [ %startPtr, %entry ], [ %nextPtr, %fillBody ]
  store i32 %x1, ptr %phi, align 4
  %nextPtr = getelementptr inbounds [10 x i32], ptr %phi, i64 1
  %atEnd = icmp eq ptr %nextPtr, %endPtr
  br i1 %atEnd, label %fillExit, label %fillBody

fillExit:                                         ; preds = %fillBody
  %agg = insertvalue %"[]i32" { i64 10, ptr undef }, ptr %array, 1
  store %"[]i32" %agg, ptr %a1, align 8
  %x2 = load i32, ptr %x, align 4
  %agg4 = insertvalue %"[]i32" { i64 0, ptr undef }, ptr %array3, 1
  store %"[]i32" %agg4, ptr %a2, align 8
  %startPtr7 = getelementptr inbounds [3 x i16], ptr %array5, i64 0, i64 0
  %endPtr8 = getelementptr inbounds [3 x i16], ptr %array5, i64 0, i64 3
  br label %fillBody6

fillBody6:                                        ; preds = %fillBody6, %fillExit
  %phi9 = phi ptr [ %startPtr7, %fillExit ], [ %nextPtr10, %fillBody6 ]
  store i16 201, ptr %phi9, align 2
  %nextPtr10 = getelementptr inbounds [3 x i16], ptr %phi9, i64 1
  %atEnd11 = icmp eq ptr %nextPtr10, %endPtr8
  br i1 %atEnd11, label %fillExit12, label %fillBody6

fillExit12:                                       ; preds = %fillBody6
  %agg13 = insertvalue %"[]i16" { i64 3, ptr undef }, ptr %array5, 1
  store %"[]i16" %agg13, ptr %a3, align 8
  %x14 = load i32, ptr %x, align 4
  %mul = mul i32 %x14, 2
  %add = add i32 1, %mul
  %startPtr17 = getelementptr inbounds [3 x i32], ptr %array15, i64 0, i64 0
  %endPtr18 = getelementptr inbounds [3 x i32], ptr %array15, i64 0, i64 3
  br label %fillBody16

fillBody16:                                       ; preds = %fillBody16, %fillExit12
  %phi19 = phi ptr [ %startPtr17, %fillExit12 ], [ %nextPtr20, %fillBody16 ]
  store i32 %add, ptr %phi19, align 4
  %nextPtr20 = getelementptr inbounds [3 x i32], ptr %phi19, i64 1
  %atEnd21 = icmp eq ptr %nextPtr20, %endPtr18
  br i1 %atEnd21, label %fillExit22, label %fillBody16

fillExit22:                                       ; preds = %fillBody16
  %agg23 = insertvalue %"[]i32" { i64 3, ptr undef }, ptr %array15, 1
  store %"[]i32" %agg23, ptr %a4, align 8
  %startPtr26 = getelementptr inbounds [4 x i32], ptr %array24, i64 0, i64 0
  %endPtr27 = getelementptr inbounds [4 x i32], ptr %array24, i64 0, i64 4
  br label %fillBody25

fillBody25:                                       ; preds = %fillBody25, %fillExit22
  %phi28 = phi ptr [ %startPtr26, %fillExit22 ], [ %nextPtr29, %fillBody25 ]
  store i32 0, ptr %phi28, align 4
  %nextPtr29 = getelementptr inbounds [4 x i32], ptr %phi28, i64 1
  %atEnd30 = icmp eq ptr %nextPtr29, %endPtr27
  br i1 %atEnd30, label %fillExit31, label %fillBody25

fillExit31:                                       ; preds = %fillBody25
  %agg32 = insertvalue %"[]i32" { i64 4, ptr undef }, ptr %array24, 1
  store %"[]i32" %agg32, ptr %a5, align 8
  %startPtr35 = getelementptr inbounds [12 x i32], ptr %array33, i64 0, i64 0
  %endPtr36 = getelementptr inbounds [12 x i32], ptr %array33, i64 0, i64 12
  br label %fillBody34

fillBody34:                                       ; preds = %fillBody34, %fillExit31
  %phi37 = phi ptr [ %startPtr35, %fillExit31 ], [ %nextPtr38, %fillBody34 ]
  store i32 1, ptr %phi37, align 4
  %nextPtr38 = getelementptr inbounds [12 x i32], ptr %phi37, i64 1
  %atEnd39 = icmp eq ptr %nextPtr38, %endPtr36
  br i1 %atEnd39, label %fillExit40, label %fillBody34

fillExit40:                                       ; preds = %fillBody34
  %agg41 = insertvalue %"[]i32" { i64 12, ptr undef }, ptr %array33, 1
  %call = call %UnitType @arrays1(%"[]i32" %agg41)
  %startPtr44 = getelementptr inbounds [3 x i32], ptr %array42, i64 0, i64 0
  %endPtr45 = getelementptr inbounds [3 x i32], ptr %array42, i64 0, i64 3
  br label %fillBody43

fillBody43:                                       ; preds = %fillBody43, %fillExit40
  %phi46 = phi ptr [ %startPtr44, %fillExit40 ], [ %nextPtr47, %fillBody43 ]
  store i32 19, ptr %phi46, align 4
  %nextPtr47 = getelementptr inbounds [3 x i32], ptr %phi46, i64 1
  %atEnd48 = icmp eq ptr %nextPtr47, %endPtr45
  br i1 %atEnd48, label %fillExit49, label %fillBody43

fillExit49:                                       ; preds = %fillBody43
  %agg50 = insertvalue %"[]i32" { i64 3, ptr undef }, ptr %array42, 1
  %agg51 = insertvalue %ArrayTest undef, %"[]i32" %agg50, 0
  store %ArrayTest %agg51, ptr %s, align 8
  %startPtr54 = getelementptr inbounds [12 x i32], ptr %array52, i64 0, i64 0
  %endPtr55 = getelementptr inbounds [12 x i32], ptr %array52, i64 0, i64 12
  br label %fillBody53

fillBody53:                                       ; preds = %fillBody53, %fillExit49
  %phi56 = phi ptr [ %startPtr54, %fillExit49 ], [ %nextPtr57, %fillBody53 ]
  store i32 123, ptr %phi56, align 4
  %nextPtr57 = getelementptr inbounds [12 x i32], ptr %phi56, i64 1
  %atEnd58 = icmp eq ptr %nextPtr57, %endPtr55
  br i1 %atEnd58, label %fillExit59, label %fillBody53

fillExit59:                                       ; preds = %fillBody53
  %agg60 = insertvalue %"[]i32" { i64 12, ptr undef }, ptr %array52, 1
  %size = extractvalue %"[]i32" %agg60, 0
  %data = extractvalue %"[]i32" %agg60, 1
  %endok = icmp ult i64 7, %size
  %checkend = select i1 %endok, i64 7, i64 %size
  %startok = icmp ule i64 4, %checkend
  %checkstart = select i1 %startok, i64 4, i64 %checkend
  %sub = sub i64 %checkend, %checkstart
  %ptr = getelementptr inbounds i32, ptr %data, i64 %checkstart
  %agg61 = insertvalue %"[]i32" undef, i64 %sub, 0
  %agg62 = insertvalue %"[]i32" %agg61, ptr %ptr, 1
  %call63 = call %UnitType @arrays1(%"[]i32" %agg62)
  %startPtr66 = getelementptr inbounds [5 x i8], ptr %array64, i64 0, i64 0
  %endPtr67 = getelementptr inbounds [5 x i8], ptr %array64, i64 0, i64 5
  br label %fillBody65

fillBody65:                                       ; preds = %fillBody65, %fillExit59
  %phi68 = phi ptr [ %startPtr66, %fillExit59 ], [ %nextPtr69, %fillBody65 ]
  store i8 7, ptr %phi68, align 1
  %nextPtr69 = getelementptr inbounds [5 x i8], ptr %phi68, i64 1
  %atEnd70 = icmp eq ptr %nextPtr69, %endPtr67
  br i1 %atEnd70, label %fillExit71, label %fillBody65

fillExit71:                                       ; preds = %fillBody65
  %agg72 = insertvalue %"[]i8" { i64 5, ptr undef }, ptr %array64, 1
  %size73 = extractvalue %"[]i8" %agg72, 0
  %check = icmp uge i64 3, %size73
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %fillExit71
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %fillExit71
  %data74 = extractvalue %"[]i8" %agg72, 1
  %value = getelementptr inbounds i8, ptr %data74, i64 3
  %load = load i8, ptr %value, align 1
  store i8 %load, ptr %r1, align 1
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @arrays3() #0 {
entry:
  %array61 = alloca [3 x i32], align 4
  %array58 = alloca [2 x i32], align 4
  %array57 = alloca [2 x %"[]i32"], align 8
  %a7 = alloca %"[][]i32", align 8
  %array51 = alloca [2 x i32], align 4
  %array46 = alloca [2 x i32], align 4
  %array45 = alloca [2 x %"[]i32"], align 8
  %a6 = alloca %"[][]i32", align 8
  %array39 = alloca [2 x i16], align 2
  %r1 = alloca i16, align 2
  %array28 = alloca [5 x i32], align 4
  %array22 = alloca [3 x i32], align 4
  %s = alloca %ArrayTest, align 8
  %array17 = alloca [3 x i32], align 4
  %array11 = alloca [2 x i32], align 4
  %a4 = alloca %"[]i32", align 8
  %array6 = alloca [2 x i16], align 2
  %a3 = alloca %"[]i16", align 8
  %array2 = alloca [2 x i8], align 1
  %a2 = alloca %"[]u8", align 8
  %array = alloca [2 x i8], align 1
  %a1 = alloca %"[]i8", align 8
  %y = alloca i32, align 4
  %x = alloca i16, align 2
  store i16 12, ptr %x, align 2
  store i32 123, ptr %y, align 4
  %ptr = getelementptr inbounds [2 x i8], ptr %array, i64 0, i64 0
  store i8 1, ptr %ptr, align 1
  %ptr1 = getelementptr inbounds [2 x i8], ptr %array, i64 0, i64 1
  store i8 -2, ptr %ptr1, align 1
  %agg = insertvalue %"[]i8" { i64 2, ptr undef }, ptr %array, 1
  store %"[]i8" %agg, ptr %a1, align 8
  %ptr3 = getelementptr inbounds [2 x i8], ptr %array2, i64 0, i64 0
  store i8 1, ptr %ptr3, align 1
  %ptr4 = getelementptr inbounds [2 x i8], ptr %array2, i64 0, i64 1
  store i8 2, ptr %ptr4, align 1
  %agg5 = insertvalue %"[]u8" { i64 2, ptr undef }, ptr %array2, 1
  store %"[]u8" %agg5, ptr %a2, align 8
  %ptr7 = getelementptr inbounds [2 x i16], ptr %array6, i64 0, i64 0
  store i16 20, ptr %ptr7, align 2
  %x8 = load i16, ptr %x, align 2
  %add = add i16 %x8, 1
  %ptr9 = getelementptr inbounds [2 x i16], ptr %array6, i64 0, i64 1
  store i16 %add, ptr %ptr9, align 2
  %agg10 = insertvalue %"[]i16" { i64 2, ptr undef }, ptr %array6, 1
  store %"[]i16" %agg10, ptr %a3, align 8
  %ptr12 = getelementptr inbounds [2 x i32], ptr %array11, i64 0, i64 0
  store i32 3, ptr %ptr12, align 4
  %y13 = load i32, ptr %y, align 4
  %mul = mul i32 %y13, 2
  %add14 = add i32 1, %mul
  %ptr15 = getelementptr inbounds [2 x i32], ptr %array11, i64 0, i64 1
  store i32 %add14, ptr %ptr15, align 4
  %agg16 = insertvalue %"[]i32" { i64 2, ptr undef }, ptr %array11, 1
  store %"[]i32" %agg16, ptr %a4, align 8
  %ptr18 = getelementptr inbounds [3 x i32], ptr %array17, i64 0, i64 0
  store i32 10, ptr %ptr18, align 4
  %ptr19 = getelementptr inbounds [3 x i32], ptr %array17, i64 0, i64 1
  store i32 20, ptr %ptr19, align 4
  %ptr20 = getelementptr inbounds [3 x i32], ptr %array17, i64 0, i64 2
  store i32 30, ptr %ptr20, align 4
  %agg21 = insertvalue %"[]i32" { i64 3, ptr undef }, ptr %array17, 1
  %call = call %UnitType @arrays1(%"[]i32" %agg21)
  %ptr23 = getelementptr inbounds [3 x i32], ptr %array22, i64 0, i64 0
  store i32 1, ptr %ptr23, align 4
  %ptr24 = getelementptr inbounds [3 x i32], ptr %array22, i64 0, i64 1
  store i32 2, ptr %ptr24, align 4
  %ptr25 = getelementptr inbounds [3 x i32], ptr %array22, i64 0, i64 2
  store i32 3, ptr %ptr25, align 4
  %agg26 = insertvalue %"[]i32" { i64 3, ptr undef }, ptr %array22, 1
  %agg27 = insertvalue %ArrayTest undef, %"[]i32" %agg26, 0
  store %ArrayTest %agg27, ptr %s, align 8
  %ptr29 = getelementptr inbounds [5 x i32], ptr %array28, i64 0, i64 0
  store i32 10, ptr %ptr29, align 4
  %ptr30 = getelementptr inbounds [5 x i32], ptr %array28, i64 0, i64 1
  store i32 20, ptr %ptr30, align 4
  %ptr31 = getelementptr inbounds [5 x i32], ptr %array28, i64 0, i64 2
  store i32 30, ptr %ptr31, align 4
  %ptr32 = getelementptr inbounds [5 x i32], ptr %array28, i64 0, i64 3
  store i32 40, ptr %ptr32, align 4
  %ptr33 = getelementptr inbounds [5 x i32], ptr %array28, i64 0, i64 4
  store i32 50, ptr %ptr33, align 4
  %agg34 = insertvalue %"[]i32" { i64 5, ptr undef }, ptr %array28, 1
  %size = extractvalue %"[]i32" %agg34, 0
  %data = extractvalue %"[]i32" %agg34, 1
  %endok = icmp ult i64 4, %size
  %checkend = select i1 %endok, i64 4, i64 %size
  %startok = icmp ule i64 1, %checkend
  %checkstart = select i1 %startok, i64 1, i64 %checkend
  %sub = sub i64 %checkend, %checkstart
  %ptr35 = getelementptr inbounds i32, ptr %data, i64 %checkstart
  %agg36 = insertvalue %"[]i32" undef, i64 %sub, 0
  %agg37 = insertvalue %"[]i32" %agg36, ptr %ptr35, 1
  %call38 = call %UnitType @arrays1(%"[]i32" %agg37)
  %ptr40 = getelementptr inbounds [2 x i16], ptr %array39, i64 0, i64 0
  store i16 100, ptr %ptr40, align 2
  %ptr41 = getelementptr inbounds [2 x i16], ptr %array39, i64 0, i64 1
  store i16 200, ptr %ptr41, align 2
  %agg42 = insertvalue %"[]i16" { i64 2, ptr undef }, ptr %array39, 1
  %size43 = extractvalue %"[]i16" %agg42, 0
  %check = icmp uge i64 0, %size43
  br i1 %check, label %failed, label %passed

failed:                                           ; preds = %entry
  call void @exit(i32 1)
  unreachable

passed:                                           ; preds = %entry
  %data44 = extractvalue %"[]i16" %agg42, 1
  %value = getelementptr inbounds i16, ptr %data44, i64 0
  %load = load i16, ptr %value, align 2
  store i16 %load, ptr %r1, align 2
  %ptr47 = getelementptr inbounds [2 x i32], ptr %array46, i64 0, i64 0
  store i32 10, ptr %ptr47, align 4
  %ptr48 = getelementptr inbounds [2 x i32], ptr %array46, i64 0, i64 1
  store i32 20, ptr %ptr48, align 4
  %agg49 = insertvalue %"[]i32" { i64 2, ptr undef }, ptr %array46, 1
  %ptr50 = getelementptr inbounds [2 x %"[]i32"], ptr %array45, i64 0, i64 0
  store %"[]i32" %agg49, ptr %ptr50, align 8
  %ptr52 = getelementptr inbounds [2 x i32], ptr %array51, i64 0, i64 0
  store i32 30, ptr %ptr52, align 4
  %ptr53 = getelementptr inbounds [2 x i32], ptr %array51, i64 0, i64 1
  store i32 40, ptr %ptr53, align 4
  %agg54 = insertvalue %"[]i32" { i64 2, ptr undef }, ptr %array51, 1
  %ptr55 = getelementptr inbounds [2 x %"[]i32"], ptr %array45, i64 0, i64 1
  store %"[]i32" %agg54, ptr %ptr55, align 8
  %agg56 = insertvalue %"[][]i32" { i64 2, ptr undef }, ptr %array45, 1
  store %"[][]i32" %agg56, ptr %a6, align 8
  %startPtr = getelementptr inbounds [2 x i32], ptr %array58, i64 0, i64 0
  %endPtr = getelementptr inbounds [2 x i32], ptr %array58, i64 0, i64 2
  br label %fillBody

fillBody:                                         ; preds = %fillBody, %passed
  %phi = phi ptr [ %startPtr, %passed ], [ %nextPtr, %fillBody ]
  store i32 8, ptr %phi, align 4
  %nextPtr = getelementptr inbounds [2 x i32], ptr %phi, i64 1
  %atEnd = icmp eq ptr %nextPtr, %endPtr
  br i1 %atEnd, label %fillExit, label %fillBody

fillExit:                                         ; preds = %fillBody
  %agg59 = insertvalue %"[]i32" { i64 2, ptr undef }, ptr %array58, 1
  %ptr60 = getelementptr inbounds [2 x %"[]i32"], ptr %array57, i64 0, i64 0
  store %"[]i32" %agg59, ptr %ptr60, align 8
  %startPtr63 = getelementptr inbounds [3 x i32], ptr %array61, i64 0, i64 0
  %endPtr64 = getelementptr inbounds [3 x i32], ptr %array61, i64 0, i64 3
  br label %fillBody62

fillBody62:                                       ; preds = %fillBody62, %fillExit
  %phi65 = phi ptr [ %startPtr63, %fillExit ], [ %nextPtr66, %fillBody62 ]
  store i32 9, ptr %phi65, align 4
  %nextPtr66 = getelementptr inbounds [3 x i32], ptr %phi65, i64 1
  %atEnd67 = icmp eq ptr %nextPtr66, %endPtr64
  br i1 %atEnd67, label %fillExit68, label %fillBody62

fillExit68:                                       ; preds = %fillBody62
  %agg69 = insertvalue %"[]i32" { i64 3, ptr undef }, ptr %array61, 1
  %ptr70 = getelementptr inbounds [2 x %"[]i32"], ptr %array57, i64 0, i64 1
  store %"[]i32" %agg69, ptr %ptr70, align 8
  %agg71 = insertvalue %"[][]i32" { i64 2, ptr undef }, ptr %array57, 1
  store %"[][]i32" %agg71, ptr %a7, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @slices(%"[]i32" %array, %str %s, i32 %start, i32 %end) #0 {
entry:
  %s1 = alloca %str, align 8
  %a3 = alloca %"[]i32", align 8
  %a2 = alloca %"[]i32", align 8
  %a1 = alloca %"[]i32", align 8
  %end4 = alloca i32, align 4
  %start3 = alloca i32, align 4
  %s2 = alloca %str, align 8
  %array1 = alloca %"[]i32", align 8
  store %"[]i32" %array, ptr %array1, align 8
  store %str %s, ptr %s2, align 8
  store i32 %start, ptr %start3, align 4
  store i32 %end, ptr %end4, align 4
  %array5 = load %"[]i32", ptr %array1, align 8
  %start6 = load i32, ptr %start3, align 4
  %end7 = load i32, ptr %end4, align 4
  %rng = insertvalue %"RangeClosed'u32'" undef, i32 %start6, 0
  %rng8 = insertvalue %"RangeClosed'u32'" %rng, i32 %end7, 1
  %size = extractvalue %"[]i32" %array5, 0
  %data = extractvalue %"[]i32" %array5, 1
  %start9 = extractvalue %"RangeClosed'u32'" %rng8, 0
  %zeroext = zext i32 %start9 to i64
  %end10 = extractvalue %"RangeClosed'u32'" %rng8, 1
  %zeroext11 = zext i32 %end10 to i64
  %add = add i64 %zeroext11, 1
  %endok = icmp ult i64 %add, %size
  %checkend = select i1 %endok, i64 %add, i64 %size
  %startok = icmp ule i64 %zeroext, %checkend
  %checkstart = select i1 %startok, i64 %zeroext, i64 %checkend
  %sub = sub i64 %checkend, %checkstart
  %ptr = getelementptr inbounds i32, ptr %data, i64 %checkstart
  %agg = insertvalue %"[]i32" undef, i64 %sub, 0
  %agg12 = insertvalue %"[]i32" %agg, ptr %ptr, 1
  store %"[]i32" %agg12, ptr %a1, align 8
  %array13 = load %"[]i32", ptr %array1, align 8
  %start14 = load i32, ptr %start3, align 4
  %end15 = load i32, ptr %end4, align 4
  %rng16 = insertvalue %"RangeHalfOpen'u32'" undef, i32 %start14, 0
  %rng17 = insertvalue %"RangeHalfOpen'u32'" %rng16, i32 %end15, 1
  %size18 = extractvalue %"[]i32" %array13, 0
  %data19 = extractvalue %"[]i32" %array13, 1
  %start20 = extractvalue %"RangeHalfOpen'u32'" %rng17, 0
  %zeroext21 = zext i32 %start20 to i64
  %end22 = extractvalue %"RangeHalfOpen'u32'" %rng17, 1
  %zeroext23 = zext i32 %end22 to i64
  %endok24 = icmp ult i64 %zeroext23, %size18
  %checkend25 = select i1 %endok24, i64 %zeroext23, i64 %size18
  %startok26 = icmp ule i64 %zeroext21, %checkend25
  %checkstart27 = select i1 %startok26, i64 %zeroext21, i64 %checkend25
  %sub28 = sub i64 %checkend25, %checkstart27
  %ptr29 = getelementptr inbounds i32, ptr %data19, i64 %checkstart27
  %agg30 = insertvalue %"[]i32" undef, i64 %sub28, 0
  %agg31 = insertvalue %"[]i32" %agg30, ptr %ptr29, 1
  store %"[]i32" %agg31, ptr %a2, align 8
  %a232 = load %"[]i32", ptr %a2, align 8
  %start33 = load i32, ptr %start3, align 4
  %add34 = add i32 %start33, 1
  %end35 = load i32, ptr %end4, align 4
  %sub36 = sub i32 %end35, 1
  %rng37 = insertvalue %"RangeClosed'u32'" undef, i32 %add34, 0
  %rng38 = insertvalue %"RangeClosed'u32'" %rng37, i32 %sub36, 1
  %size39 = extractvalue %"[]i32" %a232, 0
  %data40 = extractvalue %"[]i32" %a232, 1
  %start41 = extractvalue %"RangeClosed'u32'" %rng38, 0
  %zeroext42 = zext i32 %start41 to i64
  %end43 = extractvalue %"RangeClosed'u32'" %rng38, 1
  %zeroext44 = zext i32 %end43 to i64
  %add45 = add i64 %zeroext44, 1
  %endok46 = icmp ult i64 %add45, %size39
  %checkend47 = select i1 %endok46, i64 %add45, i64 %size39
  %startok48 = icmp ule i64 %zeroext42, %checkend47
  %checkstart49 = select i1 %startok48, i64 %zeroext42, i64 %checkend47
  %sub50 = sub i64 %checkend47, %checkstart49
  %ptr51 = getelementptr inbounds i32, ptr %data40, i64 %checkstart49
  %agg52 = insertvalue %"[]i32" undef, i64 %sub50, 0
  %agg53 = insertvalue %"[]i32" %agg52, ptr %ptr51, 1
  store %"[]i32" %agg53, ptr %a3, align 8
  %s54 = load %str, ptr %s2, align 8
  %start55 = load i32, ptr %start3, align 4
  %end56 = load i32, ptr %end4, align 4
  %rng57 = insertvalue %"RangeClosed'u32'" undef, i32 %start55, 0
  %rng58 = insertvalue %"RangeClosed'u32'" %rng57, i32 %end56, 1
  %size59 = extractvalue %str %s54, 0
  %data60 = extractvalue %str %s54, 1
  %start61 = extractvalue %"RangeClosed'u32'" %rng58, 0
  %zeroext62 = zext i32 %start61 to i64
  %end63 = extractvalue %"RangeClosed'u32'" %rng58, 1
  %zeroext64 = zext i32 %end63 to i64
  %add65 = add i64 %zeroext64, 1
  %endok66 = icmp ult i64 %add65, %size59
  %checkend67 = select i1 %endok66, i64 %add65, i64 %size59
  %startok68 = icmp ule i64 %zeroext62, %checkend67
  %checkstart69 = select i1 %startok68, i64 %zeroext62, i64 %checkend67
  %sub70 = sub i64 %checkend67, %checkstart69
  %ptr71 = getelementptr inbounds i8, ptr %data60, i64 %checkstart69
  %agg72 = insertvalue %str undef, i64 %sub70, 0
  %agg73 = insertvalue %str %agg72, ptr %ptr71, 1
  store %str %agg73, ptr %s1, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @casts(i16 %x, i16 %y, i1 %b, float %z) #0 {
entry:
  %c20 = alloca double, align 8
  %c19 = alloca double, align 8
  %c18 = alloca double, align 8
  %c17 = alloca double, align 8
  %c16 = alloca i8, align 1
  %c15 = alloca i8, align 1
  %c14 = alloca i8, align 1
  %c13 = alloca i8, align 1
  %c12 = alloca i1, align 1
  %c11 = alloca i1, align 1
  %c10 = alloca i1, align 1
  %c9 = alloca i1, align 1
  %c8 = alloca i32, align 4
  %c7 = alloca i32, align 4
  %c6 = alloca i32, align 4
  %c5 = alloca i32, align 4
  %c4 = alloca i32, align 4
  %c3 = alloca i32, align 4
  %c2 = alloca i32, align 4
  %c1 = alloca i32, align 4
  %z4 = alloca float, align 4
  %b3 = alloca i1, align 1
  %y2 = alloca i16, align 2
  %x1 = alloca i16, align 2
  store i16 %x, ptr %x1, align 2
  store i16 %y, ptr %y2, align 2
  store i1 %b, ptr %b3, align 1
  store float %z, ptr %z4, align 4
  %x5 = load i16, ptr %x1, align 2
  %cast = sext i16 %x5 to i32
  store i32 %cast, ptr %c1, align 4
  %y6 = load i16, ptr %y2, align 2
  %cast7 = zext i16 %y6 to i32
  store i32 %cast7, ptr %c2, align 4
  %b8 = load i1, ptr %b3, align 1
  %cast9 = zext i1 %b8 to i32
  store i32 %cast9, ptr %c3, align 4
  %z10 = load float, ptr %z4, align 4
  %cast11 = fptosi float %z10 to i32
  store i32 %cast11, ptr %c4, align 4
  %x12 = load i16, ptr %x1, align 2
  %cast13 = sext i16 %x12 to i32
  store i32 %cast13, ptr %c5, align 4
  %y14 = load i16, ptr %y2, align 2
  %cast15 = zext i16 %y14 to i32
  store i32 %cast15, ptr %c6, align 4
  %b16 = load i1, ptr %b3, align 1
  %cast17 = zext i1 %b16 to i32
  store i32 %cast17, ptr %c7, align 4
  %z18 = load float, ptr %z4, align 4
  %cast19 = fptoui float %z18 to i32
  store i32 %cast19, ptr %c8, align 4
  %x20 = load i16, ptr %x1, align 2
  %cast21 = icmp ne i16 %x20, 0
  store i1 %cast21, ptr %c9, align 1
  %y22 = load i16, ptr %y2, align 2
  %cast23 = icmp ne i16 %y22, 0
  store i1 %cast23, ptr %c10, align 1
  %b24 = load i1, ptr %b3, align 1
  store i1 %b24, ptr %c11, align 1
  %z25 = load float, ptr %z4, align 4
  %cast26 = fcmp one float %z25, 0.000000e+00
  store i1 %cast26, ptr %c12, align 1
  %x27 = load i16, ptr %x1, align 2
  %cast28 = trunc i16 %x27 to i8
  store i8 %cast28, ptr %c13, align 1
  %y29 = load i16, ptr %y2, align 2
  %cast30 = trunc i16 %y29 to i8
  store i8 %cast30, ptr %c14, align 1
  %b31 = load i1, ptr %b3, align 1
  %cast32 = zext i1 %b31 to i8
  store i8 %cast32, ptr %c15, align 1
  %z33 = load float, ptr %z4, align 4
  %cast34 = fptosi float %z33 to i8
  store i8 %cast34, ptr %c16, align 1
  %x35 = load i16, ptr %x1, align 2
  %cast36 = sitofp i16 %x35 to double
  store double %cast36, ptr %c17, align 8
  %y37 = load i16, ptr %y2, align 2
  %cast38 = uitofp i16 %y37 to double
  store double %cast38, ptr %c18, align 8
  %b39 = load i1, ptr %b3, align 1
  %cast40 = uitofp i1 %b39 to double
  store double %cast40, ptr %c19, align 8
  %z41 = load float, ptr %z4, align 4
  %cast42 = fpext float %z41 to double
  store double %cast42, ptr %c20, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @consts() #0 {
entry:
  %array26 = alloca [13 x i32], align 4
  %a1 = alloca %"[]i32", align 8
  %v44 = alloca i64, align 8
  %v43 = alloca i8, align 1
  %v42 = alloca i32, align 4
  %v41 = alloca float, align 4
  %v40 = alloca double, align 8
  %v39 = alloca double, align 8
  %v38 = alloca float, align 4
  %v37 = alloca float, align 4
  %v36 = alloca ptr, align 8
  %array17 = alloca [3 x i16], align 2
  %v35 = alloca %"[]i16", align 8
  %array13 = alloca [2 x i8], align 1
  %v34 = alloca %"[]i8", align 8
  %v33 = alloca %str, align 8
  %v32 = alloca %str, align 8
  %v31 = alloca %str, align 8
  %v30 = alloca %str, align 8
  %v29 = alloca i16, align 2
  %v28 = alloca i8, align 1
  %array7 = alloca [5 x i16], align 2
  %v27 = alloca %"[]i16", align 8
  %array = alloca [3 x i8], align 1
  %v26 = alloca %"[]i8", align 8
  %v25 = alloca i32, align 4
  %v24 = alloca i32, align 4
  %v23 = alloca %"RangeClosed'i16'", align 8
  %v22 = alloca %"RangeClosed'i32'", align 8
  %v21 = alloca i32, align 4
  %v20 = alloca i64, align 8
  %v19 = alloca %Test2, align 8
  %v18 = alloca i16, align 2
  %v17 = alloca i16, align 2
  %v16 = alloca i8, align 1
  %v15 = alloca i8, align 1
  %v14 = alloca i1, align 1
  %v13 = alloca i1, align 1
  %v12 = alloca i1, align 1
  %v11 = alloca i1, align 1
  %v10 = alloca i32, align 4
  %v9 = alloca i8, align 1
  %v8 = alloca i16, align 2
  %v7 = alloca %str, align 8
  %v6 = alloca i32, align 4
  %v5 = alloca i8, align 1
  %v4 = alloca i1, align 1
  %v3 = alloca i32, align 4
  %v2 = alloca i16, align 2
  %v1 = alloca i8, align 1
  store i8 12, ptr %v1, align 1
  store i16 1000, ptr %v2, align 2
  store i32 1000, ptr %v3, align 4
  store i1 true, ptr %v4, align 1
  store i8 9, ptr %v5, align 1
  store i32 122, ptr %v6, align 4
  %load = load %str, ptr @strStruct6, align 8
  store %str %load, ptr %v7, align 8
  store i16 7, ptr %v8, align 2
  store i8 29, ptr %v9, align 1
  store i32 8, ptr %v10, align 4
  store i1 true, ptr %v11, align 1
  store i1 true, ptr %v12, align 1
  store i1 false, ptr %v13, align 1
  store i1 true, ptr %v14, align 1
  store i8 1, ptr %v15, align 1
  store i8 -24, ptr %v16, align 1
  store i16 100, ptr %v17, align 2
  store i16 -1, ptr %v18, align 2
  %load1 = load %str, ptr @strStruct6, align 8
  %agg = insertvalue %Test1 { i32 7, i1 true, %str undef }, %str %load1, 2
  %agg2 = insertvalue %Test2 undef, %Test1 %agg, 0
  %agg3 = insertvalue %Test2 %agg2, i32 1012, 1
  store %Test2 %agg3, ptr %v19, align 8
  store i64 3, ptr %v20, align 8
  store i32 7, ptr %v21, align 4
  store %"RangeClosed'i32'" { i32 12, i32 1000 }, ptr %v22, align 4
  store %"RangeClosed'i16'" { i16 -4, i16 200 }, ptr %v23, align 2
  store i32 12, ptr %v24, align 4
  store i32 500, ptr %v25, align 4
  %ptr = getelementptr inbounds [3 x i8], ptr %array, i64 0, i64 0
  store i8 10, ptr %ptr, align 1
  %ptr4 = getelementptr inbounds [3 x i8], ptr %array, i64 0, i64 1
  store i8 11, ptr %ptr4, align 1
  %ptr5 = getelementptr inbounds [3 x i8], ptr %array, i64 0, i64 2
  store i8 12, ptr %ptr5, align 1
  %agg6 = insertvalue %"[]i8" { i64 3, ptr undef }, ptr %array, 1
  store %"[]i8" %agg6, ptr %v26, align 8
  %startPtr = getelementptr inbounds [5 x i16], ptr %array7, i64 0, i64 0
  %endPtr = getelementptr inbounds [5 x i16], ptr %array7, i64 0, i64 5
  br label %fillBody

fillBody:                                         ; preds = %fillBody, %entry
  %phi = phi ptr [ %startPtr, %entry ], [ %nextPtr, %fillBody ]
  store i16 -100, ptr %phi, align 2
  %nextPtr = getelementptr inbounds [5 x i16], ptr %phi, i64 1
  %atEnd = icmp eq ptr %nextPtr, %endPtr
  br i1 %atEnd, label %fillExit, label %fillBody

fillExit:                                         ; preds = %fillBody
  %agg8 = insertvalue %"[]i16" { i64 5, ptr undef }, ptr %array7, 1
  store %"[]i16" %agg8, ptr %v27, align 8
  store i8 12, ptr %v28, align 1
  store i16 -100, ptr %v29, align 2
  %load9 = load %str, ptr @strStruct7, align 8
  store %str %load9, ptr %v30, align 8
  %load10 = load %str, ptr @strStruct8, align 8
  store %str %load10, ptr %v31, align 8
  %load11 = load %str, ptr @strStruct9, align 8
  store %str %load11, ptr %v32, align 8
  %load12 = load %str, ptr @strStruct0, align 8
  store %str %load12, ptr %v33, align 8
  %ptr14 = getelementptr inbounds [2 x i8], ptr %array13, i64 0, i64 0
  store i8 11, ptr %ptr14, align 1
  %ptr15 = getelementptr inbounds [2 x i8], ptr %array13, i64 0, i64 1
  store i8 12, ptr %ptr15, align 1
  %agg16 = insertvalue %"[]i8" { i64 2, ptr undef }, ptr %array13, 1
  store %"[]i8" %agg16, ptr %v34, align 8
  %startPtr19 = getelementptr inbounds [3 x i16], ptr %array17, i64 0, i64 0
  %endPtr20 = getelementptr inbounds [3 x i16], ptr %array17, i64 0, i64 3
  br label %fillBody18

fillBody18:                                       ; preds = %fillBody18, %fillExit
  %phi21 = phi ptr [ %startPtr19, %fillExit ], [ %nextPtr22, %fillBody18 ]
  store i16 -100, ptr %phi21, align 2
  %nextPtr22 = getelementptr inbounds [3 x i16], ptr %phi21, i64 1
  %atEnd23 = icmp eq ptr %nextPtr22, %endPtr20
  br i1 %atEnd23, label %fillExit24, label %fillBody18

fillExit24:                                       ; preds = %fillBody18
  %agg25 = insertvalue %"[]i16" { i64 3, ptr undef }, ptr %array17, 1
  store %"[]i16" %agg25, ptr %v35, align 8
  store ptr @ftype1, ptr %v36, align 8
  store float 5.000000e-01, ptr %v37, align 4
  store float -2.750000e+00, ptr %v38, align 4
  store double 1.200000e+01, ptr %v39, align 8
  store double 0x426D1A94A2002000, ptr %v40, align 8
  store float 0x426D1A94A0000000, ptr %v41, align 4
  store i32 -2, ptr %v42, align 4
  store i8 -46, ptr %v43, align 1
  store i64 -8446744073709551616, ptr %v44, align 8
  %startPtr28 = getelementptr inbounds [13 x i32], ptr %array26, i64 0, i64 0
  %endPtr29 = getelementptr inbounds [13 x i32], ptr %array26, i64 0, i64 13
  br label %fillBody27

fillBody27:                                       ; preds = %fillBody27, %fillExit24
  %phi30 = phi ptr [ %startPtr28, %fillExit24 ], [ %nextPtr31, %fillBody27 ]
  store i32 0, ptr %phi30, align 4
  %nextPtr31 = getelementptr inbounds [13 x i32], ptr %phi30, i64 1
  %atEnd32 = icmp eq ptr %nextPtr31, %endPtr29
  br i1 %atEnd32, label %fillExit33, label %fillBody27

fillExit33:                                       ; preds = %fillBody27
  %agg34 = insertvalue %"[]i32" { i64 13, ptr undef }, ptr %array26, 1
  store %"[]i32" %agg34, ptr %a1, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ftype1() #0 {
entry:
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ftype2() #0 {
entry:
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define i32 @ftype3() #0 {
entry:
  ret i32 3
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ftype4(i32 %x) #0 {
entry:
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @ftype5(i32 %x, i1 %y) #0 {
entry:
  %y2 = alloca i1, align 1
  %x1 = alloca i32, align 4
  store i32 %x, ptr %x1, align 4
  store i1 %y, ptr %y2, align 1
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define ptr @ftype6() #0 {
entry:
  ret ptr @ftype3
}

; Function Attrs: noinline nounwind optnone
define %UnitType @function_type() #0 {
entry:
  %x = alloca i32, align 4
  %f7 = alloca ptr, align 8
  %f6 = alloca ptr, align 8
  %f5 = alloca ptr, align 8
  %f4 = alloca ptr, align 8
  %f3 = alloca ptr, align 8
  %f2 = alloca ptr, align 8
  %f1 = alloca ptr, align 8
  store ptr @ftype1, ptr %f1, align 8
  %f11 = load ptr, ptr %f1, align 8
  %call = call %UnitType %f11()
  store ptr @ftype1, ptr %f2, align 8
  store ptr @ftype2, ptr %f2, align 8
  %f12 = load ptr, ptr %f1, align 8
  store ptr %f12, ptr %f2, align 8
  %f23 = load ptr, ptr %f2, align 8
  %call4 = call %UnitType %f23()
  store ptr @ftype3, ptr %f3, align 8
  %f35 = load ptr, ptr %f3, align 8
  %call6 = call i32 %f35()
  store ptr @ftype4, ptr %f4, align 8
  %f47 = load ptr, ptr %f4, align 8
  %call8 = call %UnitType %f47(i32 123)
  store ptr @ftype5, ptr %f5, align 8
  %f59 = load ptr, ptr %f5, align 8
  %call10 = call %UnitType %f59(i32 456, i1 true)
  store ptr @ftype6, ptr %f6, align 8
  %f611 = load ptr, ptr %f6, align 8
  %call12 = call ptr %f611()
  store ptr %call12, ptr %f7, align 8
  %f713 = load ptr, ptr %f7, align 8
  %call14 = call i32 %f713()
  %f615 = load ptr, ptr %f6, align 8
  %call16 = call ptr %f615()
  %call17 = call i32 %call16()
  store i32 %call17, ptr %x, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @shift() #0 {
entry:
  %z = alloca i64, align 8
  %y = alloca i16, align 2
  %x = alloca i32, align 4
  store i32 1, ptr %x, align 4
  store i16 2, ptr %y, align 2
  store i64 3, ptr %z, align 8
  %load = load i32, ptr %x, align 4
  %shl = shl i32 %load, 1
  store i32 %shl, ptr %x, align 4
  %y1 = load i16, ptr %y, align 2
  %load2 = load i32, ptr %x, align 4
  %zeroext = zext i16 %y1 to i32
  %shl3 = shl i32 %load2, %zeroext
  store i32 %shl3, ptr %x, align 4
  %load4 = load i32, ptr %x, align 4
  %lshr = lshr i32 %load4, 1
  store i32 %lshr, ptr %x, align 4
  %y5 = load i16, ptr %y, align 2
  %load6 = load i32, ptr %x, align 4
  %zeroext7 = zext i16 %y5 to i32
  %lshr8 = lshr i32 %load6, %zeroext7
  store i32 %lshr8, ptr %x, align 4
  %load9 = load i32, ptr %x, align 4
  %ashr = ashr i32 %load9, 1
  store i32 %ashr, ptr %x, align 4
  %y10 = load i16, ptr %y, align 2
  %load11 = load i32, ptr %x, align 4
  %zeroext12 = zext i16 %y10 to i32
  %ashr13 = ashr i32 %load11, %zeroext12
  store i32 %ashr13, ptr %x, align 4
  %x14 = load i32, ptr %x, align 4
  %shl15 = shl i32 %x14, 1
  store i32 %shl15, ptr %x, align 4
  %x16 = load i32, ptr %x, align 4
  %y17 = load i16, ptr %y, align 2
  %zeroext18 = zext i16 %y17 to i32
  %shl19 = shl i32 %x16, %zeroext18
  store i32 %shl19, ptr %x, align 4
  %x20 = load i32, ptr %x, align 4
  %lshr21 = lshr i32 %x20, 1
  store i32 %lshr21, ptr %x, align 4
  %x22 = load i32, ptr %x, align 4
  %y23 = load i16, ptr %y, align 2
  %zeroext24 = zext i16 %y23 to i32
  %lshr25 = lshr i32 %x22, %zeroext24
  store i32 %lshr25, ptr %x, align 4
  %x26 = load i32, ptr %x, align 4
  %ashr27 = ashr i32 %x26, 1
  store i32 %ashr27, ptr %x, align 4
  %x28 = load i32, ptr %x, align 4
  %y29 = load i16, ptr %y, align 2
  %zeroext30 = zext i16 %y29 to i32
  %ashr31 = ashr i32 %x28, %zeroext30
  store i32 %ashr31, ptr %x, align 4
  store i32 8, ptr %x, align 4
  %y32 = load i16, ptr %y, align 2
  %zeroext33 = zext i16 %y32 to i32
  %shl34 = shl i32 4, %zeroext33
  store i32 %shl34, ptr %x, align 4
  store i32 2, ptr %x, align 4
  %y35 = load i16, ptr %y, align 2
  %zeroext36 = zext i16 %y35 to i32
  %lshr37 = lshr i32 4, %zeroext36
  store i32 %lshr37, ptr %x, align 4
  store i32 2, ptr %x, align 4
  %y38 = load i16, ptr %y, align 2
  %zeroext39 = zext i16 %y38 to i32
  %ashr40 = ashr i32 4, %zeroext39
  store i32 %ashr40, ptr %x, align 4
  %z41 = load i64, ptr %z, align 8
  %load42 = load i32, ptr %x, align 4
  %signext = sext i32 %load42 to i64
  %shl43 = shl i64 %signext, %z41
  %trunc = trunc i64 %shl43 to i32
  store i32 %trunc, ptr %x, align 4
  %x44 = load i32, ptr %x, align 4
  %z45 = load i64, ptr %z, align 8
  %signext46 = sext i32 %x44 to i64
  %lshr47 = lshr i64 %signext46, %z45
  %trunc48 = trunc i64 %lshr47 to i32
  store i32 %trunc48, ptr %x, align 4
  %load49 = load i32, ptr %x, align 4
  %signext50 = sext i32 %load49 to i64
  %shl51 = shl i64 %signext50, 1000000000000
  %trunc52 = trunc i64 %shl51 to i32
  store i32 %trunc52, ptr %x, align 4
  %x53 = load i32, ptr %x, align 4
  %signext54 = sext i32 %x53 to i64
  %ashr55 = ashr i64 %signext54, 1000000000000
  %trunc56 = trunc i64 %ashr55 to i32
  store i32 %trunc56, ptr %x, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @types() #0 {
entry:
  %v = alloca i32, align 4
  %f2 = alloca ptr, align 8
  %f1 = alloca ptr, align 8
  %f0 = alloca ptr, align 8
  %t6 = alloca %"<struct29>", align 8
  %t5 = alloca %TypesTest, align 8
  %t4 = alloca ptr, align 8
  %t3 = alloca i32, align 4
  %array = alloca [2 x %TypesTest], align 8
  %t2 = alloca %"[]TypesTest", align 8
  %t1 = alloca ptr, align 8
  %t0 = alloca %TypesTest, align 8
  store %TypesTest undef, ptr %t0, align 1
  store ptr %t0, ptr %t1, align 8
  %t01 = load %TypesTest, ptr %t0, align 1
  %ptr = getelementptr inbounds [2 x %TypesTest], ptr %array, i64 0, i64 0
  store %TypesTest %t01, ptr %ptr, align 1
  %ptr2 = getelementptr inbounds [2 x %TypesTest], ptr %array, i64 0, i64 1
  store %TypesTest undef, ptr %ptr2, align 1
  %agg = insertvalue %"[]TypesTest" { i64 2, ptr undef }, ptr %array, 1
  store %"[]TypesTest" %agg, ptr %t2, align 8
  store i32 10, ptr %t3, align 4
  store ptr %t3, ptr %t4, align 8
  store %TypesTest undef, ptr %t5, align 1
  store %"<struct29>" { i32 2, i16 1847 }, ptr %t6, align 4
  store ptr @ftype1, ptr %f0, align 8
  store ptr @ftype3, ptr %f1, align 8
  store ptr @oneArg, ptr %f2, align 8
  store i32 1234, ptr %v, align 4
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define float @fOpOrder1(float %a, float %b, float %c, float %d) #0 {
entry:
  %d4 = alloca float, align 4
  %c3 = alloca float, align 4
  %b2 = alloca float, align 4
  %a1 = alloca float, align 4
  store float %a, ptr %a1, align 4
  store float %b, ptr %b2, align 4
  store float %c, ptr %c3, align 4
  store float %d, ptr %d4, align 4
  %a5 = load float, ptr %a1, align 4
  %b6 = load float, ptr %b2, align 4
  %c7 = load float, ptr %c3, align 4
  %mul = fmul float %b6, %c7
  %add = fadd float %a5, %mul
  %d8 = load float, ptr %d4, align 4
  %sub = fsub float %add, %d8
  ret float %sub
}

; Function Attrs: noinline nounwind optnone
define %UnitType @fCmp(float %a, float %b, double %c, double %d) #0 {
entry:
  %b12 = alloca i1, align 1
  %b1132 = alloca i1, align 1
  %b10 = alloca i1, align 1
  %b925 = alloca i1, align 1
  %b8 = alloca i1, align 1
  %b7 = alloca i1, align 1
  %b616 = alloca i1, align 1
  %b5 = alloca i1, align 1
  %b4 = alloca i1, align 1
  %b3 = alloca i1, align 1
  %b27 = alloca i1, align 1
  %b1 = alloca i1, align 1
  %d4 = alloca double, align 8
  %c3 = alloca double, align 8
  %b2 = alloca float, align 4
  %a1 = alloca float, align 4
  store float %a, ptr %a1, align 4
  store float %b, ptr %b2, align 4
  store double %c, ptr %c3, align 8
  store double %d, ptr %d4, align 8
  %a5 = load float, ptr %a1, align 4
  %b6 = load float, ptr %b2, align 4
  %cmpeq = fcmp oeq float %a5, %b6
  store i1 %cmpeq, ptr %b1, align 1
  %a8 = load float, ptr %a1, align 4
  %b9 = load float, ptr %b2, align 4
  %cmpne = fcmp one float %a8, %b9
  store i1 %cmpne, ptr %b27, align 1
  %a10 = load float, ptr %a1, align 4
  %b11 = load float, ptr %b2, align 4
  %cmplt = fcmp olt float %a10, %b11
  store i1 %cmplt, ptr %b3, align 1
  %a12 = load float, ptr %a1, align 4
  %b13 = load float, ptr %b2, align 4
  %cmple = fcmp ole float %a12, %b13
  store i1 %cmple, ptr %b4, align 1
  %a14 = load float, ptr %a1, align 4
  %b15 = load float, ptr %b2, align 4
  %cmpgt = fcmp ogt float %a14, %b15
  store i1 %cmpgt, ptr %b5, align 1
  %a17 = load float, ptr %a1, align 4
  %b18 = load float, ptr %b2, align 4
  %cmpge = fcmp oge float %a17, %b18
  store i1 %cmpge, ptr %b616, align 1
  %c19 = load double, ptr %c3, align 8
  %d20 = load double, ptr %d4, align 8
  %cmpeq21 = fcmp oeq double %c19, %d20
  store i1 %cmpeq21, ptr %b7, align 1
  %c22 = load double, ptr %c3, align 8
  %d23 = load double, ptr %d4, align 8
  %cmpne24 = fcmp one double %c22, %d23
  store i1 %cmpne24, ptr %b8, align 1
  %c26 = load double, ptr %c3, align 8
  %d27 = load double, ptr %d4, align 8
  %cmplt28 = fcmp olt double %c26, %d27
  store i1 %cmplt28, ptr %b925, align 1
  %c29 = load double, ptr %c3, align 8
  %d30 = load double, ptr %d4, align 8
  %cmple31 = fcmp ole double %c29, %d30
  store i1 %cmple31, ptr %b10, align 1
  %c33 = load double, ptr %c3, align 8
  %d34 = load double, ptr %d4, align 8
  %cmpgt35 = fcmp ogt double %c33, %d34
  store i1 %cmpgt35, ptr %b1132, align 1
  %c36 = load double, ptr %c3, align 8
  %d37 = load double, ptr %d4, align 8
  %cmpge38 = fcmp oge double %c36, %d37
  store i1 %cmpge38, ptr %b12, align 1
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @fOpAssign(float %a, double %b) #0 {
entry:
  %y = alloca double, align 8
  %x = alloca float, align 4
  %b2 = alloca double, align 8
  %a1 = alloca float, align 4
  store float %a, ptr %a1, align 4
  store double %b, ptr %b2, align 8
  %a3 = load float, ptr %a1, align 4
  store float %a3, ptr %x, align 4
  %b4 = load double, ptr %b2, align 8
  store double %b4, ptr %y, align 8
  %a5 = load float, ptr %a1, align 4
  %load = load float, ptr %x, align 4
  %add = fadd float %load, %a5
  store float %add, ptr %x, align 4
  %a6 = load float, ptr %a1, align 4
  %load7 = load float, ptr %x, align 4
  %sub = fsub float %load7, %a6
  store float %sub, ptr %x, align 4
  %a8 = load float, ptr %a1, align 4
  %load9 = load float, ptr %x, align 4
  %mul = fmul float %load9, %a8
  store float %mul, ptr %x, align 4
  %a10 = load float, ptr %a1, align 4
  %load11 = load float, ptr %x, align 4
  %div = fdiv float %load11, %a10
  store float %div, ptr %x, align 4
  %a12 = load float, ptr %a1, align 4
  %load13 = load float, ptr %x, align 4
  %rem = frem float %load13, %a12
  store float %rem, ptr %x, align 4
  %b14 = load double, ptr %b2, align 8
  %load15 = load double, ptr %y, align 8
  %add16 = fadd double %load15, %b14
  store double %add16, ptr %y, align 8
  %b17 = load double, ptr %b2, align 8
  %load18 = load double, ptr %y, align 8
  %sub19 = fsub double %load18, %b17
  store double %sub19, ptr %y, align 8
  %b20 = load double, ptr %b2, align 8
  %load21 = load double, ptr %y, align 8
  %mul22 = fmul double %load21, %b20
  store double %mul22, ptr %y, align 8
  %b23 = load double, ptr %b2, align 8
  %load24 = load double, ptr %y, align 8
  %div25 = fdiv double %load24, %b23
  store double %div25, ptr %y, align 8
  %b26 = load double, ptr %b2, align 8
  %load27 = load double, ptr %y, align 8
  %rem28 = frem double %load27, %b26
  store double %rem28, ptr %y, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @fLiterals() #0 {
entry:
  %d = alloca double, align 8
  %c = alloca double, align 8
  %b = alloca double, align 8
  %a = alloca float, align 4
  store float 0x3FF3C08320000000, ptr %a, align 4
  store double 1.000000e-09, ptr %b, align 8
  store double 0x40FE26F333333333, ptr %c, align 8
  %b1 = load double, ptr %b, align 8
  %c2 = load double, ptr %c, align 8
  %add = fadd double %b1, %c2
  %add3 = fadd double %add, 1.000000e+00
  store double %add3, ptr %d, align 8
  ret %UnitType zeroinitializer
}

; Function Attrs: noinline nounwind optnone
define %UnitType @builtIns() #0 {
entry:
  %ptr = alloca ptr, align 8
  %addr = alloca i64, align 8
  %c = alloca i16, align 2
  %b = alloca float, align 4
  %a = alloca i64, align 8
  %n = alloca i32, align 4
  %y = alloca double, align 8
  %x = alloca float, align 4
  store float 0x400921FB60000000, ptr %x, align 4
  %x1 = load float, ptr %x, align 4
  %add = fadd float 0x4005BF0A80000000, %x1
  %fpext = fpext float %add to double
  %add2 = fadd double %fpext, 0x401921FB54442D18
  store double %add2, ptr %y, align 8
  store i32 19088743, ptr %n, align 4
  store i64 4614256656552045848, ptr %a, align 8
  %n3 = load i32, ptr %n, align 4
  %cast = bitcast i32 %n3 to float
  store float %cast, ptr %b, align 4
  store i16 1, ptr %c, align 2
  %cast4 = ptrtoint ptr %n to i64
  store i64 %cast4, ptr %addr, align 8
  %addr5 = load i64, ptr %addr, align 8
  %cast6 = inttoptr i64 %addr5 to ptr
  store ptr %cast6, ptr %ptr, align 8
  ret %UnitType zeroinitializer
}

declare void @exit(i32)

attributes #0 = { noinline nounwind optnone }
